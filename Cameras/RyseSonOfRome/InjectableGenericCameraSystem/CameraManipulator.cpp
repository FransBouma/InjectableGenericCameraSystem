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
#include "Console.h"
#include "InterceptorHelper.h"
#include "Globals.h"

using namespace DirectX;
using namespace std;

extern "C" {
	LPBYTE g_cameraStructAddress = nullptr;
	LPBYTE g_timestop1StructAddress = nullptr;
	LPBYTE g_timestop2StructAddress = nullptr;
}

namespace IGCS::GameSpecific::CameraManipulator
{
	static float _originalCoordsData[3];
	static float _originalLookData[4];
	static float _currentCameraCoords[3];
	static LPBYTE _supersamplingVarAddress = nullptr;
	static LPBYTE _hudToggleVarAddress = nullptr;


	void toggleHud(LPBYTE hostImageAddress)
	{
		byte currentValue = *(hostImageAddress + HUD_TOGGLE_VAR_IN_IMAGE_OFFSET);
		toggleHud(hostImageAddress, currentValue == 0 ? (byte)1 : (byte)0);
	}


	void toggleHud(LPBYTE hostImageAddress, byte newValue)
	{
		*(hostImageAddress+HUD_TOGGLE_VAR_IN_IMAGE_OFFSET)= newValue;
	}


	void setSupersamplingFactor(LPBYTE hostImageAddress, float newValue)
	{
		float* superSamplingVarAddress = reinterpret_cast<float*>(hostImageAddress + SUPERSAMPLING_VAR_IN_IMAGE_OFFSET);
		*superSamplingVarAddress = newValue;
	}


	// newValue: 1 == time should be frozen, 0 == normal gameplay
	void setTimeStopValue(byte newValue)
	{
		if (nullptr != g_timestop1StructAddress)
		{
			*(g_timestop1StructAddress + TIMESTOP1_IN_STRUCT_OFFSET) = newValue;
		}
		if (nullptr != g_timestop2StructAddress)
		{
			*(g_timestop2StructAddress + TIMESTOP2_IN_STRUCT_OFFSET) = newValue;
		}
	}

	// Resets the FOV to the default
	void resetFoV()
	{
		if (nullptr == g_cameraStructAddress)
		{
			return;
		}
		float* fovInMemory = reinterpret_cast<float*>(g_cameraStructAddress+ FOV_IN_STRUCT_OFFSET);
		*fovInMemory = DEFAULT_FOV_RADIANS;
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
		return XMFLOAT3(_currentCameraCoords[0], _currentCameraCoords[1], _currentCameraCoords[2]);
	}
	

	// newLookQuaternion: newly calculated quaternion of camera view space. Can be used to construct a 4x4 matrix if the game uses a matrix instead of a quaternion
	// newCoords are the new coordinates for the camera in worldspace.
	void writeNewCameraValuesToGameData(XMFLOAT3 newCoords, XMVECTOR newLookQuaternion)
	{
		if (nullptr == g_cameraStructAddress)
		{
			return;
		}

		XMFLOAT4 qAsFloat4;
		XMStoreFloat4(&qAsFloat4, newLookQuaternion);

		float* coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
		coordsInMemory[0] = newCoords.x;
		coordsInMemory[1] = newCoords.y;
		coordsInMemory[2] = newCoords.z;
		float* lookQInMemory = reinterpret_cast<float*>(g_cameraStructAddress + LOOK_DATA_IN_CAMERA_STRUCT_OFFSET);
		lookQInMemory[0] = qAsFloat4.x;
		lookQInMemory[1] = qAsFloat4.y;
		lookQInMemory[2] = qAsFloat4.z;
		lookQInMemory[3] = qAsFloat4.w;

		_currentCameraCoords[0] = newCoords.x;
		_currentCameraCoords[1] = newCoords.y;
		_currentCameraCoords[2] = newCoords.z;
	}


	// Waits for the interceptor to pick up the camera struct address. Should only return if address is found 
	void waitForCameraStructAddresses(LPBYTE hostImageAddress)
	{
		Console::WriteLine("Waiting for camera struct interception...");
		while (nullptr == g_cameraStructAddress)
		{
			Sleep(100);
		}
		Console::WriteLine("Camera found.");

#ifdef _DEBUG
		cout << "Camera struct address: " << hex << (void*)g_cameraStructAddress << endl;
#endif
	}


	// should restore the camera values in the camera structures to the cached values. This assures the free camera is always enabled at the original camera location.
	void restoreOriginalCameraValues()
	{
		float* coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress+CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
		memcpy(coordsInMemory, _originalCoordsData, 3 * sizeof(float));
		float* lookQInMemory = reinterpret_cast<float*>(g_cameraStructAddress+LOOK_DATA_IN_CAMERA_STRUCT_OFFSET);
		memcpy(lookQInMemory, _originalLookData, 4 * sizeof(float));
	}


	void cacheOriginalCameraValues()
	{
		float* coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
		memcpy(_originalCoordsData, coordsInMemory, 3 * sizeof(float));
		memcpy(_currentCameraCoords, coordsInMemory, 3 * sizeof(float));
		float* lookQInMemory = reinterpret_cast<float*>(g_cameraStructAddress + LOOK_DATA_IN_CAMERA_STRUCT_OFFSET);
		memcpy(_originalLookData, lookQInMemory, 4 * sizeof(float));
	}
}