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
	LPBYTE g_gamespeedStructAddress = nullptr;
	LPBYTE g_fovStructAddress = nullptr;
}

namespace IGCS::GameSpecific::CameraManipulator
{
	static float _originalLookData[12];	// 3x3 matrix
	static float _originalCoordsData[3];
	static bool _timeHasBeenStopped = false;

	// newValue: 1 == time should be frozen, 0 == normal gameplay
	void setTimeStopValue(byte newValue)
	{
		// set flag so camera works during menu driven timestop
		_timeHasBeenStopped = (newValue == 1);
		float* gamespeedAddress = reinterpret_cast<float*>(g_gamespeedStructAddress + GAMESPEED_IN_STRUCT_OFFSET);
		*gamespeedAddress = _timeHasBeenStopped ? 0.1f : DEFAULT_GAME_SPEED;
	}


	// decreases / increases the gamespeed till 0.1f or 10f. 
	void modifyGameSpeed(bool decrease)
	{
		if (!_timeHasBeenStopped)
		{
			return;
		}
		float* gamespeedAddress = reinterpret_cast<float*>(g_gamespeedStructAddress + GAMESPEED_IN_STRUCT_OFFSET);
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
		// This particular game uses 2 camera buffers, to which we write the same data. 
		// game uses a 3x3 matrix for look data. We have to calculate a rotation matrix from our quaternion and store the upper 3x3 matrix (_11-_33) in memory.
		XMMATRIX rotationMatrixPacked = XMMatrixRotationQuaternion(newLookQuaternion);
		XMFLOAT4X4 rotationMatrix;
		XMStoreFloat4x4(&rotationMatrix, rotationMatrixPacked);

		// 3x3 rotation part of matrix
		float* matrixInMemory = reinterpret_cast<float*>(g_cameraStructAddress + LOOK_DATA_IN_CAMERA_STRUCT_OFFSET);
		matrixInMemory[0] = rotationMatrix._11;
		matrixInMemory[1] = rotationMatrix._12;
		matrixInMemory[2] = rotationMatrix._13;
		matrixInMemory[3] = rotationMatrix._21;
		matrixInMemory[4] = rotationMatrix._22;
		matrixInMemory[5] = rotationMatrix._23;
		matrixInMemory[6] = rotationMatrix._31;
		matrixInMemory[7] = rotationMatrix._32;
		matrixInMemory[8] = rotationMatrix._33;

		// Coords
		float* coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
		coordsInMemory[0] = newCoords.x;
		coordsInMemory[1] = newCoords.y;
		coordsInMemory[2] = newCoords.z;
	}


	// Waits for the interceptor to pick up the camera struct address. Should only return if address is found 
	void waitForCameraStructAddresses()
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


	void resetFoV()
	{
		float* fovInMemory = reinterpret_cast<float*>(g_fovStructAddress + FOV_IN_STRUCT_OFFSET);
		*fovInMemory = DEFAULT_FOV;
	}


	// changes the FoV with the specified amount
	void changeFoV(float amount)
	{
		float* fovInMemory = reinterpret_cast<float*>(g_fovStructAddress + FOV_IN_STRUCT_OFFSET);
		*fovInMemory += amount;
	}


	// should restore the camera values in the camera structures to the cached values. This assures the free camera is always enabled at the original camera location.
	void restoreOriginalCameraValues()
	{
		float* lookInMemory = reinterpret_cast<float*>(g_cameraStructAddress + LOOK_DATA_IN_CAMERA_STRUCT_OFFSET);
		float* coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
		memcpy(lookInMemory, _originalLookData, 12 * sizeof(float));
		memcpy(coordsInMemory, _originalCoordsData, 3 * sizeof(float));
	}


	void cacheOriginalCameraValues()
	{
		float* lookInMemory = reinterpret_cast<float*>(g_cameraStructAddress + LOOK_DATA_IN_CAMERA_STRUCT_OFFSET);
		float* coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
		memcpy(_originalLookData, lookInMemory, 12 * sizeof(float));
		memcpy(_originalCoordsData, coordsInMemory, 3 * sizeof(float));
	}
}