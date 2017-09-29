////////////////////////////////////////////////////////////////////////////////////////////////////////
// Part of Injectable Generic Camera System
// Copyright(c) 2017, Frans Bouma
// All rights reserved.
// https://github.com/FransBouma/InjectableGenericCameraSystem
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
//
//  * Redistributions of source code must retain the above copyright notice, this
//	  list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and / or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CameraManipulator.h"
#include "GameConstants.h"
#include "InterceptorHelper.h"
#include "Globals.h"
#include "OverlayConsole.h"

using namespace DirectX;
using namespace std;

extern "C" {
	LPBYTE g_cameraStructAddress = nullptr;
	LPBYTE g_cameraCutsceneStructAddress = nullptr;
	LPBYTE g_timestopStructAddress = nullptr;
	LPBYTE g_gamespeedStructAddress = nullptr;
	LPBYTE g_dofStructAddress = nullptr;
	LPBYTE g_todStructAddress = nullptr;
	LPBYTE g_weatherStructAddress = nullptr;
}

namespace IGCS::GameSpecific::CameraManipulator
{
	static float _originalQuaternion[4];
	static float _originalCutsceneQuaternion[4];
	static float _originalCoords[3];
	static float _originalCutsceneCoords[3];
	static float _originalFov;
	static float _originalCutsceneFov;
	static int _originalToD=12;
	static int _originalWeatherA = 0;
	static int _originalWeatherB = 0;
	static float _originalWeatherIntensity=1.0f;

	void getSettingsFromGameState()
	{
		Settings& currentSettings = Globals::instance().settings();
		if (nullptr != g_dofStructAddress)
		{
			float* apertureInMemory = reinterpret_cast<float*>(g_dofStructAddress + DOF_APERTURE_IN_STRUCT_OFFSET);
			currentSettings.dofAperture = *apertureInMemory;
			float* focusDistanceInMemory = reinterpret_cast<float*>(g_dofStructAddress + DOF_FOCUS_DISTANCE_IN_STRUCT_OFFSET);
			currentSettings.dofDistance = *focusDistanceInMemory;
			float* focalLengthInMemory = reinterpret_cast<float*>(g_dofStructAddress + DOF_FOCAL_LENGTH_IN_STRUCT_OFFSET);
			currentSettings.dofFocalLength = *focalLengthInMemory;
		}
		if (nullptr != g_todStructAddress)
		{
			int* todInMemory = reinterpret_cast<int*>(g_todStructAddress + TOD_IN_STRUCT_OFFSET);
			int currentTodInSeconds = (*todInMemory);
			currentSettings.todHour = currentTodInSeconds / 3600; 
			currentSettings.todMinute = (currentTodInSeconds - (currentSettings.todHour * 3600)) / 60;
		}
		if (nullptr != g_weatherStructAddress)
		{
			int* weatherInMemory = reinterpret_cast<int*>(g_weatherStructAddress + WEATHER_DIRECT_IN_STRUCT_OFFSET);
			currentSettings.weatherA = weatherInMemory[0];
			currentSettings.weatherB = weatherInMemory[2];			// 3 values, 0 and 2 are 2 weathers you can blend. 1 is transition effect
			float* weatherIntensityInMemory = reinterpret_cast<float*>(g_weatherStructAddress + WEATHER_INTENSITY_IN_STRUCT_OFFSET);
			currentSettings.weatherIntensity = *weatherIntensityInMemory;
		}
	}


	void applySettingsToGameState()
	{
		Settings& currentSettings = Globals::instance().settings();
		if (nullptr != g_dofStructAddress)
		{
			float* apertureInMemory = reinterpret_cast<float*>(g_dofStructAddress + DOF_APERTURE_IN_STRUCT_OFFSET);
			*apertureInMemory = currentSettings.dofAperture;
			float* focusDistanceInMemory = reinterpret_cast<float*>(g_dofStructAddress + DOF_FOCUS_DISTANCE_IN_STRUCT_OFFSET);
			*focusDistanceInMemory = currentSettings.dofDistance;
			float* focalLengthInMemory = reinterpret_cast<float*>(g_dofStructAddress + DOF_FOCAL_LENGTH_IN_STRUCT_OFFSET);
			*focalLengthInMemory = currentSettings.dofFocalLength;
		}
		if (g_cameraEnabled)
		{
			if (nullptr != g_todStructAddress)
			{
				int todInSeconds = ((currentSettings.todHour % 24) * 3600) + ((currentSettings.todMinute % 60) * 60);
				int* todInMemory = reinterpret_cast<int*>(g_todStructAddress + TOD_IN_STRUCT_OFFSET);
				*todInMemory = todInSeconds;
			}
			writeWeatherValue(currentSettings.weatherA, currentSettings.weatherB, currentSettings.weatherIntensity);
		}
	}

	
	// newValue: 1 == time should be frozen, 0 == normal gameplay
	// returns true if the game was stopped by this call, false if the game was either already stopped or the state didn't change.
	bool setTimeStopValue(byte newValue)
	{
		if (nullptr == g_timestopStructAddress)
		{
			return false;
		}
		byte* timestopAddress = (g_timestopStructAddress + TIMESTOP_IN_STRUCT_OFFSET);
		bool toReturn = *timestopAddress == (byte)0 && (newValue == (byte)1);
		*timestopAddress = newValue;
		return toReturn;
	}


