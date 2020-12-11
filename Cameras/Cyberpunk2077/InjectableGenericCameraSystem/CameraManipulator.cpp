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

	void displayDebugInfo()
	{
		MessageHandler::logDebug("Debug info");
		MessageHandler::logDebug("---------------------------------");
		MessageHandler::logDebug("PM struct address: %p", (void*)g_pmStructAddress);
		MessageHandler::logDebug("Active cam struct address: %p", (void*)g_activeCamStructAddress);
		MessageHandler::logDebug("Resolution struct address: %p", (void*)g_resolutionStructAddress);
		MessageHandler::logDebug("Camera enabled: %d", g_cameraEnabled);
		MessageHandler::logDebug("---------------------------------");
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
	

	void applySettingsToGameState()
	{
		Settings& currentSettings = Globals::instance().settings();
		/*
		if (currentSettings.timeOfDayChanged && nullptr != g_todStructAddress)
		{
			*(reinterpret_cast<float*>(g_todStructAddress)) = currentSettings.timeOfDay;
		}
		if(currentSettings.resolutionScaleChanged && nullptr!=g_resolutionScaleAddress)
		{
			*(reinterpret_cast<float*>(g_resolutionScaleAddress + RESOLUTION_SCALE_IN_STRUCT_OFFSET)) = currentSettings.resolutionScale;
		}
		if(currentSettings.fogSettingsChanged && (nullptr != getWorldGraphicData()))
		{
			applyFogSettings(currentSettings);
		}
		if(currentSettings.invulnerableChanged && nullptr!=g_playerHealthStructAddress)
		{
			*(g_playerHealthStructAddress + INVULNERABLE_IN_STRUCT_OFFSET) = currentSettings.invulnerable ? (uint8_t)1 : (uint8_t)0;
		}
		*/
		currentSettings.resetFlags();
	}


	// Resets the FOV to the one it got when we enabled the camera
	void resetFoV()
	{
		if (g_activeCamStructAddress == nullptr)
		{
			return;
		}
		float* fovAddress = reinterpret_cast<float*>(g_activeCamStructAddress + FOV_IN_FREECAMSTRUCT_OFFSET);
		*fovAddress = _originalCameraData._fov;
	}


	// changes the FoV with the specified amount
	void changeFoV(float amount)
	{
		if (g_activeCamStructAddress == nullptr)
		{
			return;
		}
		float* fovAddress = reinterpret_cast<float*>(g_activeCamStructAddress + FOV_IN_FREECAMSTRUCT_OFFSET);
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
		float* fovAddress = reinterpret_cast<float*>(g_activeCamStructAddress + FOV_IN_FREECAMSTRUCT_OFFSET);
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
						   reinterpret_cast<float*>(g_activeCamStructAddress + FOV_IN_FREECAMSTRUCT_OFFSET));
	}


	void cacheGameCameraDataInCache(GameCameraData& destination)
	{
		if (!isCameraFound())
		{
			return;
		}
		destination.CacheData(reinterpret_cast<float*>(g_activeCamStructAddress + QUATERNION_IN_CAMSTRUCT_OFFSET), reinterpret_cast<int*>(g_activeCamStructAddress + COORDS_IN_CAMSTRUCT_OFFSET),
							  reinterpret_cast<float*>(g_activeCamStructAddress + FOV_IN_FREECAMSTRUCT_OFFSET));
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