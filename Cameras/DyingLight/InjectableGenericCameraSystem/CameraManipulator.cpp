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
	LPBYTE g_lodStructAddress = nullptr;
	LPBYTE g_timestopStructAddress = nullptr;
}

namespace IGCS::GameSpecific::CameraManipulator
{
	static float _originalMatrix[12];
	static float _originalFoV;

	void setLoD(float newValue)
	{
		if (nullptr == g_lodStructAddress)
		{
			return;
		}
		float* lodInMemory = reinterpret_cast<float*>(g_lodStructAddress + LOD1_IN_STRUCT_OFFSET);
		*lodInMemory = newValue;
		lodInMemory = reinterpret_cast<float*>(g_lodStructAddress + LOD2_IN_STRUCT_OFFSET);
		*lodInMemory = newValue;
	}
	

	// newValue: 1 == time should be frozen, 0 == normal gameplay
	void setTimeStopValue(BYTE newValue)
	{
		if (nullptr == g_timestopStructAddress)
		{
			return;
		}
		LPBYTE timestopInMemory = (g_timestopStructAddress + TIMESTOP_IN_STRUCT_OFFSET);
		*timestopInMemory = newValue;
	}


	void getSettingsFromGameState()
	{
		Settings& currentSettings = Globals::instance().settings();
		//if (nullptr != g_todStructAddress)
		//{
		//	float* todInMemory = reinterpret_cast<float*>(g_todStructAddress);
		//	float valueInMemory = *todInMemory;
		//	currentSettings.todHour = (int)floor(valueInMemory);
		//	float valueWithoutMinuteFraction = valueInMemory - floor(valueInMemory);
		//	currentSettings.todMinute = (int)(valueWithoutMinuteFraction * 60.0f);
		//}
	}


	void applySettingsToGameState()
	{
		Settings& currentSettings = Globals::instance().settings();
		//if (nullptr != g_todStructAddress)
		//{
		//	float* todInMemory = reinterpret_cast<float*>(g_todStructAddress);
		//	float valueToStore = (static_cast<float>(currentSettings.todHour) + (static_cast<float>(currentSettings.todMinute) / 60.0f));
		//	valueToStore = Utils::clamp(valueToStore, 0.0f, 24.0f, 0.0f);
		//	*todInMemory = valueToStore;
		//}
	}


	// Resets the FOV to the one it got when we enabled the camera
	void resetFoV()
	{
		if (nullptr == g_cameraStructAddress)
		{
			return;
		}
		float* fovAddress = reinterpret_cast<float*>(g_cameraStructAddress + FOV_IN_STRUCT_OFFSET);
		*fovAddress = _originalFoV;
	}


	// changes the FoV with the specified amount
	void changeFoV(float amount)
	{
		if (nullptr == g_cameraStructAddress)
		{
			return;
		}
		float* fovAddress = reinterpret_cast<float*>(g_cameraStructAddress + FOV_IN_STRUCT_OFFSET);
		float newValue = *fovAddress + amount;
		if (newValue < 0.001f)
		{
			// clamp. 
			newValue = 0.001f;
		}
		*fovAddress = newValue;
	}
	

	XMFLOAT3 getCurrentCameraCoords()
	{

		float* matrixInMemory = reinterpret_cast<float*>(g_cameraStructAddress + MATRIX_IN_STRUCT_OFFSET);
		return XMFLOAT3(matrixInMemory[3], matrixInMemory[7], matrixInMemory[11]);
	}


	// newLookQuaternion: newly calculated quaternion of camera view space. Can be used to construct a 4x4 matrix if the game uses a matrix instead of a quaternion
	// newCoords are the new coordinates for the camera in worldspace.
	void writeNewCameraValuesToGameData(XMFLOAT3 newCoords, XMVECTOR newLookQuaternion)
	{
		if (!isCameraFound())
		{
			return;
		}

		// calculate matrix from quaternion, write that, then write the coords on top of that. 
		XMMATRIX rotationMatrixPacked = XMMatrixRotationQuaternion(newLookQuaternion);
		XMFLOAT4X4 rotationMatrix;
		XMStoreFloat4x4(&rotationMatrix, rotationMatrixPacked);

		float* matrixInMemory = reinterpret_cast<float*>(g_cameraStructAddress + MATRIX_IN_STRUCT_OFFSET);
		matrixInMemory[0] = rotationMatrix._11;
		matrixInMemory[1] = rotationMatrix._12;
		matrixInMemory[2] = rotationMatrix._13;
		matrixInMemory[3] = newCoords.x;
		matrixInMemory[4] = rotationMatrix._21;
		matrixInMemory[5] = rotationMatrix._22;
		matrixInMemory[6] = rotationMatrix._23;
		matrixInMemory[7] = newCoords.y;
		matrixInMemory[8] = rotationMatrix._31;
		matrixInMemory[9] = rotationMatrix._32;
		matrixInMemory[10] = rotationMatrix._33;
		matrixInMemory[11] = newCoords.z;
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
		float* matrixInMemory = nullptr;
		float *fovInMemory = nullptr;

		if (!isCameraFound())
		{
			return;
		}
		matrixInMemory = reinterpret_cast<float*>(g_cameraStructAddress + MATRIX_IN_STRUCT_OFFSET);
		memcpy(matrixInMemory, _originalMatrix, 12 * sizeof(float));
		fovInMemory = reinterpret_cast<float*>(g_cameraStructAddress + FOV_IN_STRUCT_OFFSET);
		*fovInMemory = _originalFoV;
	}


	void cacheOriginalValuesBeforeCameraEnable()
	{
		float* matrixInMemory = nullptr;
		float *fovInMemory = nullptr;

		if (!isCameraFound())
		{
			return;
		}
		matrixInMemory = reinterpret_cast<float*>(g_cameraStructAddress + MATRIX_IN_STRUCT_OFFSET);
		memcpy(_originalMatrix, matrixInMemory, 12 * sizeof(float));
		fovInMemory = reinterpret_cast<float*>(g_cameraStructAddress + FOV_IN_STRUCT_OFFSET);
		_originalFoV = *fovInMemory;
	}
}