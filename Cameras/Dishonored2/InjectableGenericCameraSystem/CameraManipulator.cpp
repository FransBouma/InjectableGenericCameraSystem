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
#include "Globals.h"

using namespace DirectX;
using namespace std;


namespace IGCS::GameSpecific::CameraManipulator
{
	static LPBYTE _cameraStructAddress = nullptr;
	static float _originalLookData[4];
	static float _originalCoordsData[3];
	static float _originalFoV;

	void setCameraStructAddress(LPBYTE structAddress)
	{
		_cameraStructAddress = structAddress;
	}

	XMFLOAT3 getCurrentCameraCoords()
	{
		float* coordsInMemory = reinterpret_cast<float*>(_cameraStructAddress + CAMERA_COORDS_IN_STRUCT_OFFSET);
		XMFLOAT3 currentCoords = XMFLOAT3(coordsInMemory);
		return currentCoords;
	}


	XMVECTOR getCurrentQuaternion()
	{
		float* quaternionInMemory = reinterpret_cast<float*>(_cameraStructAddress + CAMERA_QUATERNION_IN_STRUCT_OFFSET);
		return XMVectorSet(quaternionInMemory[0], quaternionInMemory[1], quaternionInMemory[2], quaternionInMemory[3]);
	}


	// newLookQuaternion: newly calculated quaternion of camera view space. Can be used to construct a 4x4 matrix if the game uses a matrix instead of a quaternion
	// newCoords are the new coordinates for the camera in worldspace.
	void writeNewCameraValuesToGameData(XMVECTOR newLookQuaternion, XMFLOAT3 newCoords)
	{
		XMFLOAT4 qAsFloat4;
		XMStoreFloat4(&qAsFloat4, newLookQuaternion);

		// quaternion
		float* lookInMemory = reinterpret_cast<float*>(_cameraStructAddress + CAMERA_QUATERNION_IN_STRUCT_OFFSET);
		lookInMemory[0] = qAsFloat4.x;
		lookInMemory[1] = qAsFloat4.y;
		lookInMemory[2] = qAsFloat4.z;
		lookInMemory[3] = qAsFloat4.w;

		// Coords
		float* coordsInMemory = reinterpret_cast<float*>(_cameraStructAddress + CAMERA_COORDS_IN_STRUCT_OFFSET);
		coordsInMemory[0] = newCoords.x;
		coordsInMemory[1] = newCoords.y;
		coordsInMemory[2] = newCoords.z;
	}


	// changes the FoV with the specified amount
	void changeFoV(float amount)
	{
		if (nullptr == _cameraStructAddress)
		{
			return;
		}
		float* fovAddress = reinterpret_cast<float*>(_cameraStructAddress + FOV_IN_STRUCT_OFFSET);
		*fovAddress += amount;
	}


	void resetFOV()
	{
		if (nullptr == _cameraStructAddress)
		{
			return;
		}
		float* fovAddress = reinterpret_cast<float*>(_cameraStructAddress + FOV_IN_STRUCT_OFFSET);
		*fovAddress = DEFAULT_FOV_IN_DEGREES;	// original fov always contains 0. 
	}


	// should restore the camera values in the camera structures to the cached values. This assures the free camera is always enabled at the original camera location.
	void restoreOriginalCameraValues()
	{
		if (nullptr == _cameraStructAddress)
		{
			return;
		}
		float* lookInMemory = reinterpret_cast<float*>(_cameraStructAddress + CAMERA_QUATERNION_IN_STRUCT_OFFSET);
		float* coordsInMemory = reinterpret_cast<float*>(_cameraStructAddress + CAMERA_COORDS_IN_STRUCT_OFFSET);
		memcpy(lookInMemory, _originalLookData, 4 * sizeof(float));
		memcpy(coordsInMemory, _originalCoordsData, 3 * sizeof(float));
		float *fovInMemory = reinterpret_cast<float*>(_cameraStructAddress + FOV_IN_STRUCT_OFFSET);
		*fovInMemory = _originalFoV;
	}


	void cacheOriginalCameraValues()
	{
		float* lookInMemory = reinterpret_cast<float*>(_cameraStructAddress + CAMERA_QUATERNION_IN_STRUCT_OFFSET);
		float* coordsInMemory = reinterpret_cast<float*>(_cameraStructAddress + CAMERA_COORDS_IN_STRUCT_OFFSET);
		memcpy(_originalLookData, lookInMemory, 4 * sizeof(float));
		memcpy(_originalCoordsData, coordsInMemory, 3 * sizeof(float));
		float *fovInMemory = reinterpret_cast<float*>(_cameraStructAddress + FOV_IN_STRUCT_OFFSET);
		_originalFoV = *fovInMemory;
	}
}