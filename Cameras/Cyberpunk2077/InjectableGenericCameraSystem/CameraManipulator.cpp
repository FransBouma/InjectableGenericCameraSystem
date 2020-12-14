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
#include "Globals.h"
#include "Camera.h"
#include "GameCameraData.h"
#include "GameImageHooker.h"
#include "MessageHandler.h"

using namespace DirectX;
using namespace std;

namespace IGCS::GameSpecific::CameraManipulator
{
	static GameCameraData _originalCameraData;
	static float _coordMultiplierFactor = 0.0f;


	bool isPhotomodeActivated()
	{
		if(nullptr==g_pmStructAddress)
		{
			return false;
		}
		return *(g_pmStructAddress + PM_ACTIVATED_BIT_IN_STRUCT_OFFSET) == (uint8_t)1;
	}


	int getFovOffsetInActiveCameraStruct()
	{
		return isPhotomodeActivated() ? FOV_IN_FREECAMSTRUCT_OFFSET : FOV_IN_PLAYCAMSTRUCT_OFFSET;
	}
	

	float convertPackedInt32ToFloat(int toConvert)
	{
		return static_cast<float>(toConvert) * _coordMultiplierFactor;
	}

	
	int convertFloatToPackedInt32(float toConvert)
	{
		if(_coordMultiplierFactor<=0.0f)
		{
			return static_cast<int>(toConvert);
		}
		return static_cast<int>(toConvert / _coordMultiplierFactor);
	}

	
	void displayDebugInfo()
	{
		MessageHandler::logDebug("Debug info");
		MessageHandler::logDebug("---------------------------------");
		MessageHandler::logDebug("PM struct address: %p", (void*)g_pmStructAddress);
		MessageHandler::logDebug("Active cam struct address: %p", (void*)g_activeCamStructAddress);
		MessageHandler::logDebug("Resolution struct address: %p", (void*)g_resolutionStructAddress);
		MessageHandler::logDebug("Time of Day struct address: %p", (void*)g_todStructAddress);
		MessageHandler::logDebug("Play Hud Widget Bucket address: %p", (void*)g_playHudWidgetAddress);
		MessageHandler::logDebug("Photomode Hud Widget Bucket address: %p", (void*)g_pmHudWidgetAddress);
		MessageHandler::logDebug("Current fov offset: %x", getFovOffsetInActiveCameraStruct());
		MessageHandler::logDebug("Camera enabled: %d", g_cameraEnabled);
		MessageHandler::logDebug("---------------------------------");
	}

	
	void setCoordMultiplierFactor(float factor)
	{
		_coordMultiplierFactor = factor;
	}

	
	void resizeViewPort(int newWidth, int newHeight)
	{
		if(nullptr==g_resolutionStructAddress)
		{
			return;
		}
		*reinterpret_cast<int*>(g_resolutionStructAddress + WIDTH_IN_STRUCT_OFFSET) = newWidth;
		*reinterpret_cast<int*>(g_resolutionStructAddress + HEIGHT_IN_STRUCT_OFFSET) = newHeight;
	}


	void updateCameraDataInGameData(Camera& camera)
	{
		if (!g_cameraEnabled)
		{
			return;
		}

		// calculate new camera values. We have two cameras, but they might not be available both, so we have to test before we do anything. 
		DirectX::XMVECTOR newLookQuaternion = camera.calculateLookQuaternion();
		DirectX::XMFLOAT3 currentCoords;
		DirectX::XMFLOAT3 newCoords;
		if (isCameraFound())
		{
			currentCoords = getCurrentCameraCoords();
			newCoords = camera.calculateNewCoords(currentCoords, newLookQuaternion);
			writeNewCameraValuesToGameData(newCoords, newLookQuaternion);
		}
	}


	void changeTimeOfDayUsingAmount(float amount)
	{
		if(nullptr==g_todStructAddress)
		{
			return;
		}
		// calculate current time of day, then apply the amount to that, then add the # of days again, so we stay within the same day.
		int* todAddress = reinterpret_cast<int*>(g_todStructAddress + TOD_IN_STRUCT_OFFSET);
		const int currentToDInSeconds = *todAddress;
		// strip off time in the current day. this will lose the time in the current day, which is fine, as we'll set those with the specified tod
		const int todWithoutDays = currentToDInSeconds % 86400;
		const int todInDays = currentToDInSeconds - todWithoutDays;
		int newTimeOfDay = Utils::clamp(todWithoutDays + (int)(amount * 3600.0f), 0, (24 * 3600), 0);
		*todAddress = (todInDays + newTimeOfDay);
	}

	
	void toggleHud(bool showHud)
	{
		uint8_t newValue = showHud ? (uint8_t)1 : (uint8_t)0;
		if(nullptr!=g_playHudWidgetAddress)
		{
			*(g_playHudWidgetAddress + HUD_TOGGLE_SWITCH_IN_BUCKETS_OFFSET) = newValue;
		}
		if(nullptr!=g_pmHudWidgetAddress && isPhotomodeActivated())
		{
			*(g_pmHudWidgetAddress + HUD_TOGGLE_SWITCH_IN_BUCKETS_OFFSET) = newValue;
		}
	}


