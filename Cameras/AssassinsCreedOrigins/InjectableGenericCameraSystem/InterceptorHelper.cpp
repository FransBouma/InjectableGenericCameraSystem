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
	void fovReadInterceptor();
	void resolutionScaleReadInterceptor();
	void todWriteInterceptor();
	void timestopReadInterceptor();
}

// external addresses used in asm.
extern "C" {
	LPBYTE _cameraStructInterceptionContinue = nullptr;
	LPBYTE _cameraWrite1InterceptionContinue = nullptr;
	LPBYTE _cameraWrite2InterceptionContinue = nullptr;
	LPBYTE _cameraWrite3InterceptionContinue = nullptr;
	LPBYTE _cameraWrite4InterceptionContinue = nullptr;
	LPBYTE _cameraWrite5InterceptionContinue = nullptr;
	LPBYTE _fovReadInterceptionContinue = nullptr;
	LPBYTE _resolutionScaleReadInterceptionContinue = nullptr;
	LPBYTE _todWriteInterceptionContinue = nullptr;
	LPBYTE _timestopReadInterceptionContinue = nullptr;
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
		aobBlocks[CAMERA_WRITE5_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE5_INTERCEPT_KEY, "0F 29 1F 48 8B 43 38 0F 28 80 A0 0B 00 00 0F 29 45 00 48 8B 43 38", 1);
		aobBlocks[FOV_WRITE1_INTERCEPT_KEY] = new AOBBlock(FOV_WRITE1_INTERCEPT_KEY, "F3 0F 11 B7 64 02 00 00 48 8B CF 89 87 ?? 07 00 00 E8", 1);
		aobBlocks[FOV_WRITE2_INTERCEPT_KEY] = new AOBBlock(FOV_WRITE2_INTERCEPT_KEY, "F3 44 0F 11 93 64 02 00 00 49 8B D5 89 83 ?? 07 00 00 48 8B CB E8", 1);
		aobBlocks[FOV_READ_INTERCEPT_KEY] = new AOBBlock(FOV_READ_INTERCEPT_KEY, "F3 41 0F 10 94 24 24 01 00 00 45 0F 57 D2 41 0F 2F D2 F3 41 0F 10 8F 64 02 00 00", 1);
		aobBlocks[PHOTOMODE_ENABLE_ALWAYS_KEY] = new AOBBlock(PHOTOMODE_ENABLE_ALWAYS_KEY, "74 ?? E8 ?? ?? ?? ?? 80 B8 A9 02 00 00 00 74 ?? B8 01 00 00 00 48 81 C4", 1);
		aobBlocks[RESOLUTION_SCALE_INTERCEPT_KEY] = new AOBBlock(RESOLUTION_SCALE_INTERCEPT_KEY, "41 8B 86 A8 00 00 00 48 8B CD 89 85 ?? 07 00 00", 1);
		aobBlocks[RESOLUTION_SCALE_MENU_KEY] = new AOBBlock(RESOLUTION_SCALE_MENU_KEY, "F3 0F 5C 05 | ?? ?? ?? ?? 0F 54 05 ?? ?? ?? ?? 0F 2F 05 ?? ?? ?? ?? 0F 96 C0 22 D0 84 D2 0F 94 C0", 1);
		aobBlocks[TOD_WRITE_INTERCEPT_KEY] = new AOBBlock(TOD_WRITE_INTERCEPT_KEY, "F3 0F 11 00 48 8B 83 40 02 00 00 F3 0F 10 08 0F 2F CA", 1);
		aobBlocks[TIMESTOP_READ_INTERCEPT_KEY] = new AOBBlock(TIMESTOP_READ_INTERCEPT_KEY, "44 8B 85 C4 61 00 00 48 8B 95 BC 61 00 00 83 B9 58 14 00 00 00", 1);

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
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY], 0x0F, &_cameraWrite1InterceptionContinue, &cameraWrite1Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY], 0x10, &_cameraWrite2InterceptionContinue, &cameraWrite2Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE3_INTERCEPT_KEY], 0x10, &_cameraWrite3InterceptionContinue, &cameraWrite3Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE4_INTERCEPT_KEY], 0x10, &_cameraWrite4InterceptionContinue, &cameraWrite4Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE5_INTERCEPT_KEY], 0x12, &_cameraWrite5InterceptionContinue, &cameraWrite5Interceptor);
		GameImageHooker::setHook(aobBlocks[FOV_READ_INTERCEPT_KEY], 0x1B, &_fovReadInterceptionContinue, &fovReadInterceptor);
		GameImageHooker::setHook(aobBlocks[RESOLUTION_SCALE_INTERCEPT_KEY], 0x10, &_resolutionScaleReadInterceptionContinue, &resolutionScaleReadInterceptor);
		GameImageHooker::setHook(aobBlocks[TOD_WRITE_INTERCEPT_KEY], 0x0F, &_todWriteInterceptionContinue, &todWriteInterceptor);
		GameImageHooker::setHook(aobBlocks[TIMESTOP_READ_INTERCEPT_KEY], 0x15, &_timestopReadInterceptionContinue, &timestopReadInterceptor);
		CameraManipulator::setResolutionScaleMenuValueAddress(Utils::calculateAbsoluteAddress(aobBlocks[RESOLUTION_SCALE_MENU_KEY], 4)); // ACOrigins.exe+813E74 - F3 0F5C 05 A87CDE03   - subss xmm0,[ACOrigins.exe+45FBB24]
		enablePhotomodeEverywhere(aobBlocks);
	}


	void enablePhotomodeEverywhere(map<string, AOBBlock*> &aobBlocks)
	{
		//ACOrigins.exe+28F568B - E8 F0BBC5FE           - call ACOrigins.exe+1551280
		//ACOrigins.exe+28F5690 - 48 05 68030000        - add rax,00000368 { 872 }
		//ACOrigins.exe+28F5696 - 74 13                 - je ACOrigins.exe+28F56AB
		//ACOrigins.exe+28F5698 - 48 8D 15 91C5C7FE     - lea rdx,[ACOrigins.exe+1571C30] { [-9.52] }
		//ACOrigins.exe+28F569F - 48 8B C8              - mov rcx,rax
		//ACOrigins.exe+28F56A2 - E8 59C0C7FE           - call ACOrigins.exe+1571700
		//ACOrigins.exe+28F56A7 - 84 C0                 - test al,al
		//ACOrigins.exe+28F56A9 - 74 D5                 - je ACOrigins.exe+28F5680						<< NOP and photomode is usable in cutscenes. 
		//ACOrigins.exe+28F56AB - E8 30E34AFF           - call ACOrigins.exe+1DA39E0
		//ACOrigins.exe+28F56B0 - 80 B8 A9020000 00     - cmp byte ptr [rax+000002A9],00 { 0 }			<< is 1 if photomode is enabled.
		//ACOrigins.exe+28F56B7 - 74 0E                 - je ACOrigins.exe+28F56C7
		//ACOrigins.exe+28F56B9 - B8 01000000           - mov eax,00000001 { 1 }
		//ACOrigins.exe+28F56BE - 48 81 C4 20010000     - add rsp,00000120 { 288 }
		//ACOrigins.exe+28F56C5 - 5B                    - pop rbx
		//ACOrigins.exe+28F56C6 - C3                    - ret 

		GameImageHooker::nopRange(aobBlocks[PHOTOMODE_ENABLE_ALWAYS_KEY], 2);			// ACOrigins.exe+28F56A9 - 74 D5                 - je ACOrigins.exe+28F5680
	}


	// if 'enabled' is false, we'll nop the range where the FoV writes are placed, otherwise we'll write back the original statement bytes. 
	void toggleFoVWriteState(map<string, AOBBlock*> &aobBlocks, bool enabled)
	{
		// fov write 1
		// v1.0.5
		//0000000141054213 | 74 0C                    | je acorigins_dump.141054221      
		//0000000141054215 | 8B 8F 64 02 00 00        | mov ecx,dword ptr ds:[rdi+264]   
		//000000014105421B | 89 8F 68 07 00 00        | mov dword ptr ds:[rdi+768],ecx   
		//0000000141054221 | 48 8D 54 24 20           | lea rdx,qword ptr ss:[rsp+20]    
		//0000000141054226 | F3 0F 11 B7 64 02 00 00  | movss dword ptr ds:[rdi+264],xmm6			<< FOV WRITE
		//000000014105422E | 48 8B CF                 | mov rcx,rdi                      
		//0000000141054231 | 89 87 6C 07 00 00        | mov dword ptr ds:[rdi+76C],eax   
		//0000000141054237 | E8 D4 45 7E FF           | call acorigins_dump.140838810    
		//000000014105423C | 33 D2                    | xor edx,edx                      
		//000000014105423E | 89 97 34 07 00 00        | mov dword ptr ds:[rdi+734],edx   
		//0000000141054244 | 41 8B 46 20              | mov eax,dword ptr ds:[r14+20]    

		// fov write 2
		// v1.0.5
		//0000000141097E29 | EB 22                        | jmp acorigins_dump.141097E4D      
		//0000000141097E2B | E8 60 2B 2D 00               | call acorigins_dump.14136A990     
		//0000000141097E30 | 39 83 6C 07 00 00            | cmp dword ptr ds:[rbx+76C],eax    
		//0000000141097E36 | 74 0C                        | je acorigins_dump.141097E44       
		//0000000141097E38 | 8B 8B 64 02 00 00            | mov ecx,dword ptr ds:[rbx+264]    
		//0000000141097E3E | 89 8B 68 07 00 00            | mov dword ptr ds:[rbx+768],ecx    
		//0000000141097E44 | F3 44 0F 11 93 64 02 00 00   | movss dword ptr ds:[rbx+264],xmm10		<< FOV WRITE
		//0000000141097E4D | 49 8B D5                     | mov rdx,r13                       
		//0000000141097E50 | 89 83 6C 07 00 00            | mov dword ptr ds:[rbx+76C],eax    
		//0000000141097E56 | 48 8B CB                     | mov rcx,rbx                       
		//0000000141097E59 | E8 B2 09 7A FF               | call acorigins_dump.140838810     
		//0000000141097E5E | 33 D2                        | xor edx,edx                       
		//0000000141097E60 | 89 93 34 07 00 00            | mov dword ptr ds:[rbx+734],edx    
		//0000000141097E66 | 41 8B 46 20                  | mov eax,dword ptr ds:[r14+20]     

		if (enabled)
		{
			// enable writes
			BYTE originalStatementBytes1[8] = { 0xF3, 0x0F, 0x11, 0xB7, 0x64, 0x02, 0x00, 0x00 };			// 0000000141054226 | F3 0F 11 B7 64 02 00 00  | movss dword ptr ds:[rdi+264],xmm6
			GameImageHooker::writeRange(aobBlocks[FOV_WRITE1_INTERCEPT_KEY], originalStatementBytes1, 8);
			BYTE originalStatementBytes2[9] = { 0xF3, 0x44, 0x0F, 0x11, 0x93, 0x64, 0x02, 0x00, 0x00 };		// 0000000141097E44 | F3 44 0F 11 93 64 02 00 00   | movss dword ptr ds:[rbx+264],xmm10
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
