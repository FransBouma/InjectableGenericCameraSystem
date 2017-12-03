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
	LPBYTE g_cameraPhotoModeStructAddress = nullptr;
	LPBYTE g_fovStructAddress = nullptr;
	LPBYTE g_resolutionScaleAddress = nullptr;
	LPBYTE g_todStructAddress = nullptr;
	LPBYTE g_timestopStructAddress = nullptr;
}

namespace IGCS::GameSpecific::CameraManipulator
{
	// typedef of signatures of two functions we'll call from our own code to pause/unpause the game properly. 
	typedef void(__stdcall *PauseGameFunction) (LPVOID pWorld);
	typedef void(__stdcall *UnpauseGameFunction) (LPVOID pWorld);
	// the function pointers we'll call to pause/unpause the game. They point to AOB scanned addresses found in the host image.
	static PauseGameFunction _pauseGameFunc = nullptr;
	static UnpauseGameFunction _unpauseGameFunc = nullptr;

	static float _originalCoords[3];
	static float _originalPMCoords[3];		// photomode cam
	static float _originalQuaternion[4];
	static float _originalPMQuaternion[4];  // photomode cam
	static float _originalFoV;
	static LPBYTE g_resolutionScaleMenuValueAddress = nullptr;


	void setResolutionScaleMenuValueAddress(LPBYTE address)
	{
		g_resolutionScaleMenuValueAddress = address;
	}


