////////////////////////////////////////////////////////////////////////////////////////////////////////
// Part of Injectable Generic Camera System
// Copyright(c) 2019, Frans Bouma
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
	LPBYTE g_resolutionScaleAddress = nullptr;
	LPBYTE g_todStructAddress = nullptr;
	LPBYTE g_timestopStructAddress = nullptr;
	LPBYTE g_fogStructAddress = nullptr;
}

namespace IGCS::GameSpecific::CameraManipulator
{
	static float _originalCoords[3];
	static float _originalQuaternion[4];
	static float _originalFoV;
	static LPBYTE g_resolutionScaleMenuValueAddress = nullptr;

	// typedef of signatures of two functions we'll call from our own code to pause/unpause the game properly. 
	typedef void(__stdcall *PauseGameFunction) (LPVOID pWorld);
	typedef void(__stdcall *UnpauseGameFunction) (LPVOID pWorld);
	// the function pointers we'll call to pause/unpause the game. They point to AOB scanned addresses found in the host image.
	static PauseGameFunction _pauseGameFunc = nullptr;
	static UnpauseGameFunction _unpauseGameFunc = nullptr;


	void setPauseUnpauseGameFunctionPointers(LPBYTE pauseFunctionAddress, LPBYTE unpauseFunctionAddress)
	{
		// pause function: 
		//- RAX: 1 (not relevant)
		//- RBX: some pointer, not relevant
		//- RCX: struct pointer into which to set values (pWorld).
		//ACOdyssey.exe+1062DDD0 - 53                    - push rbx
		//ACOdyssey.exe+1062DDD1 - 48 83 EC 20           - sub rsp,20 { 32 }
		//ACOdyssey.exe+1062DDD5 - 48 89 CB              - mov rbx,rcx
		//ACOdyssey.exe+1062DDD8 - 48 8D 0D 51C502F5     - lea rcx,[ACOdyssey.exe+565A330] { [FFFFFFFF] }
		//ACOdyssey.exe+1062DDDF - E8 DC2F17F0           - call ACOdyssey.exe+7A0DC0
		//ACOdyssey.exe+1062DDE4 - FF 83 6C180000        - inc [rbx+0000186C]
		//ACOdyssey.exe+1062DDEA - 83 BB 6C180000 01     - cmp dword ptr [rbx+0000186C],01 { 1 }
		//ACOdyssey.exe+1062DDF1 - 75 10                 - jne ACOdyssey.exe+1062DE03
		//ACOdyssey.exe+1062DDF3 - 48 8B 05 CEE5D1F4     - mov rax,[ACOdyssey.exe+534C3C8] { [024168D0] }
		//ACOdyssey.exe+1062DDFA - 8B 50 2C              - mov edx,[rax+2C]
		//ACOdyssey.exe+1062DDFD - 89 93 68180000        - mov [rbx+00001868],edx								<< This sets everything in motion to pause things (value >=0)
		//ACOdyssey.exe+1062DE03 - 48 8D 0D 26C502F5     - lea rcx,[ACOdyssey.exe+565A330] { [FFFFFFFF] }
		//ACOdyssey.exe+1062DE0A - 48 83 C4 20           - add rsp,20 { 32 }
		//ACOdyssey.exe+1062DE0E - 5B                    - pop rbx
		//ACOdyssey.exe+1062DE0F - E9 2CDA4BF3           - jmp ACOdyssey.exe+3AEB840
		//ACOdyssey.exe+1062DE14 - CC                    - int 3 

		// unpause function:
		//ABI: Same as pause game function.
		//ACOdyssey.exe+106A6E10 - 53                    - push rbx
		//ACOdyssey.exe+106A6E11 - 48 83 EC 20           - sub rsp,20 { 32 }
		//ACOdyssey.exe+106A6E15 - 48 89 CB              - mov rbx,rcx
		//ACOdyssey.exe+106A6E18 - 48 8D 0D 1135FBF4     - lea rcx,[ACOdyssey.exe+565A330] { [FFFFFFFF] }
		//ACOdyssey.exe+106A6E1F - E8 9C9F0FF0           - call ACOdyssey.exe+7A0DC0
		//ACOdyssey.exe+106A6E24 - 8B 83 6C180000        - mov eax,[rbx+0000186C]
		//ACOdyssey.exe+106A6E2A - 85 C0                 - test eax,eax
		//ACOdyssey.exe+106A6E2C - 74 5B                 - je ACOdyssey.exe+106A6E89
		//ACOdyssey.exe+106A6E2E - 83 E8 01              - sub eax,01 { 1 }
		//ACOdyssey.exe+106A6E31 - 89 83 6C180000        - mov [rbx+0000186C],eax
		//ACOdyssey.exe+106A6E37 - 75 68                 - jne ACOdyssey.exe+106A6EA1
		//ACOdyssey.exe+106A6E39 - 48 8B 05 8855CAF4     - mov rax,[ACOdyssey.exe+534C3C8] { [021B68D0] }
		//ACOdyssey.exe+106A6E40 - 8B 48 2C              - mov ecx,[rax+2C]
		//ACOdyssey.exe+106A6E43 - 3B 8B 68180000        - cmp ecx,[rbx+00001868]
		//ACOdyssey.exe+106A6E49 - 75 1B                 - jne ACOdyssey.exe+106A6E66
		//ACOdyssey.exe+106A6E4B - 48 8D 0D DE34FBF4     - lea rcx,[ACOdyssey.exe+565A330] { [FFFFFFFF] }
		//ACOdyssey.exe+106A6E52 - C7 83 68180000 FFFFFFFF - mov [rbx+00001868],FFFFFFFF { -1 }
		//ACOdyssey.exe+106A6E5C - 48 83 C4 20           - add rsp,20 { 32 }
		//ACOdyssey.exe+106A6E60 - 5B                    - pop rbx
		//ACOdyssey.exe+106A6E61 - E9 DA4944F3           - jmp ACOdyssey.exe+3AEB840
		//ACOdyssey.exe+106A6E66 - B8 02000000           - mov eax,00000002 { 2 }
		//ACOdyssey.exe+106A6E6B - 39 C1                 - cmp ecx,eax
		//ACOdyssey.exe+106A6E6D - 0F46 C8               - cmovbe ecx,eax
		//ACOdyssey.exe+106A6E70 - 29 C1                 - sub ecx,eax
		//ACOdyssey.exe+106A6E72 - 89 8B 68180000        - mov [rbx+00001868],ecx
		//ACOdyssey.exe+106A6E78 - 48 8D 0D B134FBF4     - lea rcx,[ACOdyssey.exe+565A330] { [FFFFFFFF] }
		//ACOdyssey.exe+106A6E7F - 48 83 C4 20           - add rsp,20 { 32 }
		//ACOdyssey.exe+106A6E83 - 5B                    - pop rbx
		//ACOdyssey.exe+106A6E84 - E9 B74944F3           - jmp ACOdyssey.exe+3AEB840
		//ACOdyssey.exe+106A6E89 - 48 8D 8B 50170000     - lea rcx,[rbx+00001750]
		//ACOdyssey.exe+106A6E90 - E8 1B0C36F0           - call ACOdyssey.exe+A07AB0
		//ACOdyssey.exe+106A6E95 - 48 8D 8B 00180000     - lea rcx,[rbx+00001800]
		//ACOdyssey.exe+106A6E9C - E8 0F0C36F0           - call ACOdyssey.exe+A07AB0
		//ACOdyssey.exe+106A6EA1 - 48 8D 0D 8834FBF4     - lea rcx,[ACOdyssey.exe+565A330] { [FFFFFFFF] }
		//ACOdyssey.exe+106A6EA8 - 48 83 C4 20           - add rsp,20 { 32 }
		//ACOdyssey.exe+106A6EAC - 5B                    - pop rbx
		//ACOdyssey.exe+106A6EAD - E9 8E4944F3           - jmp ACOdyssey.exe+3AEB840
		//ACOdyssey.exe+106A6EB2 - CC                    - int 3 
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

		//// First set byte ONE to 1, then byte TWO to 1 for pause
		//// For unpause, set byte THREE to 0, then byte TWO to FF. 
		//// Unpause function isn't determinable. They seem to pause the thread, and it has to be resurrected before things move again officially.
		//// we keep the threads going, we just block things from moving on. Not totally reliable, but better than nothing. See notes for
		//// pause/unpause functions. 
		//if (newValue)
		//{
		//	*(g_timestopStructAddress + TIMESTOP_BYTE_ONE_OFFSET) = newValue;
		//	// wait 200ms
		//	Sleep(200);
		//	*((DWORD*)(g_timestopStructAddress + TIMESTOP_BYTE_TWO_OFFSET)) = 0x13131313;		// just write a code that's not used for a module in the game so it's not reset.
		//}
		//else
		//{
		//	*(g_timestopStructAddress + TIMESTOP_BYTE_THREE_OFFSET) = newValue;
		//	// wait 200ms
		//	Sleep(200);
		//	*((DWORD*)(g_timestopStructAddress + TIMESTOP_BYTE_TWO_OFFSET)) = 0xFFFFFFFF;		// set it to -1 so the thread reading it knows things are reset and back on.
		//}
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
		if (nullptr != g_fogStructAddress)
		{
			float* fogStrengthInMemory = reinterpret_cast<float*>(g_fogStructAddress + FOG_STRENGTH_IN_STRUCT_OFFSET);
			currentSettings.fogStrength = *fogStrengthInMemory;
			float* fogStartCurveInMemory = reinterpret_cast<float*>(g_fogStructAddress + FOG_START_CURVE_IN_STRUCT_OFFSET);
			currentSettings.fogStartCurve = *fogStartCurveInMemory;
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
		if (nullptr != g_fogStructAddress)
		{
			float* fogStrengthInMemory = reinterpret_cast<float*>(g_fogStructAddress + FOG_STRENGTH_IN_STRUCT_OFFSET);
			*fogStrengthInMemory = currentSettings.fogStrength;
			float* fogStartCurveInMemory = reinterpret_cast<float*>(g_fogStructAddress + FOG_START_CURVE_IN_STRUCT_OFFSET);
			*fogStartCurveInMemory = currentSettings.fogStartCurve;
		}
		// refactored in its own function so we can apply this too when the camera is enabled. 
		killInGameDofIfNeeded();
	}


	void killInGameDofIfNeeded()
	{
		Settings& currentSettings = Globals::instance().settings();
		if (isCameraFound() && g_cameraEnabled && currentSettings.disableInGameDofWhenCameraIsEnabled)
		{
			BYTE* cameraStruct = (BYTE*)g_cameraStructAddress;
			*(cameraStruct + DOF_ENABLE1_IN_STRUCT_OFFSET) = (BYTE)0;
			*(cameraStruct + DOF_ENABLE1_IN_STRUCT_OFFSET) = (BYTE)0;
		}
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

		// only the gameplay camera. Photomode coords aren't updated.
		coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + COORDS_IN_STRUCT_OFFSET);
		coordsInMemory[0] = newCoords.x;
		coordsInMemory[1] = newCoords.y;
		coordsInMemory[2] = newCoords.z;

		quaternionInMemory = reinterpret_cast<float*>(g_cameraStructAddress + QUATERNION_IN_STRUCT_OFFSET);
		quaternionInMemory[0] = qAsFloat4.x;
		quaternionInMemory[1] = qAsFloat4.y;
		quaternionInMemory[2] = qAsFloat4.z;
		quaternionInMemory[3] = qAsFloat4.w;
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

		fovInMemory = reinterpret_cast<float*>(g_cameraStructAddress + FOV_IN_STRUCT_OFFSET);
		*fovInMemory = _originalFoV;
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

		fovInMemory = reinterpret_cast<float*>(g_cameraStructAddress + FOV_IN_STRUCT_OFFSET);
		_originalFoV = *fovInMemory;
	}
}