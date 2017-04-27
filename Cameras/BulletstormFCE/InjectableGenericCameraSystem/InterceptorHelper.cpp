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
	void cameraCinematicsStructInterceptor();
}

// external addresses used in asm.
extern "C" {
	LPBYTE _cameraStructInterceptionContinue = nullptr;
	LPBYTE _cameraCinematicsStructInterceptionContinue = nullptr;
	// the continue address for continuing execution after camera write interception. 
	LPBYTE _cameraWrite1InterceptionContinue = nullptr;
	// the continue address for continuing execution after camera write interception. 
	LPBYTE _cameraWrite2InterceptionContinue = nullptr;
	// the continue address for continuing execution after camera write interception. 
	LPBYTE _cameraWrite3InterceptionContinue = nullptr;
	// the continue address for continuing execution after camera write interception. 
	LPBYTE _cameraWrite4InterceptionContinue = nullptr;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*> &aobBlocks)
	{
		aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_ADDRESS_INTERCEPT_KEY, "FF 90 ?? ?? ?? ?? | 8B 08 89 0B 8B 48 04 89 4B 04 8B 40 08 89 43 08 48 8B 5C 24 40 48 83 C4 30", 2);	// same as write 2, but 2nd occurrence
		aobBlocks[CAMERA_CINEMATICS_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_CINEMATICS_ADDRESS_INTERCEPT_KEY, "0F 29 B8 ?? ?? ?? ?? 8B 06 48 8D B9 ?? ?? ?? ?? 0F 28 FB 89 44 24 ", 1);
		aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE1_INTERCEPT_KEY, "F3 41 0F 10 47 10 F3 0F 58 00 F3 41 0F 11 47 10 F3 0F 10 48 04 F3 41 0F 58 4F 14 F3 41 0F 11 4F 14 F3 0F 10 40 08 F3 41 0F 58 47 18 F3 41 0F 11 47 18 F7 86 98 05 00 00", 1);
		aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE2_INTERCEPT_KEY, "FF 90 ?? ?? ?? ?? | 8B 08 89 0B 8B 48 04 89 4B 04 8B 40 08 89 43 08 48 8B 5C 24 40 48 83 C4 30", 1);
		aobBlocks[CAMERA_WRITE3_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE3_INTERCEPT_KEY, "8B 08 89 0B 8B 48 04 89 4B 04 8B 48 08 89 4B 08 8B 08 41 89 0C 24", 1);
		aobBlocks[CAMERA_WRITE4_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE4_INTERCEPT_KEY, "8B 45 90 01 03 8B 45 A8 01 43 04 8B 44 24 60 01 43 08 48 8D 1D", 1);

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
		GameImageHooker::setHook(aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY], 0x10, &_cameraStructInterceptionContinue, &cameraStructInterceptor);
	}


	void setPostCameraStructHooks(map<string, AOBBlock*> &aobBlocks)
	{
		GameImageHooker::setHook(aobBlocks[CAMERA_CINEMATICS_ADDRESS_INTERCEPT_KEY], 0x10, &_cameraCinematicsStructInterceptionContinue, &cameraCinematicsStructInterceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY], 0x32, &_cameraWrite1InterceptionContinue, &cameraWrite1Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY], 0x15, &_cameraWrite2InterceptionContinue, &cameraWrite2Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE3_INTERCEPT_KEY], 0x10, &_cameraWrite3InterceptionContinue, &cameraWrite3Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE4_INTERCEPT_KEY], 0x12, &_cameraWrite4InterceptionContinue, &cameraWrite4Interceptor);
	}
}