	void setPauseUnpauseGameFunctionPointers(LPBYTE pauseFunctionAddress, LPBYTE unpauseFunctionAddress)
	{
		// pause function: 
		//ABI:
		//- RCX: struct pointer into which to set values (pWorld). Same pointer we found as 'timestop struct'
		//ACOrigins.exe+1157A40 - 48 89 5C 24 08        - mov [rsp+08],rbx
		//ACOrigins.exe+1157A45 - 57                    - push rdi
		//ACOrigins.exe+1157A46 - 48 83 EC 20           - sub rsp,20 { 32 }
		//ACOrigins.exe+1157A4A - 48 8B F9              - mov rdi,rcx
		//ACOrigins.exe+1157A4D - 0F31                  - rdtsc 
		//ACOrigins.exe+1157A4F - 48 C1 E2 20           - shl rdx,20 { 32 }
		//ACOrigins.exe+1157A53 - 48 8D 0D 76C07203     - lea rcx,[ACOrigins.exe+4883AD0] { [FFFFFFFF] }
		//ACOrigins.exe+1157A5A - 48 0B C2              - or rax,rdx
		//ACOrigins.exe+1157A5D - 48 8B D8              - mov rbx,rax
		//ACOrigins.exe+1157A60 - E8 EB0A27FF           - call ACOrigins.exe+3C8550
		//ACOrigins.exe+1157A65 - 0F31                  - rdtsc 
		//ACOrigins.exe+1157A67 - 48 C1 E2 20           - shl rdx,20 { 32 }
		//ACOrigins.exe+1157A6B - 48 8D 0D 36477A02     - lea rcx,[ACOrigins.exe+38FC1A8] { [1431EF1F0] }
		//ACOrigins.exe+1157A72 - 48 0B C2              - or rax,rdx
		//ACOrigins.exe+1157A75 - 48 8B D3              - mov rdx,rbx
		//ACOrigins.exe+1157A78 - 4C 8B C0              - mov r8,rax
		//ACOrigins.exe+1157A7B - E8 603CFA00           - call ACOrigins.exe+20FB6E0
		//ACOrigins.exe+1157A80 - FF 87 3C150000        - inc [rdi+0000153C]
		//ACOrigins.exe+1157A86 - 83 BF 3C150000 01     - cmp dword ptr [rdi+0000153C],01 { 1 }
		//ACOrigins.exe+1157A8D - 75 10                 - jne ACOrigins.exe+1157A9F
		//ACOrigins.exe+1157A8F - 48 8B 05 E29A3403     - mov rax,[ACOrigins.exe+44A1578] { [01C368F0] }
		//ACOrigins.exe+1157A96 - 8B 50 2C              - mov edx,[rax+2C]
		//ACOrigins.exe+1157A99 - 89 97 38150000        - mov [rdi+00001538],edx
		//ACOrigins.exe+1157A9F - 48 8D 0D 2AC07203     - lea rcx,[ACOrigins.exe+4883AD0] { [FFFFFFFF] }
		//ACOrigins.exe+1157AA6 - 48 8B 5C 24 30        - mov rbx,[rsp+30]
		//ACOrigins.exe+1157AAB - 48 83 C4 20           - add rsp,20 { 32 }
		//ACOrigins.exe+1157AAF - 5F                    - pop rdi
		//ACOrigins.exe+1157AB0 - E9 EBF9E101           - jmp ACOrigins.exe+2F774A0		// Leave Critical Section, with ret.

		// unpause function:
		//ABI: Same as pause game function.
		//ACOrigins.exe+11629D0 - 48 89 5C 24 08        - mov [rsp+08],rbx
		//ACOrigins.exe+11629D5 - 57                    - push rdi
		//ACOrigins.exe+11629D6 - 48 83 EC 20           - sub rsp,20 { 32 }
		//ACOrigins.exe+11629DA - 48 8B F9              - mov rdi,rcx
		//ACOrigins.exe+11629DD - 0F31                  - rdtsc 
		//ACOrigins.exe+11629DF - 48 C1 E2 20           - shl rdx,20 { 32 }
		//ACOrigins.exe+11629E3 - 48 8D 0D E6107203     - lea rcx,[ACOrigins.exe+4883AD0] { [FFFFFFFF] }
		//ACOrigins.exe+11629EA - 48 0B C2              - or rax,rdx
		//ACOrigins.exe+11629ED - 48 8B D8              - mov rbx,rax
		//ACOrigins.exe+11629F0 - E8 5B5B26FF           - call ACOrigins.exe+3C8550
		//ACOrigins.exe+11629F5 - 0F31                  - rdtsc 
		//ACOrigins.exe+11629F7 - 48 C1 E2 20           - shl rdx,20 { 32 }
		//ACOrigins.exe+11629FB - 48 8D 0D A6977902     - lea rcx,[ACOrigins.exe+38FC1A8] { [1431EF1F0] }
		//ACOrigins.exe+1162A02 - 48 0B C2              - or rax,rdx
		//ACOrigins.exe+1162A05 - 48 8B D3              - mov rdx,rbx
		//ACOrigins.exe+1162A08 - 4C 8B C0              - mov r8,rax
		//ACOrigins.exe+1162A0B - E8 D08CF900           - call ACOrigins.exe+20FB6E0
		//ACOrigins.exe+1162A10 - 8B 87 3C150000        - mov eax,[rdi+0000153C]
		//ACOrigins.exe+1162A16 - 85 C0                 - test eax,eax
		//ACOrigins.exe+1162A18 - 74 3D                 - je ACOrigins.exe+1162A57
		//ACOrigins.exe+1162A1A - 83 E8 01              - sub eax,01 { 1 }
		//ACOrigins.exe+1162A1D - 89 87 3C150000        - mov [rdi+0000153C],eax
		//ACOrigins.exe+1162A23 - 75 4A                 - jne ACOrigins.exe+1162A6F
		//ACOrigins.exe+1162A25 - 48 8B 05 4CEB3303     - mov rax,[ACOrigins.exe+44A1578] { [01C368F0] }
		//ACOrigins.exe+1162A2C - 8B 48 2C              - mov ecx,[rax+2C]
		//ACOrigins.exe+1162A2F - 3B 8F 38150000        - cmp ecx,[rdi+00001538]
		_pauseGameFunc = (PauseGameFunction)(pauseFunctionAddress);
		_unpauseGameFunc = (UnpauseGameFunction)(unpauseFunctionAddress);
	}
	

	// newValue: 1 == time should be frozen, 0 == normal gameplay
	// returns true if the game was stopped by this call, false if the game was either already stopped or the state didn't change.
	bool setTimeStopValue(BYTE newValue)
	{
		if (nullptr == g_timestopStructAddress)
		{
			return false;
		}
		if (nullptr == _pauseGameFunc)
		{
			return false;
		}

		if (newValue)
		{
			_pauseGameFunc((LPVOID)g_timestopStructAddress);
		}
		else
		{
			_unpauseGameFunc((LPVOID)g_timestopStructAddress);
		}
		return newValue;
	}


	void getSettingsFromGameState()
	{
		Settings& currentSettings = Globals::instance().settings();
		if (nullptr != g_resolutionScaleAddress)
		{
			float* resolutionScaleInMemory = reinterpret_cast<float*>(g_resolutionScaleAddress + RESOLUTION_SCALE_IN_STRUCT_OFFSET);
			currentSettings.resolutionScale = *resolutionScaleInMemory;
		}
		if (nullptr != g_todStructAddress)
		{
			float* todInMemory = reinterpret_cast<float*>(g_todStructAddress);
			float valueInMemory = *todInMemory;
			currentSettings.todHour = (int)floor(valueInMemory);
			float valueWithoutMinuteFraction = valueInMemory - floor(valueInMemory);
			currentSettings.todMinute = (int)(valueWithoutMinuteFraction * 60.0f);
		}
	}


