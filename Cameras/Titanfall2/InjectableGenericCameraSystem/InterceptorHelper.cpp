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
#include "InterceptorHelper.h"
#include "GameConstants.h"
#include "GameImageHooker.h"
#include <map>
#include "OverlayConsole.h"
#include "CameraManipulator.h"

using namespace std;

//--------------------------------------------------------------------------------------------------------------------------------
// external asm functions
extern "C" {
	void cameraStructInterceptor();
	void cameraWrite1Interceptor();
	void cameraWrite2Interceptor();
	void cameraWrite3Interceptor();
	void cameraWrite4Interceptor();
	void cameraWrite5Interceptor();
}

// external addresses used in asm.
extern "C" {
	LPBYTE _cameraStructInterceptionContinue = nullptr;
	LPBYTE _cameraWrite1InterceptionContinue = nullptr;
	LPBYTE _cameraWrite2InterceptionContinue = nullptr;
	LPBYTE _cameraWrite3InterceptionContinue = nullptr;
	LPBYTE _cameraWrite4InterceptionContinue = nullptr;
	LPBYTE _cameraWrite5InterceptionContinue = nullptr;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*> &aobBlocks)
	{
		aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_ADDRESS_INTERCEPT_KEY, "89 0E 8B 48 04 89 4E 04 8B 40 08 48 8B CB 89 46 08 48 8B 03 FF 90 ?? ?? ?? ?? 8B 08 89 0F 8B 48 04 89 4F 04 8B 40 08 89 47 08 E8 ?? ?? ?? ?? 33 D2", 1);	
		aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE1_INTERCEPT_KEY, "F3 0F 11 07 F3 0F 10 44 24 4C F3 0F 11 4F 04 F3 0F 11 47 08 4C 8D 9C 24", 1);
		aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE2_INTERCEPT_KEY, "F3 0F 11 07 F3 0F 10 44 24 6C F3 0F 11 4F 04 F3 0F 11 47 08 4C 8D 9C 24", 1);
		aobBlocks[CAMERA_WRITE3_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE3_INTERCEPT_KEY, "8B 08 89 0F 8B 48 04 89 4F 04 8B 40 08 89 47 08 E8 ?? ?? ?? ?? 33 D2", 1);
		aobBlocks[CAMERA_WRITE4_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE4_INTERCEPT_KEY, "89 0F 8B 48 04 89 4F 04 8B 40 08 48 8B CE 89 47 08 C7 46 08 00 00 00 00 F3 0F 11 06 F3 0F 11 4E 04 E8", 1);
		aobBlocks[CAMERA_WRITE5_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE5_INTERCEPT_KEY, "F3 0F 11 7F 08 0F 28 7C 24 70 F3 0F 11 07 F3 0F 11 4F 04 49 8B E3 5F", 1);
		aobBlocks[FOV_MIN_CLAMP_LOCATION_KEY] = new AOBBlock(FOV_MIN_CLAMP_LOCATION_KEY, "F3 0F 10 58 58 | F3 0F 5F 1D ?? ?? ?? ?? 0F 28 C3 F3 0F 5D 05 ?? ?? ?? ?? 48", 1);
		aobBlocks[FOV_MAX_CLAMP_LOCATION_KEY] = new AOBBlock(FOV_MAX_CLAMP_LOCATION_KEY, "F3 0F 10 58 58 F3 0F 5F 1D ?? ?? ?? ?? 0F 28 C3 | F3 0F 5D 05 ?? ?? ?? ?? 48", 1);
		aobBlocks[FOV_ADDRESS_LOCATION_KEY] = new AOBBlock(FOV_ADDRESS_LOCATION_KEY, "48 8B 05 | ?? ?? ?? ?? F3 0F 10 58 58 F3 0F5F 1D ?? ?? ?? ?? 0F 28 C3 F3 0F 5D 05 ?? ?? ?? ?? 48", 1);
		aobBlocks[DRAWVIEWMODEL_LOCATION_KEY] = new AOBBlock(DRAWVIEWMODEL_LOCATION_KEY, "48 83 EC 28 48 8B 05 | ?? ?? ?? ?? 83 78 5C 00 75 07 32 C0 48 83 C4 28 C3 83 C9 FF", 1);
		aobBlocks[CAMERA_ANGLE_WRITE_YAW_KEY] = new AOBBlock(CAMERA_ANGLE_WRITE_YAW_KEY, "41 0F 28 CA F3 0F 59 C6 | F3 0F 11 47 04 41 0F 28 C0 E8 9C 3D 1D 00 41 0F 28 CC F3 0F 59 C6 F3 0F 11 07 41 0F 28 C3 E8 87 3D 1D 00 F3 0F 59 C6 F3 0F 11 47 08", 1);
		aobBlocks[CAMERA_ANGLE_WRITE_PITCH_KEY] = new AOBBlock(CAMERA_ANGLE_WRITE_PITCH_KEY, "41 0F 28 CA F3 0F 59 C6 F3 0F 11 47 04 41 0F 28 C0 E8 9C 3D 1D 00 41 0F 28 CC F3 0F 59 C6 | F3 0F 11 07 41 0F 28 C3 E8 87 3D 1D 00 F3 0F 59 C6 F3 0F 11 47 08", 1);
		aobBlocks[CAMERA_ANGLE_WRITE_ROLL_KEY] = new AOBBlock(CAMERA_ANGLE_WRITE_ROLL_KEY, "41 0F 28 CA F3 0F 59 C6 F3 0F 11 47 04 41 0F 28 C0 E8 9C 3D 1D 00 41 0F 28 CC F3 0F 59 C6 F3 0F 11 07 41 0F 28 C3 E8 87 3D 1D 00 F3 0F 59 C6 | F3 0F 11 47 08", 1);
		aobBlocks[PAUSE_UNPAUSE_ROOTOBJECT_LOCATION_KEY] = new AOBBlock(PAUSE_UNPAUSE_ROOTOBJECT_LOCATION_KEY, "48 8B 0D | ?? ?? ?? ?? 48 8D 15 ?? ?? ?? ?? 48 8B 01 FF 90 D8 00 00 00 48 63 0D ?? ?? ?? ?? 48 8B D9", 1);

		map<string, AOBBlock*>::iterator it;
		bool result = true;
		for (it = aobBlocks.begin(); it != aobBlocks.end(); it++)
		{
			result &= it->second->scan(hostImageAddress, hostImageSize);
		}
		if (result)
		{
			OverlayConsole::instance().logLine("All interception offsets found.");
		}
		else
		{
			OverlayConsole::instance().logError("One or more interception offsets weren't found: tools aren't compatible with this game's version.");
		}
	}


	void setCameraStructInterceptorHook(map<string, AOBBlock*> &aobBlocks)
	{
		GameImageHooker::setHook(aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY], 0x11, &_cameraStructInterceptionContinue, &cameraStructInterceptor);
	}


	void setPostCameraStructHooks(map<string, AOBBlock*> &aobBlocks)
	{
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY], 0x14, &_cameraWrite1InterceptionContinue, &cameraWrite1Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY], 0x14, &_cameraWrite2InterceptionContinue, &cameraWrite2Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE3_INTERCEPT_KEY], 0x10, &_cameraWrite3InterceptionContinue, &cameraWrite3Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE4_INTERCEPT_KEY], 0x21, &_cameraWrite4InterceptionContinue, &cameraWrite4Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE5_INTERCEPT_KEY], 0x13, &_cameraWrite5InterceptionContinue, &cameraWrite5Interceptor);
		CameraManipulator::setFoVAddress(Utils::calculateAbsoluteAddress(aobBlocks[FOV_ADDRESS_LOCATION_KEY], 4));	// client.dll+2C5A44 - 48 8B 05 E5CAF200     - mov rax,[client.dll+11F2530]
		CameraManipulator::setPauseUnpauseGameFunctionPointers(Utils::calculateAbsoluteAddress(aobBlocks[PAUSE_UNPAUSE_ROOTOBJECT_LOCATION_KEY], 4));
		nopFoVClamps(aobBlocks);
	}


	void toggleHideModelInFirstPerson(map<string, AOBBlock*> &aobBlocks, bool hide)
	{
		// we're going to pull the address of r_drawviewmodel from memory and set its value. 
		//client.dll+374CE0 - 48 83 EC 28           - sub rsp,28 { 40 }
		//client.dll+374CE4 - 48 8B 05 1DEAF201     - mov rax,[client.dll+22A3708] 			<< Read root address of object which contains r_drawviewmodel
		//client.dll+374CEB - 83 78 5C 00           - cmp dword ptr [rax+5C],00 { 0 }			<< read r_drawviewmodel Set to 0 to disable model in 1st person.
		//client.dll+374CEF - 75 07                 - jne client.dll+374CF8
		//client.dll+374CF1 - 32 C0                 - xor al,al
		//client.dll+374CF3 - 48 83 C4 28           - add rsp,28 { 40 }
		//client.dll+374CF7 - C3                    - ret 
		//client.dll+374CF8 - 83 C9 FF              - or ecx,-01 { 255 }
		//client.dll+374CFB - 48 89 5C 24 20        - mov [rsp+20],rbx
		//client.dll+374D00 - E8 3BA2DDFF           - call client.dll+14EF40
		//client.dll+374D05 - 48 8B D8              - mov rbx,rax
		//client.dll+374D08 - 48 85 C0              - test rax,rax
		//client.dll+374D0B - 0F84 AA000000         - je client.dll+374DBB
		//client.dll+374D11 - 80 B8 D9270000 00     - cmp byte ptr [rax+000027D9],00 { 0 }
		//client.dll+374D18 - 0F85 9D000000         - jne client.dll+374DBB
		//client.dll+374D1E - 8B 90 582C0000        - mov edx,[rax+00002C58]
		//client.dll+374D24 - 83 FA FF              - cmp edx,-01 { 255 }
		//client.dll+374D27 - 74 28                 - je client.dll+374D51

		__int64* pRootObject = (__int64*)Utils::calculateAbsoluteAddress(aobBlocks[DRAWVIEWMODEL_LOCATION_KEY], 4);
		DWORD* drawViewModel = (DWORD*)((*pRootObject) + 0x5C);
		if (hide)
		{
			*drawViewModel = 0;
		}
		else
		{
			*drawViewModel = 1;
		}
	}


	// These angle writes are blocked using a NOP range, as they can't be intercepted because there's a call with a relative address in between. 
	// if enable, writes are enabled, otherwise they're NOPed
	void toggleAngleWrites(map<string, AOBBlock*> &aobBlocks, bool enabled)
	{
		//client.dll+62D47A - 41 0F28 CA            - movaps xmm1,xmm10
		//client.dll+62D47E - F3 0F59 C6            - mulss xmm0,xmm6
		//client.dll+62D482 - F3 0F11 47 04         - movss [rdi+04],xmm0					<< WRITE yaw rot
		//client.dll+62D487 - 41 0F28 C0            - movaps xmm0,xmm8
		//client.dll+62D48B - E8 9C3D1D00           - call client.dll+80122C
		//client.dll+62D490 - 41 0F28 CC            - movaps xmm1,xmm12
		//client.dll+62D494 - F3 0F59 C6            - mulss xmm0,xmm6
		//client.dll+62D498 - F3 0F11 07            - movss [rdi],xmm0						<< WRITE pitch rot
		//client.dll+62D49C - 41 0F28 C3            - movaps xmm0,xmm11
		//client.dll+62D4A0 - E8 873D1D00           - call client.dll+80122C
		//client.dll+62D4A5 - F3 0F59 C6            - mulss xmm0,xmm6
		//client.dll+62D4A9 - F3 0F11 47 08         - movss [rdi+08],xmm0					<< WRITE roll rot
		//client.dll+62D4AE - EB 49                 - jmp client.dll+62D4F9
		if (enabled)
		{
			// restore with original code
			// pitch
			BYTE statementBytesPitch[4] = { 0xF3, 0x0F, 0x11, 0x07 };						//client.dll+62D498 - F3 0F11 07            - movss [rdi],xmm0
			GameImageHooker::writeRange(aobBlocks[CAMERA_ANGLE_WRITE_PITCH_KEY], statementBytesPitch, 4);
			BYTE statementBytesYaw[5] = { 0xF3, 0x0F, 0x11, 0x47, 0x04 };							//client.dll+62D482 - F3 0F11 47 04         - movss [rdi+04],xmm0
			GameImageHooker::writeRange(aobBlocks[CAMERA_ANGLE_WRITE_YAW_KEY], statementBytesYaw, 5);
			BYTE statementBytesRoll[5] = { 0xF3, 0x0F, 0x11, 0x47, 0x08 };							//client.dll+62D482 - F3 0F11 47 04         - movss [rdi+04],xmm0
			GameImageHooker::writeRange(aobBlocks[CAMERA_ANGLE_WRITE_ROLL_KEY], statementBytesRoll, 5);
		}
		else
		{
			// nop ranges
			// pitch
			GameImageHooker::nopRange(aobBlocks[CAMERA_ANGLE_WRITE_PITCH_KEY], 4);		//client.dll+62D498 - F3 0F11 07            - movss [rdi],xmm0
			// yaw
			GameImageHooker::nopRange(aobBlocks[CAMERA_ANGLE_WRITE_YAW_KEY], 5);		//client.dll+62D482 - F3 0F11 47 04         - movss [rdi+04],xmm0
			// roll
			GameImageHooker::nopRange(aobBlocks[CAMERA_ANGLE_WRITE_ROLL_KEY], 5);		//client.dll+62D4A9 - F3 0F11 47 08         - movss [rdi+08],xmm0
		}
	}


	// nops the statements which clamps the fov min/max. 
	void nopFoVClamps(map<string, AOBBlock*> &aobBlocks)
	{
		//client.dll+2C5A44 - 48 8B 05 E5CAF200     - mov rax,[client.dll+11F2530] { [7FF89021DE00] }
		//client.dll+2C5A4B - F3 0F10 58 58         - movss xmm3,[rax+58]									<<< READ FOV
		//client.dll+2C5A50 - F3 0F5F 1D 20277400   - maxss xmm3,[client.dll+A08178] { [1.00] }			<< CLAMP MIN
		//client.dll+2C5A58 - 0F28 C3               - movaps xmm0,xmm3
		//client.dll+2C5A5B - F3 0F5D 05 D9906400   - minss xmm0,[client.dll+90EB3C] { [1.70] }			<< CLAMP MAX
		//client.dll+2C5A63 - 48 83 C4 28           - add rsp,28 { 40 }

		// we simply write the maxss and minss statements with NOPs. 
		GameImageHooker::nopRange(aobBlocks[FOV_MIN_CLAMP_LOCATION_KEY]->absoluteAddress(), 8);
		GameImageHooker::nopRange(aobBlocks[FOV_MAX_CLAMP_LOCATION_KEY]->absoluteAddress(), 8);
	}
}
