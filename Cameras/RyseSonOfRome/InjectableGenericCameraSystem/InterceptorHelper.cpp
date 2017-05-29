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
#include "Console.h"

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
	void cameraWrite6Interceptor();
	void cameraWrite7Interceptor();
	void cameraWrite8Interceptor();
	void cameraWrite9Interceptor();
	void timestopInterceptor();
	void fovWriteInterceptor();
}

// external addresses used in asm.
extern "C" {
	LPBYTE _cameraStructInterceptionContinue = nullptr;
	LPBYTE _cameraWrite1InterceptionContinue = nullptr;
	LPBYTE _cameraWrite2InterceptionContinue = nullptr;
	LPBYTE _cameraWrite3InterceptionContinue = nullptr;
	LPBYTE _cameraWrite4InterceptionContinue = nullptr;
	LPBYTE _cameraWrite5InterceptionContinue = nullptr;
	LPBYTE _cameraWrite6InterceptionContinue = nullptr;
	LPBYTE _cameraWrite7InterceptionContinue = nullptr;
	LPBYTE _cameraWrite8InterceptionContinue = nullptr;
	LPBYTE _cameraWrite9InterceptionContinue = nullptr;
	LPBYTE _timestopInterceptionContinue = nullptr;
	LPBYTE _fovWriteInterceptionContinue = nullptr;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*> &aobBlocks)
	{
		aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_ADDRESS_INTERCEPT_KEY, "89 06 8B 44 24 54 F3 0F 59 F0 F3 0F 59 F8 F3 44 0F10 45 D8 F3 0F 11 75 D0", 1);	
		aobBlocks[FOV_INTERCEPT_KEY] = new AOBBlock(FOV_INTERCEPT_KEY, "88 83 A9 00 00 00 F3 0F 58 83 A4 00 00 00 F3 0F 11 43 30", 1);
		aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE1_INTERCEPT_KEY, "89 06 F3 44 0F 11 5C 24 44 F3 44 0F 11 44 24 48 8B 44 24 44 89 46 04 8B 44 24 48 89 46 08", 1);
		aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE2_INTERCEPT_KEY, "89 03 8B 45 AB 89 43 04 F3 0F 11 45 AF 8B 45 AF 89 43 08 8B 83", 1);
		aobBlocks[CAMERA_WRITE3_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE3_INTERCEPT_KEY, "89 01 8B 42 04 89 41 04 8B 42 08 89 41 08 8B 42 0C 89 41 0C 8B 42 10 89 41 10 8B 42 14 89 41 14 8B 42 18 89 41 18 8B 42 1C 89 41 1C 8B 42 20 89 41 20 8B 42 24 89 41 24 8B 42 28 89 41 28 8B 42 2C 89 41 2C 8B 42 30 89 41 30 0F B6 42 34", 1);
		aobBlocks[CAMERA_WRITE4_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE4_INTERCEPT_KEY, "F3 44 0F11 46 18 0F 28 CF 0F 28 D5 F3 0F 59 C5 F3 0F 59 E6 F3 0F 5C E0 41", 1);
		aobBlocks[CAMERA_WRITE5_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE5_INTERCEPT_KEY, "89 07 F3 0F 11 4C 24 44 F3 41 0F 58 D1 8B 44 24 44 89 47 04", 1);
		aobBlocks[CAMERA_WRITE6_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE6_INTERCEPT_KEY, "F3 44 0F 11 57 0C F3 0F 11 17 F3 0F 11 5F 04 F3 0F 59 E1 F3 0F 11 67 08", 1);
		aobBlocks[CAMERA_WRITE7_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE7_INTERCEPT_KEY, "89 43 0C 44 0F 28 8C 24 90 00 00 00 44 0F 28 84 24 A0 00 00 00", 1);
		aobBlocks[CAMERA_WRITE8_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE8_INTERCEPT_KEY, "F3 44 0F 5C D2 F3 44 0F 11 57 0C 8B 43 54 89 44 24 40", 1);
		aobBlocks[CAMERA_WRITE9_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE9_INTERCEPT_KEY, "F3 0F 5C DA F3 0F 11 5B 18 8B 83 94 00 00 00 89 45 A7", 1);
		aobBlocks[TIMESTOP_INTERCEPT_KEY] = new AOBBlock(TIMESTOP_INTERCEPT_KEY, "F2 48 0F 2A 47 40 F2 0F 5E D0 66 0F EF C0 F2 48 0F 2A C1 F2 0F 59 C2", 1);
		
		map<string, AOBBlock*>::iterator it;
		bool result = true;
		for (it = aobBlocks.begin(); it != aobBlocks.end(); it++)
		{
			result &= it->second->scan(hostImageAddress, hostImageSize);
		}
		if (result)
		{
			Console::WriteLine("All interception offsets found.");
		}
		else
		{
			Console::WriteError("One or more interception offsets weren't found: tools aren't compatible with this game's version.");
		}
	}


	void setCameraStructInterceptorHook(map<string, AOBBlock*> &aobBlocks)
	{
		GameImageHooker::setHook(aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY], 0x35, &_cameraStructInterceptionContinue, &cameraStructInterceptor);
	}


	void setPostCameraStructHooks(map<string, AOBBlock*> &aobBlocks)
	{
		GameImageHooker::setHook(aobBlocks[FOV_INTERCEPT_KEY], 0x13, &_fovWriteInterceptionContinue, &fovWriteInterceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY], 0x1E, &_cameraWrite1InterceptionContinue, &cameraWrite1Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY], 0x13, &_cameraWrite2InterceptionContinue, &cameraWrite2Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE3_INTERCEPT_KEY], 0x4A, &_cameraWrite3InterceptionContinue, &cameraWrite3Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE4_INTERCEPT_KEY], 0xA8, &_cameraWrite4InterceptionContinue, &cameraWrite4Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE5_INTERCEPT_KEY], 0x21, &_cameraWrite5InterceptionContinue, &cameraWrite5Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE6_INTERCEPT_KEY], 0x18, &_cameraWrite6InterceptionContinue, &cameraWrite6Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE7_INTERCEPT_KEY], 0x2B, &_cameraWrite7InterceptionContinue, &cameraWrite7Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE8_INTERCEPT_KEY], 0xE, &_cameraWrite8InterceptionContinue, &cameraWrite8Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE9_INTERCEPT_KEY], 0xF, &_cameraWrite9InterceptionContinue, &cameraWrite9Interceptor);
		GameImageHooker::setHook(aobBlocks[TIMESTOP_INTERCEPT_KEY], 0x1B, &_timestopInterceptionContinue, &timestopInterceptor);
	}
}