	void applySettingsToGameState()
	{
		Settings& currentSettings = Globals::instance().settings();
		if (nullptr != g_resolutionScaleAddress)
		{
			float* resolutionScaleInMemory = reinterpret_cast<float*>(g_resolutionScaleAddress + RESOLUTION_SCALE_IN_STRUCT_OFFSET);
			*resolutionScaleInMemory = Utils::clamp(currentSettings.resolutionScale, RESOLUTION_SCALE_MIN, RESOLUTION_SCALE_MAX, 1.0f);
		}
		if (nullptr != g_todStructAddress)
		{
			float* todInMemory = reinterpret_cast<float*>(g_todStructAddress);
			float valueToStore = (static_cast<float>(currentSettings.todHour) + (static_cast<float>(currentSettings.todMinute) / 60.0f));
			valueToStore = Utils::clamp(valueToStore, 0.0f, 24.0f, 0.0f);
			*todInMemory = valueToStore;
		}
	}


	// Resets the FOV to the one it got when we enabled the camera
	void resetFoV()
	{
		if (g_fovStructAddress == nullptr)
		{
			return;
		}
		float* fovAddress = reinterpret_cast<float*>(g_fovStructAddress + FOV_IN_STRUCT_OFFSET);
		*fovAddress = _originalFoV;
	}


	// changes the FoV with the specified amount
	void changeFoV(float amount)
	{
		if (g_fovStructAddress == nullptr)
		{
			return;
		}
		float* fovAddress = reinterpret_cast<float*>(g_fovStructAddress + FOV_IN_STRUCT_OFFSET);
		float newValue = *fovAddress + amount;
		if (newValue < 0.001f)
		{
			// clamp. Game will crash with negative fov
			newValue = 0.001f;
		}
		*fovAddress = newValue;
	}
	

