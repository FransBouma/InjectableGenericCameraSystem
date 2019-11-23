////////////////////////////////////////////////////////////////////////////////////////////////////////
// Part of Injectable Generic Camera System
// Copyright(c) 2019, Frans Bouma
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
#include "Camera.h"
#include "GameCameraData.h"

using namespace DirectX;
using namespace std;

extern "C" {
	LPBYTE g_cameraStructAddress = nullptr;
	LPBYTE g_fovStructAddress = nullptr;
}

namespace IGCS::GameSpecific::CameraManipulator
{
	static GameCameraData _originalData;
	static GameCameraData _preMultiShotData;
	static float _currentCameraCoords[3];

	void updateCameraDataInGameData(Camera& camera)
	{
		if (!g_cameraEnabled)
		{
			return;
		}

		// calculate new camera values.
		DirectX::XMVECTOR newLookQuaternion = camera.calculateLookQuaternion();
		DirectX::XMFLOAT3 currentCoords;
		DirectX::XMFLOAT3 newCoords;
		if (isCameraFound())
		{
			currentCoords = getCurrentCameraCoords();
			newCoords = camera.calculateNewCoords(currentCoords, newLookQuaternion);
			float convertFactor = 180.0f / XM_PI;
			writeNewCameraValuesToGameData(newCoords, (camera.getPitch() * convertFactor), (camera.getYaw() * convertFactor), (camera.getRoll() * convertFactor));
		}
	}


	void getSettingsFromGameState()
	{
		Settings& currentSettings = Globals::instance().settings();
	}


	void applySettingsToGameState()
	{
		Settings& currentSettings = Globals::instance().settings();
	}


	// Resets the FOV to the one it got when we enabled the camera
	void resetFoV()
	{
		if (g_fovStructAddress == nullptr)
		{
			return;
		}
		float* fovAddress = reinterpret_cast<float*>(g_fovStructAddress + FOV_IN_STRUCT_OFFSET);
		*fovAddress = _originalData._fov;
	}


	// changes the FoV with the specified amount
	void changeFoV(float amount)
	{
		if (g_fovStructAddress == nullptr)
		{
			return;
		}
		float* fovAddress = reinterpret_cast<float*>(g_fovStructAddress + FOV_IN_STRUCT_OFFSET);
		float currentValue = getCurrentFoV();
		float newValue = currentValue + amount;
		if (newValue < 0.001f)
		{
			// clamp. Game will crash with negative fov
			newValue = 0.001f;
		}
		*fovAddress = newValue;
	}


	float getCurrentFoV()
	{
		if (nullptr == g_fovStructAddress)
		{
			return DEFAULT_FOV_DEGREES;
		}
		float* fovAddress = reinterpret_cast<float*>(g_fovStructAddress + FOV_IN_STRUCT_OFFSET);
		float toReturn = *fovAddress;
		if (toReturn <= 0.001f)
		{
			// not set. Read fov from camera struct
			if (nullptr == g_cameraStructAddress)
			{
				toReturn = DEFAULT_FOV_DEGREES;
			}
			else
			{
				toReturn = *(reinterpret_cast<float*>(g_cameraStructAddress + FOV_IN_CAMERASTRUCT_OFFSET));
			}
		}
		return toReturn;
	}


	XMFLOAT3 getCurrentCameraCoords()
	{
		return XMFLOAT3(_currentCameraCoords[0], _currentCameraCoords[1], _currentCameraCoords[2]);
	}


	// newCoords are the new coordinates for the camera in worldspace.
	// yaw, pitch, roll are the angles used for the rotation as they're now defined in the camera. These angles have to be written as floats (in degrees)
	// pitch, yaw, roll (they store it as 'x rotation', 'y rotation', 'z rotation'). 
	void writeNewCameraValuesToGameData(XMFLOAT3 newCoords, float pitch, float yaw, float roll)
	{
		if (!isCameraFound())
		{
			return;
		}
		// this camera is stored as 3 coords (floats) and 3 angles (floats). 
		float* coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + COORDS_IN_STRUCT_OFFSET);
		coordsInMemory[0] = newCoords.x;
		coordsInMemory[1] = newCoords.y;
		coordsInMemory[2] = newCoords.z;
		float* anglesInMemory = reinterpret_cast<float*>(g_cameraStructAddress + ANGLES_IN_STRUCT_OFFSET);

		anglesInMemory[0] = pitch;
		anglesInMemory[1] = yaw;
		anglesInMemory[2] = roll;

		_currentCameraCoords[0] = newCoords.x;
		_currentCameraCoords[1] = newCoords.y;
		_currentCameraCoords[2] = newCoords.z;
	}


	bool isCameraFound()
	{
		return nullptr != g_cameraStructAddress;
	}


	void displayCameraStructAddress()
	{
		OverlayConsole::instance().logDebug("Camera struct address: %p", (void*)g_cameraStructAddress);
	}


	void restoreGameCameraDataWithCachedData(GameCameraData& source)
	{
		if (!isCameraFound())
		{
			return;
		}
		source.RestoreData(reinterpret_cast<float*>(g_cameraStructAddress + COORDS_IN_STRUCT_OFFSET), 
						   reinterpret_cast<float*>(g_cameraStructAddress + ANGLES_IN_STRUCT_OFFSET),
						   g_fovStructAddress==nullptr ? nullptr : reinterpret_cast<float*>(g_fovStructAddress + FOV_IN_STRUCT_OFFSET));
		_currentCameraCoords[0] = source._coords[0];		// x
		_currentCameraCoords[1] = source._coords[1];		// y
		_currentCameraCoords[2] = source._coords[2];		// z
	}


	void cacheGameCameraDataInCache(GameCameraData& destination)
	{
		if (!isCameraFound())
		{
			return;
		}
		destination.CacheData(reinterpret_cast<float*>(g_cameraStructAddress + COORDS_IN_STRUCT_OFFSET),
							  reinterpret_cast<float*>(g_cameraStructAddress + ANGLES_IN_STRUCT_OFFSET),
							  g_fovStructAddress == nullptr ? nullptr : reinterpret_cast<float*>(g_fovStructAddress + FOV_IN_STRUCT_OFFSET));
		_currentCameraCoords[0] = destination._coords[0];		// x
		_currentCameraCoords[1] = destination._coords[1];		// y
		_currentCameraCoords[2] = destination._coords[2];		// z
	}


	void restoreOriginalValuesAfterCameraDisable()
	{
		restoreGameCameraDataWithCachedData(_originalData);
		// reset the fov to 0. This is specific for this game, as the fov here is the 'override' so if it's 0 it's not overriding the fov in the camera struct. 
		// when the camera is disabled, the fov will be reset to 0, which will mean it's going to use the fov of the camera struct which will make it work in cutscenes etc. again.
		if (nullptr != g_fovStructAddress)
		{
			float* fovInMemory = reinterpret_cast<float*>(g_fovStructAddress + FOV_IN_STRUCT_OFFSET);
			*fovInMemory = 0.0f;
		}
	}


	void cacheOriginalValuesBeforeCameraEnable()
	{
		cacheGameCameraDataInCache(_originalData);
	}


	void restoreOriginalValuesAfterMultiShot()
	{
		restoreGameCameraDataWithCachedData(_preMultiShotData);
	}


	void cacheOriginalValuesBeforeMultiShot()
	{
		cacheGameCameraDataInCache(_preMultiShotData);
	}
}