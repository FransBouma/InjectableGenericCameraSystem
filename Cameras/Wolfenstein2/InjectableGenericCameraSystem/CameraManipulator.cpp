////////////////////////////////////////////////////////////////////////////////////////////////////////
// Part of Injectable Generic Camera System
// Copyright(c) 2018, Frans Bouma
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
	// global variables which are accessed in Interceptor.asm and therefore need to be defined as 'extern "C"'
	LPBYTE g_cameraStructAddress = nullptr;
	LPBYTE g_fovStructAddress = nullptr;
	LPBYTE g_runFramesStructAddress = nullptr;
}

namespace IGCS::GameSpecific::CameraManipulator
{
	static float _originalRotationMatrixData[9];
	static float _originalCoordsData[3];
	static float _originalFoV;
	static bool _timeHasBeenStopped = false;
	static LPBYTE _showHudAddress = nullptr;
	static LPBYTE _stopTimeAddress = nullptr;
	

	void setShowHudAddress(LPBYTE address)
	{
		_showHudAddress = address;
	}


	void setStopTimeAddress(LPBYTE address)
	{
		_stopTimeAddress = address;
	}
	

	// newValue is the amount of frames to skip ahead when g_stopTime (via setStopTimeValue) is set to a value > 0
	void setRunFramesValue(BYTE newValue)
	{
		if (nullptr == g_runFramesStructAddress)
		{
			return;
		}
		*(g_runFramesStructAddress + RUNFRAMES_CVAR_IN_STRUCT_OFFSET) = newValue;
	}


	// newValue: 1 == time should be frozen, 0 == normal gameplay. There are 3 values possible for this variable (g_stopTime). 2 stops everything, also the player, 
	// I opted for 1, as it leaves a bit freedom for what to do with the player after pausing the game. 
	void setStopTimeValue(BYTE newValue)
	{
		if (nullptr == _stopTimeAddress)
		{
			return;
		}
		*_stopTimeAddress = newValue;
	}


	// 1== show hud, 0 is hide hud.
	void setShowHudValue(BYTE newValue)
	{
		if (nullptr == _showHudAddress)
		{
			return;
		}
		*_showHudAddress = newValue;
	}


	XMFLOAT3 getCurrentCameraCoords()
	{
		float* coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
		XMFLOAT3 currentCoords = XMFLOAT3(coordsInMemory);
		return currentCoords;
	}


	// newLookQuaternion: newly calculated quaternion of camera view space. Used here to construct a 4x4 matrix as the game uses a matrix instead of a quaternion
	// newCoords are the new coordinates for the camera in worldspace.
	void writeNewCameraValuesToGameData(XMVECTOR newLookQuaternion, XMFLOAT3 newCoords)
	{
		XMFLOAT4 qAsFloat4;
		XMStoreFloat4(&qAsFloat4, newLookQuaternion);

		// game uses a 3x3 matrix for look data. We have to calculate a rotation matrix from our quaternion and store the upper 3x3 matrix (_11-_33) in memory.
		XMMATRIX rotationMatrixPacked = XMMatrixRotationQuaternion(newLookQuaternion);
		XMFLOAT4X4 rotationMatrix;
		XMStoreFloat4x4(&rotationMatrix, rotationMatrixPacked);

		// 3x3 rotation part of matrix
		float* matrixInMemory = reinterpret_cast<float*>(g_cameraStructAddress + ROTATION_MATRIX_IN_CAMERA_STRUCT_OFFSET);
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
		cout << "Camera address: " << hex << (void*)g_cameraStructAddress << endl;
#endif
	}


	void resetFoV()
	{
		if (nullptr == g_fovStructAddress)
		{
			return;
		}
		float* fovInMemory = reinterpret_cast<float*>(g_fovStructAddress + FOV_IN_FOV_STRUCT_OFFSET);
		*fovInMemory = _originalFoV;
	}


	void changeFoV(float amount)
	{
		if (nullptr == g_fovStructAddress)
		{
			return;
		}
		float* fovInMemory = reinterpret_cast<float*>(g_fovStructAddress + FOV_IN_FOV_STRUCT_OFFSET);
		*fovInMemory += amount;
	}


	void restoreOriginalCameraValues()
	{
		if (nullptr == g_cameraStructAddress)
		{
			return;
		}
		float* matrixInMemory = reinterpret_cast<float*>(g_cameraStructAddress + ROTATION_MATRIX_IN_CAMERA_STRUCT_OFFSET);
		float* coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
		memcpy(matrixInMemory, _originalRotationMatrixData, 9 * sizeof(float));
		memcpy(coordsInMemory, _originalCoordsData, 3 * sizeof(float));
		if (nullptr != g_fovStructAddress)
		{
			float* floatInMemory = reinterpret_cast<float*>(g_fovStructAddress + FOV_IN_FOV_STRUCT_OFFSET);
			*floatInMemory = _originalFoV;
		}
	}


	void cacheOriginalCameraValues()
	{
		if (nullptr == g_cameraStructAddress)
		{
			return;
		}
		float* matrixInMemory = reinterpret_cast<float*>(g_cameraStructAddress + ROTATION_MATRIX_IN_CAMERA_STRUCT_OFFSET);
		float* coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
		memcpy(_originalRotationMatrixData, matrixInMemory, 9 * sizeof(float));
		memcpy(_originalCoordsData, coordsInMemory, 3 * sizeof(float));
		if (nullptr != g_fovStructAddress)
		{
			float* floatInMemory = reinterpret_cast<float*>(g_fovStructAddress + FOV_IN_FOV_STRUCT_OFFSET);
			_originalFoV = *floatInMemory;
		}
	}
}