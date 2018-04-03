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
	// typedef of signatures of function we'll call from our own code to pause/unpause the game properly. 
	typedef void(__stdcall *RunCommandFunction) (LPVOID pRootObject, LPVOID pCommandString);
	// the function pointer of the function we'll call to pause/unpause the game. 
	static RunCommandFunction _runCommandFunc = nullptr;
	// the pointer to the root object we have to pass as 1st argument to the RunCommand function to pause/unpause the game. 
	static __int64* _rootObjectAddress = nullptr;
	static LPBYTE _fovStructAddress = nullptr;

	static float _originalCameraData[6];	// 3 floats (x, y, z) and 3 angles (degrees)
	static float _originalFoV;

	void setFoVAddress(LPBYTE fovStructAddress)
	{
		//client.dll+2C5A26 - 75 1C                 - jne client.dll+2C5A44
		//client.dll+2C5A28 - 83 C8 01              - or eax,01 { 1 }
		//client.dll+2C5A2B - 48 8D 15 36896400     - lea rdx,[client.dll+90E368]
		//client.dll+2C5A32 - 48 8D 0D EFCAF200     - lea rcx,[client.dll+11F2528] 
		//client.dll+2C5A39 - 89 05 F9CAF200        - mov [client.dll+11F2538],eax
		//client.dll+2C5A3F - E8 7C1E4700           - call client.dll+7378C0
		//client.dll+2C5A44 - 48 8B 05 E5CAF200     - mov rax,[client.dll+11F2530]						<< this address. 
		//client.dll+2C5A4B - F3 0F10 58 58         - movss xmm3,[rax+58]								<<< READ FOV
		//client.dll+2C5A50 - F3 0F5F 1D 20277400   - maxss xmm3,[client.dll+A08178] { [1.00] }			<< CLAMP MIN
		//client.dll+2C5A58 - 0F28 C3               - movaps xmm0,xmm3
		//client.dll+2C5A5B - F3 0F5D 05 D9906400   - minss xmm0,[client.dll+90EB3C] { [1.70] }			<< CLAMP MAX
		//client.dll+2C5A63 - 48 83 C4 28           - add rsp,28 { 40 }
		//client.dll+2C5A67 - C3                    - ret 
		_fovStructAddress = (LPBYTE)*(__int64*)fovStructAddress;
		OverlayConsole::instance().logDebug("_fovStructAddress: %p", (void*)_fovStructAddress);
	}


	void setPauseUnpauseGameFunctionPointers(LPBYTE pRootObjectAddress)
	{
		// pause/unpause uses a runcommand function which parses the input we pass to it. We need to pass a struct pointer (which also contains the runcommand function pointer) 
		// and a pointer to the command, be it either "setpause nomsg\0" or "unpause nomsg\0". 
		// Pause:
		//00007FFED44B5CDB | 48 8B 0D 5E 7C 88 00             | mov rcx,qword ptr ds:[7FFED4D3D940]                   | Arg1
		//00007FFED44B5CE2 | 48 8B 01                         | mov rax,qword ptr ds:[rcx]                            |
		//00007FFED44B5CE5 | FF 90 90 06 00 00                | call qword ptr ds:[rax+690]                           | sub_[rax+690]
		//00007FFED44B5CEB | 84 C0                            | test al,al                                            |
		//00007FFED44B5CED | 74 17                            | je client.7FFED44B5D06                                |
		//00007FFED44B5CEF | 48 8B 0D 4A 7C 88 00             | mov rcx,qword ptr ds:[7FFED4D3D940]                   | Arg1
		//00007FFED44B5CF6 | 48 8D 15 AB 43 58 00             | lea rdx,qword ptr ds:[7FFED4A3A0A8]                   | Arg2 = "setpause nomsg"
		//00007FFED44B5CFD | 48 8B 01                         | mov rax,qword ptr ds:[rcx]                            |
		//00007FFED44B5D00 | FF 90 D8 00 00 00                | call qword ptr ds:[rax+D8]                            | Call RunFunction. 
		//00007FFED44B5D06 | 48 63 0D 6B 21 03 02             | movsxd rcx,dword ptr ds:[7FFED64E7E78]                |
		//00007FFED44B5D0D | 48 8B D9                         | mov rbx,rcx                                           |
		//00007FFED44B5D10 | 8D 41 01                         | lea eax,qword ptr ds:[rcx+1]                          |
		//00007FFED44B5D13 | 48 63 D0                         | movsxd rdx,eax                                        |
		//00007FFED44B5D16 | 48 2B 15 4B 21 03 02             | sub rdx,qword ptr ds:[7FFED64E7E68]                   |
		//00007FFED44B5D1D | 48 85 D2                         | test rdx,rdx                                          |
		//00007FFED44B5D20 | 7E 12                            | jle client.7FFED44B5D34                               |
		//
		// Unpause:
		//00007FFED44B5511 | 48 8B 0D 28 84 88 00             | mov rcx,qword ptr ds:[7FFED4D3D940]                   | Arg1
		//00007FFED44B5518 | 48 8B 01                         | mov rax,qword ptr ds:[rcx]                            |
		//00007FFED44B551B | FF 90 90 06 00 00                | call qword ptr ds:[rax+690]                           | sub_[rax+690]
		//00007FFED44B5521 | 84 C0                            | test al,al                                            |
		//00007FFED44B5523 | 74 17                            | je client.7FFED44B553C                                |
		//00007FFED44B5525 | 48 8B 0D 14 84 88 00             | mov rcx,qword ptr ds:[7FFED4D3D940]                   | Arg1
		//00007FFED44B552C | 48 8D 15 85 4B 58 00             | lea rdx,qword ptr ds:[7FFED4A3A0B8]                   | Arg2 = "unpause nomsg"
		//00007FFED44B5533 | 48 8B 01                         | mov rax,qword ptr ds:[rcx]                            |
		//00007FFED44B5536 | FF 90 D8 00 00 00                | call qword ptr ds:[rax+D8]                            | Call RunFunction. 
		//00007FFED44B553C | C6 05 51 15 03 02 00             | mov byte ptr ds:[7FFED64E6A94],0                      |
		//00007FFED44B5543 | EB 07                            | jmp client.7FFED44B554C                               |
		//00007FFED44B5545 | 40 88 35 48 15 03 02             | mov byte ptr ds:[7FFED64E6A94],sil                    |
		//00007FFED44B554C | 48 8B 5C 24 30                   | mov rbx,qword ptr ss:[rsp+30]                         |
		//00007FFED44B5551 | 48 8B 74 24 38                   | mov rsi,qword ptr ss:[rsp+38]                         |
		//00007FFED44B5556 | 48 8B 7C 24 40                   | mov rdi,qword ptr ss:[rsp+40]                         |
		//00007FFED44B555B | 48 83 C4 20                      | add rsp,20                                            |
		// the pointer to pass as Arg1 is pointed to by the rootObjectAddress passed to this function.
		__int64* pRootObjectAsP64 = (__int64*)(pRootObjectAddress);
		_rootObjectAddress = (__int64*)*pRootObjectAsP64;
		__int64* pRunCommandLocation = (__int64*)((*_rootObjectAddress)+ 0xD8);
		_runCommandFunc = (RunCommandFunction)(*pRunCommandLocation);
		OverlayConsole::instance().logDebug("runCommandFunc: %p", (void*)_runCommandFunc);
	}
	

	// newValue: 1 == time should be frozen, 0 == normal gameplay
	// returns true if the game was stopped by this call, false if the game was either already stopped or the state didn't change.
	bool setTimeStopValue(BYTE newValue)
	{
		if (nullptr == _rootObjectAddress)
		{
			return false;
		}
		if (nullptr == _runCommandFunc)
		{
			return false;
		}
		char* setPauseCommand = "setpause nomsg\0";
		char* unpauseCommand = "unpause nomsg\0";

		_runCommandFunc((LPVOID)_rootObjectAddress, newValue ? (LPVOID)setPauseCommand : (LPVOID)unpauseCommand);
		return newValue;
	}


	void getSettingsFromGameState()
	{
		Settings& currentSettings = Globals::instance().settings();
		// nothing for now.
	}


	void applySettingsToGameState()
	{
		Settings& currentSettings = Globals::instance().settings();
		// nothing for now.
	}


	// Resets the FOV to the one it got when we enabled the camera
	void resetFoV()
	{
		if (_fovStructAddress == nullptr)
		{
			return;
		}
		float* fovAddress = reinterpret_cast<float*>(_fovStructAddress + FOV_IN_STRUCT_OFFSET);
		*fovAddress = _originalFoV;
	}


	// changes the FoV with the specified amount
	void changeFoV(float amount)
	{
		if (_fovStructAddress == nullptr)
		{
			return;
		}
		float* fovAddress = reinterpret_cast<float*>(_fovStructAddress + FOV_IN_STRUCT_OFFSET);
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


	// angle is an angle in degrees (0-360 range) and we have to clamp it for the game range (which is 0-180 and all above is -180-0
	float clampAngleForGameRange(float angle)
	{
		if (angle < 180.0f)
		{
			return angle;
		}
		return angle - 360.0f;
	}


	// newCoords are the new coordinates for the camera in worldspace. pitch / yaw / roll are the 3 angles the game uses for rotation around x, z and y resp. 
	// the angles given are in radians, so we have to recalculate them to degrees as the game uses the angles in degrees, in a range -180,0 - 180,0. If 
	// the angle reaches 180, it flips to -180, where 0 is normal angle (so the camera faces forward)
	void writeNewCameraValuesToGameData(XMFLOAT3 newCoords, float pitch, float yaw, float roll)
	{
		if (!isCameraFound())
		{
			return;
		}

		float* coordsInMemory = nullptr;

		coordsInMemory = reinterpret_cast<float*>(g_cameraStructAddress + COORDS_IN_STRUCT_OFFSET);
		coordsInMemory[0] = newCoords.x;
		coordsInMemory[1] = newCoords.y;
		coordsInMemory[2] = newCoords.z;

		// angles are stored right after the coords. Order is: pitch / yaw / roll. Our angle ranges are 0-2PI so 0-360 degrees. Our 0 is equal to the game's 0 and
		// although the game uses -180, 180 as range, it can deal with 0-360 so we don't do clamping here. 
		coordsInMemory[3] = Utils::convertRadiansToDegrees(pitch);
		coordsInMemory[4] = Utils::convertRadiansToDegrees(yaw);
		coordsInMemory[5] = Utils::convertRadiansToDegrees(roll);
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
		float* cameraDataInMemory = nullptr;
		float *fovInMemory = nullptr;

		if (!isCameraFound())
		{
			return;
		}
		cameraDataInMemory = reinterpret_cast<float*>(g_cameraStructAddress + COORDS_IN_STRUCT_OFFSET);
		memcpy(cameraDataInMemory, _originalCameraData, 6 * sizeof(float));

		if (nullptr != _fovStructAddress)
		{
			fovInMemory = reinterpret_cast<float*>(_fovStructAddress + FOV_IN_STRUCT_OFFSET);
			*fovInMemory = _originalFoV;
		}
	}


	void cacheOriginalValuesBeforeCameraEnable()
	{
		float* cameraDataInMemory = nullptr;
		float *fovInMemory = nullptr;

		if (!isCameraFound())
		{
			return;
		}
		cameraDataInMemory = reinterpret_cast<float*>(g_cameraStructAddress + COORDS_IN_STRUCT_OFFSET);
		memcpy(_originalCameraData, cameraDataInMemory, 6 * sizeof(float));

		if (nullptr != _fovStructAddress)
		{
			fovInMemory = reinterpret_cast<float*>(_fovStructAddress + FOV_IN_STRUCT_OFFSET);
			_originalFoV = *fovInMemory;
		}
	}
}