	// Resets the FOV to the one it got when we enabled the camera
	void resetFoV()
	{
		float* fovInMemory = nullptr;
		if (isCameraFound())
		{
			fovInMemory = reinterpret_cast<float*>(g_cameraStructAddress + FOV_IN_STRUCT_OFFSET);
			*fovInMemory = _originalFov;
		}
		if (isCutsceneCameraFound())
		{
			fovInMemory = reinterpret_cast<float*>(g_cameraCutsceneStructAddress + FOV_CUTSCENE_IN_STRUCT_OFFSET);
			*fovInMemory = _originalCutsceneFov;
		}
	}


	// changes the FoV with the specified amount
	void changeFoV(float amount)
	{
		// fov is in focal length, so the higher the number the more zoomed in, hence the negation of the value.
		float* fovInMemory = nullptr;
		if (isCameraFound())
		{
			fovInMemory = reinterpret_cast<float*>(g_cameraStructAddress + FOV_IN_STRUCT_OFFSET);
			*fovInMemory -= amount;
		}
		if (isCutsceneCameraFound())
		{
			fovInMemory = reinterpret_cast<float*>(g_cameraCutsceneStructAddress + FOV_CUTSCENE_IN_STRUCT_OFFSET);
			*fovInMemory -= amount;
		}
	}