	XMFLOAT3 getCurrentCameraCoords()
	{
		// we write to both cameras at once, so we just grab one of the coords, it always works. Photomode does inherit its coords from the 
		// gameplay / current cam anyway. 
		float* coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + COORDS_IN_STRUCT_OFFSET);
		XMFLOAT3 currentCoords = XMFLOAT3(coordsInMemory[0], coordsInMemory[1], coordsInMemory[2]);
		return currentCoords;
	}


	// newLookQuaternion: newly calculated quaternion of camera view space. Can be used to construct a 4x4 matrix if the game uses a matrix instead of a quaternion
	// newCoords are the new coordinates for the camera in worldspace.
	void writeNewCameraValuesToGameData(XMFLOAT3 newCoords, XMVECTOR newLookQuaternion)
	{
		if (!isCameraFound())
		{
			return;
		}

		XMFLOAT4 qAsFloat4;
		XMStoreFloat4(&qAsFloat4, newLookQuaternion);

		float* coordsInMemory = nullptr;
		float* quaternionInMemory = nullptr;

		// game uses 2 cameras, one for gameplay and one for photomode. We'll write to both, if found. We have to write the coords to more places, as the game does that too.
		coordsInMemory = coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + COORDS_IN_STRUCT_OFFSET);
		coordsInMemory[0] = newCoords.x;
		coordsInMemory[1] = newCoords.y;
		coordsInMemory[2] = newCoords.z;

		coordsInMemory = coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + COORDS2_IN_STRUCT_OFFSET);
		coordsInMemory[0] = newCoords.x;
		coordsInMemory[1] = newCoords.y;
		coordsInMemory[2] = newCoords.z;

		quaternionInMemory = reinterpret_cast<float*>(g_cameraStructAddress + QUATERNION_IN_STRUCT_OFFSET);
		quaternionInMemory[0] = qAsFloat4.x;
		quaternionInMemory[1] = qAsFloat4.y;
		quaternionInMemory[2] = qAsFloat4.z;
		quaternionInMemory[3] = qAsFloat4.w;

		if (isPhotomodeCameraFound())
		{
			coordsInMemory = coordsInMemory = reinterpret_cast<float*>(g_cameraPhotoModeStructAddress + COORDS_IN_PM_STRUCT_OFFSET);
			coordsInMemory[0] = newCoords.x;
			coordsInMemory[1] = newCoords.y;
			coordsInMemory[2] = newCoords.z;

			quaternionInMemory = reinterpret_cast<float*>(g_cameraPhotoModeStructAddress + QUATERNION_IN_PM_STRUCT_OFFSET);
			quaternionInMemory[0] = qAsFloat4.x;
			quaternionInMemory[1] = qAsFloat4.y;
			quaternionInMemory[2] = qAsFloat4.z;
			quaternionInMemory[3] = qAsFloat4.w;
		}
	}


	bool isCameraFound()
	{
		return nullptr != g_cameraStructAddress;
	}


	bool isPhotomodeCameraFound()
	{
		return nullptr != g_cameraPhotoModeStructAddress;
	}


	void displayCameraStructAddress()
	{
		OverlayConsole::instance().logDebug("Camera struct address: %p", (void*)g_cameraStructAddress);
	}
	

	// should restore the camera values in the camera structures to the cached values. This assures the free camera is always enabled at the original camera location.
	void restoreOriginalValuesAfterCameraDisable()
	{
		float* quaternionInMemory = nullptr;
		float* coordsInMemory = nullptr;
		float *fovInMemory = nullptr;

		if (!isCameraFound())
		{
			return;
		}
		// gameplay / cutscene cam
		quaternionInMemory = reinterpret_cast<float*>(g_cameraStructAddress + QUATERNION_IN_STRUCT_OFFSET);
		coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + COORDS_IN_STRUCT_OFFSET);
		memcpy(quaternionInMemory, _originalQuaternion, 4 * sizeof(float));
		memcpy(coordsInMemory, _originalCoords, 3 * sizeof(float));

		if (nullptr != g_fovStructAddress)
		{
			fovInMemory = reinterpret_cast<float*>(g_fovStructAddress + FOV_IN_STRUCT_OFFSET);
			*fovInMemory = _originalFoV;
		}
		if (isPhotomodeCameraFound())
		{
			// photomode cam
			quaternionInMemory = reinterpret_cast<float*>(g_cameraPhotoModeStructAddress + QUATERNION_IN_PM_STRUCT_OFFSET);
			coordsInMemory = reinterpret_cast<float*>(g_cameraPhotoModeStructAddress + COORDS_IN_PM_STRUCT_OFFSET);
			memcpy(quaternionInMemory, _originalPMQuaternion, 4 * sizeof(float));
			memcpy(coordsInMemory, _originalPMCoords, 3 * sizeof(float));
		}
		if (nullptr != g_resolutionScaleMenuValueAddress && nullptr!=g_resolutionScaleAddress)
		{
			float* resolutionScaleInMemory = reinterpret_cast<float*>(g_resolutionScaleAddress + RESOLUTION_SCALE_IN_STRUCT_OFFSET);
			float* resolutionScaleMenuInMemory = reinterpret_cast<float*>(g_resolutionScaleMenuValueAddress);
			*resolutionScaleInMemory = *resolutionScaleMenuInMemory;
		}
	}


	void cacheOriginalValuesBeforeCameraEnable()
	{
		float* quaternionInMemory = nullptr;
		float* coordsInMemory = nullptr;
		float *fovInMemory = nullptr;

		if (!isCameraFound())
		{
			return;
		}
		// gameplay/cutscene cam
		quaternionInMemory = reinterpret_cast<float*>(g_cameraStructAddress + QUATERNION_IN_STRUCT_OFFSET);
		coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + COORDS_IN_STRUCT_OFFSET);
		memcpy(_originalQuaternion, quaternionInMemory, 4 * sizeof(float));
		memcpy(_originalCoords, coordsInMemory, 3 * sizeof(float));

		if (nullptr != g_fovStructAddress)
		{
			fovInMemory = reinterpret_cast<float*>(g_fovStructAddress + FOV_IN_STRUCT_OFFSET);
			_originalFoV = *fovInMemory;
		}
		if (isPhotomodeCameraFound())
		{
			// photomode cam
			quaternionInMemory = reinterpret_cast<float*>(g_cameraPhotoModeStructAddress + QUATERNION_IN_PM_STRUCT_OFFSET);
			coordsInMemory = reinterpret_cast<float*>(g_cameraPhotoModeStructAddress + COORDS_IN_PM_STRUCT_OFFSET);
			memcpy(_originalPMQuaternion, quaternionInMemory, 4 * sizeof(float));
			memcpy(_originalPMCoords, coordsInMemory, 3 * sizeof(float));
		}
	}
}