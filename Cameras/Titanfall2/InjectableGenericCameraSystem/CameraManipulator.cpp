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
#include "EngineClient.h"

using namespace DirectX;
using namespace std;

extern "C" {
	LPBYTE g_cameraStructAddress = nullptr;
}

namespace IGCS::GameSpecific::CameraManipulator
{
	// typedef of signatures of function we'll call from our own code to pause/unpause the game properly. 
	typedef void(__stdcall *RunCommandFunction) (LPVOID pRootObject, LPVOID pCommandString);
	//// the function pointer of the function we'll call to pause/unpause the game. 
	//static RunCommandFunction _runCommandFunc = nullptr;
	//// the pointer to the root object we have to pass as 1st argument to the RunCommand function to pause/unpause the game. 
	//static __int64* _rootObjectAddress = nullptr;
	static EngineClient* _engineClient;
	static LPBYTE _fovStructAddress = nullptr;

	static float _originalCameraData[6];	// 3 floats (x, y, z) and 3 angles (degrees)
	static float _originalFoV;
	static bool _firstPersonMode = true;		// true is first person, false is third person

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


	void setEngineClientLocationAddress(LPBYTE pEngineClientLocationAddress)
	{
		//00007FFED44B5CEF | 48 8B 0D 4A 7C 88 00             | mov rcx,qword ptr ds:[7FFED4D3D940]                   | Arg1
		//00007FFED44B5CF6 | 48 8D 15 AB 43 58 00             | lea rdx,qword ptr ds:[7FFED4A3A0A8]                   | Arg2 = "setpause nomsg"
		//00007FFED44B5CFD | 48 8B 01                         | mov rax,qword ptr ds:[rcx]                            |
		//00007FFED44B5D00 | FF 90 D8 00 00 00                | call qword ptr ds:[rax+D8]                            | Call RunFunction. 
		_engineClient = EngineClient::GetInstance(pEngineClientLocationAddress);
		OverlayConsole::instance().logDebug("EngineClient: %p", (void*)_engineClient);
		if (nullptr != _engineClient)
		{
			// issue sv_cheats 1 so all commands work
			runCommand("sv_cheats 1");
		}
	}


	void toggleFirstThirdPerson()
	{
		_firstPersonMode = !_firstPersonMode;
		runCommand(_firstPersonMode ? "firstperson" : "thirdperson");
	}


	void pauseUnpauseGame(bool pauseGame)
	{
		runCommand(pauseGame ? "setpause nomsg" : "unpause nomsg");
	}


	void toggleHideModelInFirstPerson(bool hide)
	{
		runCommand(hide ? "r_drawviewmodel 0" : "r_drawviewmodel 1");
	}


	void runCommand(const char* command)
	{
		if (nullptr == _engineClient)
		{
			return;
		}
		if (nullptr == command)
		{
			return;
		}
		_engineClient->ClientCmd(command);
	}


	void getSettingsFromGameState()
	{
		Settings& currentSettings = Globals::instance().settings();
		// nothing for now.
	}


	void applySettingsToGameState()
	{
		Settings& currentSettings = Globals::instance().settings();
		
		// dof
		if (currentSettings.dofEnable)
		{
			runCommand("dof_overrideParams 1");
			runCommand("dof_enable 1");
		}
		else
		{
			runCommand("dof_overrideParams 0");
			// done
			return;
		}
		char buffer[1024];			// if it crashes, too bad.
		sprintf_s(buffer, 1024, "dof_variable_blur %d\0", currentSettings.dofBlurStrength);
		runCommand(buffer);
		sprintf_s(buffer, 1024, "dof_nearDepthStart %d\0", currentSettings.dofNearStart);
		runCommand(buffer);
		sprintf_s(buffer, 1024, "dof_nearDepthEnd %d\0", currentSettings.dofNearEnd);
		runCommand(buffer);
		sprintf_s(buffer, 1024, "dof_farDepthStart %d\0", currentSettings.dofFarStart);
		runCommand(buffer);
		sprintf_s(buffer, 1024, "dof_farDepthEnd %d\0", currentSettings.dofFarEnd);
		runCommand(buffer);
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