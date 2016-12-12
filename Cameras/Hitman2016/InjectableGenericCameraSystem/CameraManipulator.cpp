////////////////////////////////////////////////////////////////////////////////////////////////////////
// Part of Injectable Generic Camera System
// Copyright(c) 2016, Frans Bouma
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

using namespace DirectX;

extern "C" {
	LPBYTE _cameraStructAddress = NULL;
	LPBYTE _cameraStructAddress2 = NULL;
	LPBYTE _gamespeedStructAddress = NULL;
}

static float _originalLookData[4];
static float _originalCoordsData[3];
static bool _timeHasBeenStopped = false;
static bool _alternativeTimeHasBeenStopped = false;

// newValue: 1 == time should be frozen, 0 == normal gameplay
void SetTimeStopValue(byte newValue)
{
	// set flag so camera works during menu driven timestop
	_timeHasBeenStopped = (newValue == 1);
	float* gamespeedAddress = reinterpret_cast<float*>(_gamespeedStructAddress + GAMESPEED_IN_STRUCT_OFFSET);
	*gamespeedAddress = _timeHasBeenStopped ? 0.000001f : 1.0f;
}

void SetAlternativeTimeStop(LPBYTE hostImageAddress, byte newValue)
{
	// Menu driven timestop. No FOV
	_alternativeTimeHasBeenStopped = (newValue == 1);
	LPBYTE toWrite = hostImageAddress + MENU_TIMESTOP_OFFSET_IN_IMAGE;
	*toWrite = newValue;
}


XMFLOAT3 GetCurrentCameraCoords()
{
	float* coordsInMemory = reinterpret_cast<float*>(_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
	XMFLOAT3 currentCoords = XMFLOAT3(coordsInMemory);
	return currentCoords;
}


// newLookQuaternion: newly calculated quaternion of camera view space. Can be used to construct a 4x4 matrix if the game uses a matrix instead of a quaternion
// newCoords are the new coordinates for the camera in worldspace.
void WriteNewCameraValuesToGameData(XMVECTOR newLookQuaternion, XMFLOAT3 newCoords)
{
	XMFLOAT4 qAsFloat4;
	XMStoreFloat4(&qAsFloat4, newLookQuaternion);

	// look Quaternion
	float* lookInMemory = reinterpret_cast<float*>(_cameraStructAddress + LOOK_QUATERNION_IN_CAMERA_STRUCT_OFFSET);
	lookInMemory[0] = qAsFloat4.x;
	lookInMemory[1] = qAsFloat4.y;
	lookInMemory[2] = qAsFloat4.z;
	lookInMemory[3] = qAsFloat4.w;

	if (_alternativeTimeHasBeenStopped)
	{
		//// second camera, used during timestop
		lookInMemory = reinterpret_cast<float*>(_cameraStructAddress2 + LOOK_QUATERNION_IN_CAMERA2_STRUCT_OFFSET);
		lookInMemory[0] = qAsFloat4.x;
		lookInMemory[1] = qAsFloat4.y;
		lookInMemory[2] = qAsFloat4.z;
		lookInMemory[3] = qAsFloat4.w;
	}

	// Coords
	float* coordsInMemory = reinterpret_cast<float*>(_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
	coordsInMemory[0] = newCoords.x;
	coordsInMemory[1] = newCoords.y;
	coordsInMemory[2] = newCoords.z;

	if (_alternativeTimeHasBeenStopped)
	{
		//// second camera, used during timestop
		coordsInMemory = reinterpret_cast<float*>(_cameraStructAddress2 + CAMERA_COORDS_IN_CAMERA2_STRUCT_OFFSET);
		coordsInMemory[0] = newCoords.x;
		coordsInMemory[1] = newCoords.y;
		coordsInMemory[2] = newCoords.z;
	}
}


// Waits for the interceptor to pick up the camera struct address. Should only return if address is found 
void WaitForCameraStructAddresses()
{
	while(0 == _cameraStructAddress)
	{
		Sleep(100);
	}
}


// Resets the FOV to the default
void ResetFoV()
{
	float* fovInMemory = reinterpret_cast<float*>(_cameraStructAddress + FOV_IN_CAMERA_STRUCT_OFFSET);
	*fovInMemory = DEFAULT_FOV_RADIANS;
}


// changes the FoV with the specified amount
void ChangeFoV(float amount)
{
	float* fovInMemory = reinterpret_cast<float*>(_cameraStructAddress + FOV_IN_CAMERA_STRUCT_OFFSET);
	*fovInMemory += amount;
}


// should restore the camera values in the camera structures to the cached values. This assures the free camera is always enabled at the original camera location.
void RestoreOriginalCameraValues()
{
	float* lookInMemory = reinterpret_cast<float*>(_cameraStructAddress + LOOK_QUATERNION_IN_CAMERA_STRUCT_OFFSET);
	float* coordsInMemory = reinterpret_cast<float*>(_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
	memcpy(lookInMemory, _originalLookData, 4 * sizeof(float));
	memcpy(coordsInMemory, _originalCoordsData, 3 * sizeof(float));
	
	lookInMemory = reinterpret_cast<float*>(_cameraStructAddress2 + LOOK_QUATERNION_IN_CAMERA2_STRUCT_OFFSET);
	coordsInMemory = reinterpret_cast<float*>(_cameraStructAddress2 + CAMERA_COORDS_IN_CAMERA2_STRUCT_OFFSET);
	memcpy(lookInMemory, _originalLookData, 4 * sizeof(float));
	memcpy(coordsInMemory, _originalCoordsData, 3 * sizeof(float));
}


void CacheOriginalCameraValues()
{
	float* lookInMemory = reinterpret_cast<float*>(_cameraStructAddress + LOOK_QUATERNION_IN_CAMERA_STRUCT_OFFSET);
	float* coordsInMemory = reinterpret_cast<float*>(_cameraStructAddress + CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET);
	memcpy(_originalLookData, lookInMemory, 4 * sizeof(float));
	memcpy(_originalCoordsData, coordsInMemory, 3 * sizeof(float));
}

