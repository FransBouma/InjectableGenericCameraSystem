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
	void fovReadInterceptor();
	void resolutionScaleReadInterceptor();
	void todWriteInterceptor();
}

// external addresses used in asm.
extern "C" {
	LPBYTE _cameraStructInterceptionContinue = nullptr;
	LPBYTE _cameraWrite1InterceptionContinue = nullptr;
	LPBYTE _cameraWrite2InterceptionContinue = nullptr;
	LPBYTE _cameraWrite3InterceptionContinue = nullptr;
	LPBYTE _cameraWrite4InterceptionContinue = nullptr;
	LPBYTE _fovReadInterceptionContinue = nullptr;
	LPBYTE _resolutionScaleReadInterceptionContinue = nullptr;
	LPBYTE _todWriteInterceptionContinue = nullptr;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*> &aobBlocks)
	{
		aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_ADDRESS_INTERCEPT_KEY, "48 8B CF 0F 28 00 0F 29 87 E0 02 00 00 0F 29 47 60 E8", 1);	
		aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE1_INTERCEPT_KEY, "49 8B D4 41 0F 29 04 24 48 8B 8F 40 03 00 00 48 8B 49 50 E8", 1);
		aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE2_INTERCEPT_KEY, "0F 58 E5 0F 29 26 41 0F 29 24 24 F3 41 0F 10 07 F3 0F 5F 05", 1);
		aobBlocks[CAMERA_WRITE3_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE3_INTERCEPT_KEY, "44 0F 29 A7 70 04 00 00 45 0F 28 63 90 49 8B E3", 1);
		aobBlocks[CAMERA_WRITE4_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE4_INTERCEPT_KEY, "0F 28 DC 0F 29 A7 80 04 00 00 0F 59 DD 0F 28 CB 0F 28 C3 0F C6 CB AA 0F C6 C3 55 F3 0F 58 C1", 1);
		aobBlocks[FOV_WRITE1_INTERCEPT_KEY] = new AOBBlock(FOV_WRITE1_INTERCEPT_KEY, "F3 0F 11 B7 64 02 00 00 48 8B CF 89 87 5C 07 00 00", 1);
		aobBlocks[FOV_WRITE2_INTERCEPT_KEY] = new AOBBlock(FOV_WRITE2_INTERCEPT_KEY, "F3 44 0F 11 93 64 02 00 00 49 8B D5 89 83 5C 07 00 00 48 8B CB E8", 1);
		aobBlocks[FOV_READ_INTERCEPT_KEY] = new AOBBlock(FOV_READ_INTERCEPT_KEY, "F3 41 0F 10 94 24 24 01 00 00 45 0F 57 D2 41 0F 2F D2 F3 41 0F 10 8F 64 02 00 00", 1);
		aobBlocks[PHOTOMODE_ENABLE_ALWAYS_KEY] = new AOBBlock(PHOTOMODE_ENABLE_ALWAYS_KEY, "74 ?? E8 ?? ?? ?? ?? 80 B8 A9 02 00 00 00 74 ?? B8 01 00 00 00 48 81 C4", 1);
		aobBlocks[RESOLUTION_SCALE_INTERCEPT_KEY] = new AOBBlock(RESOLUTION_SCALE_INTERCEPT_KEY, "41 8B 86 A8 00 00 00 48 8B CD 89 85 20 07 00 00", 1);
		aobBlocks[RESOLUTION_SCALE_MENU_KEY] = new AOBBlock(RESOLUTION_SCALE_MENU_KEY, "F3 0F 5C 05 | ?? ?? ?? ?? 0F 54 05 ?? ?? ?? ?? 0F 2F 05 ?? ?? ?? ?? 0F 96 C0 22 D0 84 D2 0F 94 C0", 1);
		aobBlocks[TOD_WRITE_INTERCEPT_KEY] = new AOBBlock(TOD_WRITE_INTERCEPT_KEY, "F3 0F 11 00 48 8B 83 40 02 00 00 F3 0F 10 08 0F 2F CA", 1);

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


	void setPostCameraStructHooks(map<string, AOBBlock*> &aobBlocks, LPBYTE hostImageAddress)
	{
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY], 0x0F, &_cameraWrite1InterceptionContinue, &cameraWrite1Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY], 0x10, &_cameraWrite2InterceptionContinue, &cameraWrite2Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE3_INTERCEPT_KEY], 0x10, &_cameraWrite3InterceptionContinue, &cameraWrite3Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE4_INTERCEPT_KEY], 0x10, &_cameraWrite4InterceptionContinue, &cameraWrite4Interceptor);
		GameImageHooker::setHook(aobBlocks[FOV_READ_INTERCEPT_KEY], 0x1B, &_fovReadInterceptionContinue, &fovReadInterceptor);
		GameImageHooker::setHook(aobBlocks[RESOLUTION_SCALE_INTERCEPT_KEY], 0x10, &_resolutionScaleReadInterceptionContinue, &resolutionScaleReadInterceptor);
		GameImageHooker::setHook(aobBlocks[TOD_WRITE_INTERCEPT_KEY], 0x0F, &_todWriteInterceptionContinue, &todWriteInterceptor);
		CameraManipulator::setResolutionScaleMenuValueAddress(Utils::calculateAbsoluteAddress(aobBlocks[RESOLUTION_SCALE_MENU_KEY], 4)); // ACOrigins.exe+813E74 - F3 0F5C 05 A87CDE03   - subss xmm0,[ACOrigins.exe+45FBB24]
		enablePhotomodeEverywhere(aobBlocks);
	}


	void enablePhotomodeEverywhere(map<string, AOBBlock*> &aobBlocks)
	{
		//ACOrigins.exe+28D0AA6 - 74 13                 - je ACOrigins.exe+28D0ABB
		//ACOrigins.exe+28D0AA8 - 48 8D 15 517AC9FE     - lea rdx,[ACOrigins.exe+1568500] { [-9.52] }
		//ACOrigins.exe+28D0AAF - 48 8B C8              - mov rcx,rax
		//ACOrigins.exe+28D0AB2 - E8 4975C9FE           - call ACOrigins.exe+1568000
		//ACOrigins.exe+28D0AB7 - 84 C0                 - test al,al
		//ACOrigins.exe+28D0AB9 - 74 D5                 - je ACOrigins.exe+28D0A90						<< NOP and photomode is usable in cutscenes. 
		//ACOrigins.exe+28D0ABB - E8 606C4BFF           - call ACOrigins.exe+1D87720
		//ACOrigins.exe+28D0AC0 - 80 B8 A9020000 00     - cmp byte ptr [rax+000002A9],00 { 0 }
		//ACOrigins.exe+28D0AC7 - 74 0E                 - je ACOrigins.exe+28D0AD7
		//ACOrigins.exe+28D0AC9 - B8 01000000           - mov eax,00000001 { 1 }
		//ACOrigins.exe+28D0ACE - 48 81 C4 20010000     - add rsp,00000120 { 288 }
		//ACOrigins.exe+28D0AD5 - 5B                    - pop rbx
		//ACOrigins.exe+28D0AD6 - C3                    - ret 

		GameImageHooker::nopRange(aobBlocks[PHOTOMODE_ENABLE_ALWAYS_KEY], 2);			// ACOrigins.exe+28D0AB9 - 74 D5                 - je ACOrigins.exe+28D0A90
	}


	// if 'enabled' is false, we'll nop the range where the FoV writes are placed, otherwise we'll write back the original statement bytes. 
	void toggleFoVWriteState(map<string, AOBBlock*> &aobBlocks, bool enabled)
	{
		// fov write 1
		// ACOrigins.exe+1058743 - 74 0C                 - je ACOrigins.exe+1058751
		// ACOrigins.exe+1058745 - 8B 8F 64020000        - mov ecx,[rdi+00000264]
		// ACOrigins.exe+105874B - 89 8F 58070000        - mov [rdi+00000758],ecx
		// ACOrigins.exe+1058751 - 48 8D 54 24 20        - lea rdx,[rsp+20]
		// ACOrigins.exe+1058756 - F3 0F11 B7 64020000   - movss [rdi+00000264],xmm6			    << FOV WRITE
		// ACOrigins.exe+105875E - 48 8B CF              - mov rcx,rdi
		// ACOrigins.exe+1058761 - 89 87 5C070000        - mov [rdi+0000075C],eax
		// ACOrigins.exe+1058767 - E8 344C7EFF           - call ACOrigins.exe+83D3A0
		// ACOrigins.exe+105876C - 33 D2                 - xor edx,edx
		// ACOrigins.exe+105876E - 89 97 24070000        - mov [rdi+00000724],edx
		// ACOrigins.exe+1058774 - 41 8B 46 20           - mov eax,[r14+20]

		// fov write 2
		// ACOrigins.exe+109C569 - EB 22                 - jmp ACOrigins.exe+109C58D
		// ACOrigins.exe+109C56B - E8 F0392D00           - call ACOrigins.exe+136FF60
		// ACOrigins.exe+109C570 - 39 83 5C070000        - cmp [rbx+0000075C],eax
		// ACOrigins.exe+109C576 - 74 0C                 - je ACOrigins.exe+109C584
		// ACOrigins.exe+109C578 - 8B 8B 64020000        - mov ecx,[rbx+00000264]
		// ACOrigins.exe+109C57E - 89 8B 58070000        - mov [rbx+00000758],ecx
		// ACOrigins.exe+109C584 - F3 44 0F11 93 64020000  - movss [rbx+00000264],xmm10			<< FOV WRITE
		// ACOrigins.exe+109C58D - 49 8B D5              - mov rdx,r13
		// ACOrigins.exe+109C590 - 89 83 5C070000        - mov [rbx+0000075C],eax
		// ACOrigins.exe+109C596 - 48 8B CB              - mov rcx,rbx
		// ACOrigins.exe+109C599 - E8 020E7AFF           - call ACOrigins.exe+83D3A0
		// ACOrigins.exe+109C59E - 33 D2                 - xor edx,edx
		// ACOrigins.exe+109C5A0 - 89 93 24070000        - mov [rbx+00000724],edx
		// ACOrigins.exe+109C5A6 - 41 8B 46 20           - mov eax,[r14+20]

		if (enabled)
		{
			// enable writes
			byte originalStatementBytes1[8] = { 0xF3, 0x0F, 0x11, 0xB7, 0x64, 0x02, 0x00, 0x00 };			// ACOrigins.exe+1058756 - F3 0F11 B7 64020000   - movss [rdi+00000264],xmm6
			GameImageHooker::writeRange(aobBlocks[FOV_WRITE1_INTERCEPT_KEY], originalStatementBytes1, 8);
			byte originalStatementBytes2[9] = { 0xF3, 0x44, 0x0F, 0x11, 0x93, 0x64, 0x02, 0x00, 0x00 };		// ACOrigins.exe+109C584 - F3 44 0F11 93 64020000  - movss [rbx+00000264],xmm10
			GameImageHooker::writeRange(aobBlocks[FOV_WRITE2_INTERCEPT_KEY], originalStatementBytes2, 9);
		}
		else
		{
			// disable writes, by nopping the range.
			GameImageHooker::nopRange(aobBlocks[FOV_WRITE1_INTERCEPT_KEY], 8);
			GameImageHooker::nopRange(aobBlocks[FOV_WRITE2_INTERCEPT_KEY], 9);
		}
	}
}
