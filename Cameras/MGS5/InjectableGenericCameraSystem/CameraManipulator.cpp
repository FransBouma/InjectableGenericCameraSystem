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
}

namespace IGCS::GameSpecific::CameraManipulator
{
	static float _originalQuaternion[4];
	static float _originalQuaternionCutscene[4];
	static float _originalCoords[3];
	static float _originalCutsceneCoords[3];
	static float _originalFov;


	// newValue: 1 == time should be frozen, 0 == normal gameplay
	void setTimeStopValue(byte newValue)
	{
		if (nullptr == g_gamespeedStructAddress)
		{
			return;
		}
		float* gamespeedAddress = reinterpret_cast<float*>(g_gamespeedStructAddress + GAMESPEED_IN_STRUCT_OFFSET);
		*gamespeedAddress = newValue ? 0.00001f : 1.0f;
	}

	// Resets the FOV to the one it got when we enabled the camera
	void resetFoV()
	{
		if (nullptr == g_cameraStructAddress)
		{
			return;
		}
		float* fovInMemory = reinterpret_cast<float*>(g_cameraStructAddress + FOV_IN_STRUCT_OFFSET);
		*fovInMemory = _originalFov;
	}


	// changes the FoV with the specified amount
	void changeFoV(float amount)
	{
		if (nullptr == g_cameraStructAddress)
		{
			return;
		}
		float* fovInMemory = reinterpret_cast<float*>(g_cameraStructAddress + FOV_IN_STRUCT_OFFSET);
		*fovInMemory += amount;
	}


	XMFLOAT3 getCurrentCameraCoords()
	{
		float* coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + COORDS_IN_STRUCT_OFFSET);
		return XMFLOAT3(coordsInMemory[0], coordsInMemory[1], coordsInMemory[2]);
	}
	

	XMFLOAT3 getCurrentCutsceneCameraCoords()
	{
		float* coordsInMemory = reinterpret_cast<float*>(g_cameraCutsceneStructAddress + COORDS_IN_STRUCT_OFFSET);
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
	void restoreOriginalCameraValues()
	{
		float* coordsInMemory = nullptr;
		float* quaternionInMemory = nullptr;
		if (isCameraFound())
		{
			coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + COORDS_IN_STRUCT_OFFSET);
			memcpy(coordsInMemory, _originalCoords, 3 * sizeof(float));
			quaternionInMemory = reinterpret_cast<float*>(g_cameraStructAddress + QUATERNION_IN_STRUCT_OFFSET);
			memcpy(quaternionInMemory, _originalQuaternion, 4 * sizeof(float));
			float* fovInMemory = reinterpret_cast<float*>(g_cameraStructAddress + FOV_IN_STRUCT_OFFSET);
			*fovInMemory = _originalFov;
		}
		if (isCutsceneCameraFound())
		{
			coordsInMemory = reinterpret_cast<float*>(g_cameraCutsceneStructAddress + COORDS_CUTSCENE_IN_STRUCT_OFFSET);
			memcpy(coordsInMemory, _originalCutsceneCoords, 3 * sizeof(float));
			quaternionInMemory = reinterpret_cast<float*>(g_cameraCutsceneStructAddress + QUATERNION_CUTSCENE_IN_STRUCT_OFFSET);
			memcpy(quaternionInMemory, _originalQuaternion, 4 * sizeof(float));
		}
		// reset pointer of our cutscene camera, as it might be become deactivated and has to be set again in the interceptor
		// before we can use it.
		g_cameraCutsceneStructAddress = nullptr;
	}


	void cacheOriginalCameraValues()
	{
		float* coordsInMemory = nullptr;
		float* quaternionInMemory = nullptr;
		if (isCameraFound())
		{
			coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + COORDS_IN_STRUCT_OFFSET);
			memcpy(_originalCoords, coordsInMemory, 3 * sizeof(float));
			quaternionInMemory = reinterpret_cast<float*>(g_cameraStructAddress + QUATERNION_IN_STRUCT_OFFSET);
			memcpy(_originalQuaternion, quaternionInMemory, 4 * sizeof(float));
			float* fovInMemory = reinterpret_cast<float*>(g_cameraStructAddress + FOV_IN_STRUCT_OFFSET);
			_originalFov = *fovInMemory;

		}
		if (isCutsceneCameraFound())
		{
			coordsInMemory = reinterpret_cast<float*>(g_cameraCutsceneStructAddress + COORDS_CUTSCENE_IN_STRUCT_OFFSET);
			memcpy(_originalCutsceneCoords, coordsInMemory, 3 * sizeof(float));
			quaternionInMemory = reinterpret_cast<float*>(g_cameraCutsceneStructAddress + QUATERNION_CUTSCENE_IN_STRUCT_OFFSET);
			memcpy(_originalQuaternion, quaternionInMemory, 4 * sizeof(float));
		}
	}
}