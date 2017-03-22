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

using namespace DirectX;
using namespace std;

extern "C" {
	LPBYTE g_cameraStructAddress = nullptr;
	LPBYTE g_gameSpeedStructAddress = nullptr;
}

namespace IGCS::GameSpecific::CameraManipulator
{
	static float _originalLookData[4];		// game uses a quaternion
	static float _originalCoordsData[3];
	static bool _gameSpeedHasBeenStopped = false;

	void setGamespeedFreezeValue(byte newValue)
	{
		if (nullptr == g_gameSpeedStructAddress)
		{
			return;
		}
		// set flag so camera works during menu driven timestop
		_gameSpeedHasBeenStopped = (newValue == 1);
		float* gamespeedAddress = reinterpret_cast<float*>(g_gameSpeedStructAddress);
		*gamespeedAddress = _gameSpeedHasBeenStopped ? DEFAULT_MIN_GAME_SPEED : DEFAULT_GAME_SPEED;
	}


	// decreases / increases the gamespeed till 0.0001f or 10f. 
	void modifyGameSpeed(bool decrease)
	{
		if (!_gameSpeedHasBeenStopped)
		{
			return;
		}
		float* gamespeedAddress = reinterpret_cast<float*>(g_gameSpeedStructAddress);
		float newValue = *gamespeedAddress;
		newValue += (decrease ? -0.1f : 0.1f);
		if (newValue < DEFAULT_MIN_GAME_SPEED)
		{
			newValue = DEFAULT_MIN_GAME_SPEED;
		}
		if (newValue > DEFAULT_MAX_GAME_SPEED)
		{
			newValue = DEFAULT_MAX_GAME_SPEED;
		}
		*gamespeedAddress = newValue;
		Console::WriteLine("Game speed is set to now: " + to_string(newValue));
	}


	XMFLOAT3 getCurrentCameraCoords()
	{
		// This particular game uses 2 camera buffers, to which we write the same data. So we pick the first as it doesn't matter. 
		float* coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
		XMFLOAT3 currentCoords = XMFLOAT3(coordsInMemory);
		return currentCoords;
	}
	

	// newLookQuaternion: newly calculated quaternion of camera view space. Can be used to construct a 4x4 matrix if the game uses a matrix instead of a quaternion
	// newCoords are the new coordinates for the camera in worldspace.
	void writeNewCameraValuesToGameData(XMVECTOR newLookQuaternion, XMFLOAT3 newCoords)
	{
		XMFLOAT4 qAsFloat4;
		XMStoreFloat4(&qAsFloat4, newLookQuaternion);

		// look Quaternion
		float* lookInMemory = reinterpret_cast<float*>(g_cameraStructAddress + LOOK_DATA_IN_CAMERA_STRUCT_OFFSET);
		lookInMemory[0] = qAsFloat4.x;
		lookInMemory[1] = qAsFloat4.y;
		lookInMemory[2] = qAsFloat4.z;
		lookInMemory[3] = qAsFloat4.w;

		// Coords
		float* coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
		coordsInMemory[0] = newCoords.x;
		coordsInMemory[1] = newCoords.y;
		coordsInMemory[2] = newCoords.z;
	}


	// Waits for the interceptor to pick up the camera struct address. Should only return if address is found 
	void waitForCameraStructAddresses(LPBYTE hostImageAddress)
	{
		Console::WriteLine("Waiting for camera struct interception...");
		g_cameraStructAddress = hostImageAddress + CAMERA_STRUCT_OFFSET_IN_IMAGE;
		Console::WriteLine("Camera found.");

#ifdef _DEBUG
		cout << "Camera struct address: " << hex << (void*)g_cameraStructAddress << endl;
#endif
	}

	// should restore the camera values in the camera structures to the cached values. This assures the free camera is always enabled at the original camera location.
	void restoreOriginalCameraValues()
	{
		float* lookInMemory = reinterpret_cast<float*>(g_cameraStructAddress + LOOK_DATA_IN_CAMERA_STRUCT_OFFSET);
		float* coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
		memcpy(lookInMemory, _originalLookData, 4 * sizeof(float));
		memcpy(coordsInMemory, _originalCoordsData, 3 * sizeof(float));
	}


	void cacheOriginalCameraValues()
	{
		float* lookInMemory = reinterpret_cast<float*>(g_cameraStructAddress + LOOK_DATA_IN_CAMERA_STRUCT_OFFSET);
		float* coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
		memcpy(_originalLookData, lookInMemory, 4 * sizeof(float));
		memcpy(_originalCoordsData, coordsInMemory, 3 * sizeof(float));
	}
}