	XMFLOAT3 getCurrentCameraCoords()
	{
		float* coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + COORDS_IN_STRUCT_OFFSET);
		return XMFLOAT3(coordsInMemory[0], coordsInMemory[1], coordsInMemory[2]);
	}
	

	XMFLOAT3 getCurrentCutsceneCameraCoords()
	{
		float* coordsInMemory = reinterpret_cast<float*>(g_cameraCutsceneStructAddress + COORDS_CUTSCENE_IN_STRUCT_OFFSET);
		return XMFLOAT3(coordsInMemory[0], coordsInMemory[1], coordsInMemory[2]);
	}


	// newLookQuaternion: newly calculated quaternion of camera view space. Can be used to construct a 4x4 matrix if the game uses a matrix instead of a quaternion
	// newCoords are the new coordinates for the camera in worldspace.
	void writeNewCameraValuesToGameData(XMFLOAT3 newCoords, XMVECTOR newLookQuaternion, bool cutsceneCamera)
	{
		float* coordsInMemory = nullptr;
		float* quaternionInMemory = nullptr;
		if (cutsceneCamera)
		{
			if (nullptr == g_cameraCutsceneStructAddress)
			{
				return;
			}
			coordsInMemory = reinterpret_cast<float*>(g_cameraCutsceneStructAddress + COORDS_CUTSCENE_IN_STRUCT_OFFSET);
			quaternionInMemory = reinterpret_cast<float*>(g_cameraCutsceneStructAddress + QUATERNION_CUTSCENE_IN_STRUCT_OFFSET);
		}
		else
		{
			if (nullptr == g_cameraStructAddress)
			{
				return;
			}
			coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + COORDS_IN_STRUCT_OFFSET);
			quaternionInMemory = reinterpret_cast<float*>(g_cameraStructAddress + QUATERNION_IN_STRUCT_OFFSET);
		}
		if (nullptr==coordsInMemory)
		{
			return;
		}
		if (nullptr == quaternionInMemory)
		{
			return;
		}
		coordsInMemory[0] = newCoords.x;
		coordsInMemory[1] = newCoords.y;
		coordsInMemory[2] = newCoords.z;

		XMFLOAT4 qAsFloat4;
		XMStoreFloat4(&qAsFloat4, newLookQuaternion);
		quaternionInMemory[0] = qAsFloat4.x;
		quaternionInMemory[1] = qAsFloat4.y;
		quaternionInMemory[2] = qAsFloat4.z;
		quaternionInMemory[3] = qAsFloat4.w;
	}


	bool isCameraFound()
	{
		return nullptr != g_cameraStructAddress;
	}


	bool isCutsceneCameraFound()
	{
		return nullptr != g_cameraCutsceneStructAddress;
	}


	void displayCameraStructAddress()
	{
		OverlayConsole::instance().logDebug("Camera struct address: %p", (void*)g_cameraStructAddress);
	}
	

	// should restore the camera values in the camera structures to the cached values. This assures the free camera is always enabled at the original camera location.
	void restoreOriginalValuesAfterCameraDisable()
	{
		float* coordsInMemory = nullptr;
		float* quaternionInMemory = nullptr;
		float* fovInMemory = nullptr;
		if (isCameraFound())
		{
			coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + COORDS_IN_STRUCT_OFFSET);
			memcpy(coordsInMemory, _originalCoords, 3 * sizeof(float));
			quaternionInMemory = reinterpret_cast<float*>(g_cameraStructAddress + QUATERNION_IN_STRUCT_OFFSET);
			memcpy(quaternionInMemory, _originalQuaternion, 4 * sizeof(float));
			fovInMemory = reinterpret_cast<float*>(g_cameraStructAddress + FOV_IN_STRUCT_OFFSET);
			*fovInMemory = _originalFov;
		}
		if (isCutsceneCameraFound())
		{
			coordsInMemory = reinterpret_cast<float*>(g_cameraCutsceneStructAddress + COORDS_CUTSCENE_IN_STRUCT_OFFSET);
			memcpy(coordsInMemory, _originalCutsceneCoords, 3 * sizeof(float));
			quaternionInMemory = reinterpret_cast<float*>(g_cameraCutsceneStructAddress + QUATERNION_CUTSCENE_IN_STRUCT_OFFSET);
			memcpy(quaternionInMemory, _originalCutsceneQuaternion, 4 * sizeof(float));
			fovInMemory = reinterpret_cast<float*>(g_cameraCutsceneStructAddress + FOV_CUTSCENE_IN_STRUCT_OFFSET);
			*fovInMemory = _originalCutsceneFov;
		}
		if (nullptr != g_todStructAddress)
		{
			int* todInMemory = reinterpret_cast<int*>(g_todStructAddress + TOD_IN_STRUCT_OFFSET);
			*todInMemory = _originalToD;
		}
		writeWeatherValue(_originalWeatherA, _originalWeatherB, _originalWeatherIntensity);
		
		// in theory we should reset the cutscene camera pointer to null as it could be the next time the user enables it, it might be somewhere else and the interception
		// hasn't ran yet. This is such an edge case that we leave it for now, as otherwise the camera can't be enabled twice when the game is paused. 
	}


	void cacheOriginalValuesBeforeCameraEnable()
	{
		float* coordsInMemory = nullptr;
		float* quaternionInMemory = nullptr;
		float* fovInMemory = nullptr;
		if (isCameraFound())
		{
			coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + COORDS_IN_STRUCT_OFFSET);
			memcpy(_originalCoords, coordsInMemory, 3 * sizeof(float));
			quaternionInMemory = reinterpret_cast<float*>(g_cameraStructAddress + QUATERNION_IN_STRUCT_OFFSET);
			memcpy(_originalQuaternion, quaternionInMemory, 4 * sizeof(float));
			fovInMemory = reinterpret_cast<float*>(g_cameraStructAddress + FOV_IN_STRUCT_OFFSET);
			_originalFov = *fovInMemory;
		}
		if (isCutsceneCameraFound())
		{
			coordsInMemory = reinterpret_cast<float*>(g_cameraCutsceneStructAddress + COORDS_CUTSCENE_IN_STRUCT_OFFSET);
			memcpy(_originalCutsceneCoords, coordsInMemory, 3 * sizeof(float));
			quaternionInMemory = reinterpret_cast<float*>(g_cameraCutsceneStructAddress + QUATERNION_CUTSCENE_IN_STRUCT_OFFSET);
			memcpy(_originalCutsceneQuaternion, quaternionInMemory, 4 * sizeof(float));
			fovInMemory = reinterpret_cast<float*>(g_cameraCutsceneStructAddress + FOV_CUTSCENE_IN_STRUCT_OFFSET);
			_originalCutsceneFov = *fovInMemory;
		}
		if (nullptr != g_todStructAddress)
		{
			int* todInMemory = reinterpret_cast<int*>(g_todStructAddress + TOD_IN_STRUCT_OFFSET);
			_originalToD = *todInMemory;
		}
		if (nullptr != g_weatherStructAddress)
		{
			int* weatherInMemory = reinterpret_cast<int*>(g_weatherStructAddress + WEATHER_DIRECT_IN_STRUCT_OFFSET);
			_originalWeatherA = weatherInMemory[0];
			_originalWeatherB = weatherInMemory[2];
			float* weatherIntensityInMemory = reinterpret_cast<float*>(g_weatherStructAddress + WEATHER_INTENSITY_IN_STRUCT_OFFSET);
			*weatherIntensityInMemory = _originalWeatherIntensity;
		}
	}


	void writeWeatherValue(int newWeatherValueA, int newWeatherValueB, float newWeatherIntensity)
	{
		if (nullptr != g_weatherStructAddress)
		{
			int* weatherInMemory = reinterpret_cast<int*>(g_weatherStructAddress + WEATHER_DIRECT_IN_STRUCT_OFFSET);
			weatherInMemory[0] = (newWeatherValueA % 5);
			weatherInMemory[2] = (newWeatherValueB % 5);
			float* weatherIntensityInMemory = reinterpret_cast<float*>(g_weatherStructAddress + WEATHER_INTENSITY_IN_STRUCT_OFFSET);
			*weatherIntensityInMemory = newWeatherIntensity < 0.0f ? 1.0f : newWeatherIntensity;
		}
	}
}