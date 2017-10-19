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
}

namespace IGCS::GameSpecific::CameraManipulator
{
	static double _originalCoords[3];			// coords are stored in doubles.
	static float _originalRotationMatrix[12];	// 3x3 matrix
	static float _originalFoV;
	static LPBYTE g_timestopValueAddress = nullptr;


	void setTimestopValueAddress(LPBYTE address)
	{
		g_timestopValueAddress = address;
	}

	
	// newValue: 1 == time should be frozen, 0 == normal gameplay
	// returns true if the game was stopped by this call, false if the game was either already stopped or the state didn't change.
	bool setTimeStopValue(byte newValue)
	{
		if (nullptr == g_timestopValueAddress)
		{
			return false;
		}
		bool toReturn = *g_timestopValueAddress == (byte)0 && (newValue == (byte)1);
		*g_timestopValueAddress = newValue;
		return toReturn;
	}


	// Resets the FOV to the one it got when we enabled the camera
	void resetFoV()
	{
		if (g_cameraStructAddress == nullptr)
		{
			return;
		}
		float* fovAddress = reinterpret_cast<float*>(g_cameraStructAddress + FOV_IN_STRUCT_OFFSET);
		*fovAddress = _originalFoV;
	}


	// changes the FoV with the specified amount
	void changeFoV(float amount)
	{
		if (g_cameraStructAddress == nullptr)
		{
			return;
		}
		float* fovAddress = reinterpret_cast<float*>(g_cameraStructAddress + FOV_IN_STRUCT_OFFSET);
		float newValue = *fovAddress + amount;
		if (newValue < 0.1f)
		{
			// clamp. Game will crash with negative fov
			newValue = 0.1;
		}
		*fovAddress = newValue;
	}
	

	XMFLOAT3 getCurrentCameraCoords()
	{
		// This particular game uses 2 camera buffers, to which we write the same data. So we pick the first as it doesn't matter. 
		double* coordsInMemory = reinterpret_cast<double*>(g_cameraStructAddress + COORDS_IN_STRUCT_OFFSET);
		// convert to float. This will lose precision, but the game itself converts doubles to floats almost right away... 
		XMFLOAT3 currentCoords = XMFLOAT3((float)coordsInMemory[0], (float)coordsInMemory[1], (float)coordsInMemory[2]);
		return currentCoords;
	}


	// newLookQuaternion: newly calculated quaternion of camera view space. Can be used to construct a 4x4 matrix if the game uses a matrix instead of a quaternion
	// newCoords are the new coordinates for the camera in worldspace.
	void writeNewCameraValuesToGameData(XMFLOAT3 newCoords, XMVECTOR newLookQuaternion)
	{
		if (nullptr == g_cameraStructAddress)
		{
			return;
		}

		// game uses a 3x3 matrix for look data. We have to calculate a rotation matrix from our quaternion and store the upper 3x3 matrix (_11-_33) in memory.
		XMMATRIX rotationMatrixPacked = XMMatrixRotationQuaternion(newLookQuaternion);
		XMFLOAT4X4 rotationMatrix;
		XMStoreFloat4x4(&rotationMatrix, rotationMatrixPacked);

		double* coordsInMemory = coordsInMemory = reinterpret_cast<double*>(g_cameraStructAddress + COORDS_IN_STRUCT_OFFSET);
		coordsInMemory[0] = (double)newCoords.x;
		coordsInMemory[1] = (double)newCoords.y;
		coordsInMemory[2] = (double)newCoords.z;

		// 3x3 rotation part of matrix
		float* matrixInMemory = reinterpret_cast<float*>(g_cameraStructAddress + ROTATION_MATRIX_IN_STRUCT_OFFSET);
		matrixInMemory[0] = rotationMatrix._11;
		matrixInMemory[1] = rotationMatrix._12;
		matrixInMemory[2] = rotationMatrix._13;
		matrixInMemory[3] = rotationMatrix._21;
		matrixInMemory[4] = rotationMatrix._22;
		matrixInMemory[5] = rotationMatrix._23;
		matrixInMemory[6] = rotationMatrix._31;
		matrixInMemory[7] = rotationMatrix._32;
		matrixInMemory[8] = rotationMatrix._33;
	}


	bool isCameraFound()
	{
		return nullptr != g_cameraStructAddress;
	}


	void displayCameraStructAddress()
	{
		OverlayConsole::instance().logDebug("Camera struct address: %p", (void*)g_cameraStructAddress);
	}
	

	// should restore the camera values in the camera structures to the cached values. This assures the free camera is always enabled at the original camera location.
	void restoreOriginalValuesAfterCameraDisable()
	{
		float* rotationMatrixInMemory = reinterpret_cast<float*>(g_cameraStructAddress + ROTATION_MATRIX_IN_STRUCT_OFFSET);
		double* coordsInMemory = reinterpret_cast<double*>(g_cameraStructAddress + COORDS_IN_STRUCT_OFFSET);
		memcpy(rotationMatrixInMemory, _originalRotationMatrix, 12 * sizeof(float));
		memcpy(coordsInMemory, _originalCoords, 3 * sizeof(double));
		float *fovInMemory = reinterpret_cast<float*>(g_cameraStructAddress + FOV_IN_STRUCT_OFFSET);
		*fovInMemory = _originalFoV;
	}


	void cacheOriginalValuesBeforeCameraEnable()
	{
		float* rotationMatrixInMemory = reinterpret_cast<float*>(g_cameraStructAddress + ROTATION_MATRIX_IN_STRUCT_OFFSET);
		double *coordsInMemory = reinterpret_cast<double*>(g_cameraStructAddress + COORDS_IN_STRUCT_OFFSET);
		memcpy(_originalRotationMatrix, rotationMatrixInMemory, 12 * sizeof(float));
		memcpy(_originalCoords, coordsInMemory, 3 * sizeof(double));
		float *fovInMemory = reinterpret_cast<float*>(g_cameraStructAddress + FOV_IN_STRUCT_OFFSET);
		_originalFoV = *fovInMemory;
	}
}