	void applySettingsToGameState()
	{
		Settings& currentSettings = Globals::instance().settings();
		if (currentSettings.timeOfDayChanged && nullptr != g_todStructAddress)
		{
			int* todAddress = reinterpret_cast<int*>(g_todStructAddress + TOD_IN_STRUCT_OFFSET);
			const int currentToDInSeconds = *todAddress;
			// strip off time in the current day. this will lose the time in the current day, which is fine, as we'll set those with the specified tod
			const int todWithoutDays = currentToDInSeconds % 86400;	
			const int todInDays = currentToDInSeconds - todWithoutDays;
			*todAddress = (todInDays + (int)(currentSettings.timeOfDay * 3600.0f));
		}
		currentSettings.resetFlags();
	}


	// Resets the FOV to the one it got when we enabled the camera
	void resetFoV()
	{
		if (g_activeCamStructAddress == nullptr)
		{
			return;
		}
		float* fovAddress = reinterpret_cast<float*>(g_activeCamStructAddress + getFovOffsetInActiveCameraStruct());
		*fovAddress = _originalCameraData._fov;
	}


	// changes the FoV with the specified amount
	void changeFoV(float amount)
	{
		if (g_activeCamStructAddress == nullptr)
		{
			return;
		}
		float* fovAddress = reinterpret_cast<float*>(g_activeCamStructAddress + getFovOffsetInActiveCameraStruct());
		float newValue = *fovAddress + amount;
		if (newValue < 0.001f)
		{
			newValue = 0.001f;
		}
		*fovAddress = newValue;
	}


	float getCurrentFoV()
	{
		if (nullptr == g_activeCamStructAddress)
		{
			return DEFAULT_FOV_DEGREES;
		}
		float* fovAddress = reinterpret_cast<float*>(g_activeCamStructAddress + getFovOffsetInActiveCameraStruct());
		return *fovAddress;
	}
	

	XMFLOAT3 getCurrentCameraCoords()
	{
		int* coordsInMemory = reinterpret_cast<int*>(g_activeCamStructAddress + COORDS_IN_CAMSTRUCT_OFFSET);
		return XMFLOAT3(convertPackedInt32ToFloat(coordsInMemory[0]), convertPackedInt32ToFloat(coordsInMemory[1]), convertPackedInt32ToFloat(coordsInMemory[2]));
	}


	// newCoords are the new coordinates for the camera in worldspace. 
	void writeNewCameraValuesToGameData(XMFLOAT3 newCoords, XMVECTOR newLookQuaternion)
	{
		if (!isCameraFound())
		{
			return;
		}

		XMFLOAT4 qAsFloat4;
		XMStoreFloat4(&qAsFloat4, newLookQuaternion);

		int* coordsInMemory = reinterpret_cast<int*>(g_activeCamStructAddress + COORDS_IN_CAMSTRUCT_OFFSET);
		coordsInMemory[0] = convertFloatToPackedInt32(newCoords.x);
		coordsInMemory[1] = convertFloatToPackedInt32(newCoords.y);
		coordsInMemory[2] = convertFloatToPackedInt32(newCoords.z);

		float* quaternionInMemory = reinterpret_cast<float*>(g_activeCamStructAddress + QUATERNION_IN_CAMSTRUCT_OFFSET);
		quaternionInMemory[0] = qAsFloat4.x;
		quaternionInMemory[1] = qAsFloat4.y;
		quaternionInMemory[2] = qAsFloat4.z;
		quaternionInMemory[3] = qAsFloat4.w;
	}


	bool isCameraFound()
	{
		return nullptr != g_activeCamStructAddress;
	}


	void displayCameraStructAddress()
	{
		MessageHandler::logDebug("Camera struct address: %p", (void*)g_activeCamStructAddress);
	}


	void restoreGameCameraDataWithCachedData(GameCameraData& source)
	{
		if (!isCameraFound())
		{
			return;
		}
		source.RestoreData(reinterpret_cast<float*>(g_activeCamStructAddress + QUATERNION_IN_CAMSTRUCT_OFFSET), reinterpret_cast<int*>(g_activeCamStructAddress + COORDS_IN_CAMSTRUCT_OFFSET),
						   reinterpret_cast<float*>(g_activeCamStructAddress + getFovOffsetInActiveCameraStruct()));
	}


	void cacheGameCameraDataInCache(GameCameraData& destination)
	{
		if (!isCameraFound())
		{
			return;
		}
		destination.CacheData(reinterpret_cast<float*>(g_activeCamStructAddress + QUATERNION_IN_CAMSTRUCT_OFFSET), reinterpret_cast<int*>(g_activeCamStructAddress + COORDS_IN_CAMSTRUCT_OFFSET),
							  reinterpret_cast<float*>(g_activeCamStructAddress + getFovOffsetInActiveCameraStruct()));
	}


	void restoreOriginalValuesAfterCameraDisable()
	{
		restoreGameCameraDataWithCachedData(_originalCameraData);
	}


	void cacheOriginalValuesBeforeCameraEnable()
	{
		cacheGameCameraDataInCache(_originalCameraData);
	}
}