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
}

// external addresses used in asm.
extern "C" {
	LPBYTE _cameraStructInterceptionContinue = nullptr;
	LPBYTE _cameraWrite1InterceptionContinue = nullptr;
	LPBYTE _cameraWrite2InterceptionContinue = nullptr;
	LPBYTE _cameraWrite3InterceptionContinue = nullptr;
	LPBYTE _cameraWrite4InterceptionContinue = nullptr;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*> &aobBlocks)
	{
		aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_ADDRESS_INTERCEPT_KEY, "89 83 80 FF FF FF 8B 83 7C FF FF FF 89 83 84 FF FF FF 0F 28 83 A0 FF FF FF", 1);	
		aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE1_INTERCEPT_KEY, "66 0F 7F 83 A0 FF FF FF 0F 28 8B F0 01 00 00 66 0F 7F 8B B0 FF FF FF", 1);
		aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE2_INTERCEPT_KEY, "41 0F 29 67 60 0F 28 C3 0F C6 C1 FE 0F C6 D8 84 0F 28 C5 41 0F 29 5F 70", 1);
		aobBlocks[CAMERA_WRITE3_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE3_INTERCEPT_KEY, "41 0F 29 AF 80 00 00 000F C6 D0 84 41 0F 29 97 90 00 00 00", 1);
		aobBlocks[CAMERA_WRITE4_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE4_INTERCEPT_KEY, "0F C6 83 B0 FF FF FF FE 0F C6 D0 84 0F 29 93 B0 FF FF FF", 1);
		aobBlocks[FOV_WRITE_INTERCEPT_KEY] = new AOBBlock(FOV_WRITE_INTERCEPT_KEY, "F3 0F 11 83 78 FF FF FF 48 89 E9 E8 E1 6F E8 F4 89 FA", 1);

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
		GameImageHooker::setHook(aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY], 0x12, &_cameraStructInterceptionContinue, &cameraStructInterceptor);
	}


	void setPostCameraStructHooks(map<string, AOBBlock*> &aobBlocks)
	{
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY], 0x35, &_cameraWrite1InterceptionContinue, &cameraWrite1Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY], 0x18, &_cameraWrite2InterceptionContinue, &cameraWrite2Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE3_INTERCEPT_KEY], 0x14, &_cameraWrite3InterceptionContinue, &cameraWrite3Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE4_INTERCEPT_KEY], 0x13, &_cameraWrite4InterceptionContinue, &cameraWrite4Interceptor);
	}


	// if 'enabled' is true, we'll nop the range where the FoV writes are placed, otherwise we'll write back the original statement bytes. 
	void toggleFoVWriteState(map<string, AOBBlock*> &aobBlocks, bool enabled)
	{
		// fov write 
		//SOTTR.exe+BB1A228 - EB 13                 - jmp SOTTR.exe+BB1A23D
		//SOTTR.exe+BB1A22A - 48 8D 4C 24 60        - lea rcx,[rsp+60]
		//SOTTR.exe+BB1A22F - E8 2C3A65F4           - call SOTTR.exe+16DC60
		//SOTTR.exe+BB1A234 - F3 0F10 84 24 10010000  - movss xmm0,[rsp+00000110]
		//SOTTR.exe+BB1A23D - 89 FA                 - mov edx,edi								<< ENDPOINT OF JUMP
		//SOTTR.exe+BB1A23F - F3 0F11 83 78FFFFFF   - movss [rbx-00000088],xmm0				<< FOV WRITE. NOP instead of intercept.
		//SOTTR.exe+BB1A247 - 48 89 E9              - mov rcx,rbp
		//SOTTR.exe+BB1A24A - E8 E16FE8F4           - call SOTTR.exe+9A1230
		//SOTTR.exe+BB1A24F - 89 FA                 - mov edx,edi
		//SOTTR.exe+BB1A251 - F3 0F11 83 88FFFFFF   - movss [rbx-00000078],xmm0
		//SOTTR.exe+BB1A259 - 48 89 E9              - mov rcx,rbp

		if (enabled)
		{
			// enable writes
			BYTE originalStatementBytes[8] = { 0xF3, 0x0F, 0x11, 0x83, 0x78, 0xFF, 0xFF, 0xFF};			// SOTTR.exe+BB1A23F - F3 0F11 83 78FFFFFF   - movss [rbx-00000088],xmm0
			GameImageHooker::writeRange(aobBlocks[FOV_WRITE_INTERCEPT_KEY], originalStatementBytes, 8);
		}
		else
		{
			// disable writes, by nopping the range.
			GameImageHooker::nopRange(aobBlocks[FOV_WRITE_INTERCEPT_KEY], 8);
		}
	}
}
