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
	void cameraWriteInterceptor();
	void gamespeedStructInterceptor();
	void todStructInterceptor();
	void hotsampleInterceptor();
}

// external addresses used in asm.
extern "C" {
	LPBYTE _cameraStructInterceptionContinue = nullptr;
	// the continue address for continuing execution after camera write interception. 
	LPBYTE _cameraWriteInterceptionContinue = nullptr;
	// the continue address for continuing execution after gamespeed address interception. 
	LPBYTE _gamespeedStructInterceptorContinue = nullptr;
	// the continue address for continuing execution after tod address interception. 
	LPBYTE _todStructInterceptorContinue = nullptr;
	// the continue address for continuing execution after hotsample address interception.
	LPBYTE _hotsampleInterceptorContinue = nullptr; 
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*> &aobBlocks)
	{
		aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_ADDRESS_INTERCEPT_KEY, "48 8B 44 24 20 48 89 43 70 8B 44 24 28 89 43 78 F3 0F 11 73 7C 8B 83", 1);
		aobBlocks[CAMERA_WRITE_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE_INTERCEPT_KEY, "41 89 00 8B 42 04 41 89 40 04 8B 42 08 41 89 40 08 48 8B 0D", 1);
		aobBlocks[GAMESPEED_ADDRESS_INTERCEPT_KEY] = new AOBBlock(GAMESPEED_ADDRESS_INTERCEPT_KEY, "F2 0F 10 93 80 00 00 00 66 0F 2F CA 72 03 0F 28 CA", 1);
		aobBlocks[TOD_ADDRESS_INTERCEPT_KEY] = new AOBBlock(TOD_ADDRESS_INTERCEPT_KEY, "48 8B 08 48 8D 54 24 50 48 89 4C 24 20 48 8D 4C 24 20", 1);
		aobBlocks[HOTSAMPLE_CODE_INTERCEPT_KEY] = new AOBBlock(HOTSAMPLE_CODE_INTERCEPT_KEY, "44 8B B3 ?? ?? ?? ?? 8B AB", 1);

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
		GameImageHooker::setHook(aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY], 0x15, &_cameraStructInterceptionContinue, &cameraStructInterceptor);
	}


	void setPostCameraStructHooks(map<string, AOBBlock*> &aobBlocks)
	{
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE_INTERCEPT_KEY], 0x11, &_cameraWriteInterceptionContinue, &cameraWriteInterceptor);
		GameImageHooker::setHook(aobBlocks[GAMESPEED_ADDRESS_INTERCEPT_KEY], 0x11, &_gamespeedStructInterceptorContinue, &gamespeedStructInterceptor);
		GameImageHooker::setHook(aobBlocks[TOD_ADDRESS_INTERCEPT_KEY], 0x12, &_todStructInterceptorContinue, &todStructInterceptor);
		GameImageHooker::setHook(aobBlocks[HOTSAMPLE_CODE_INTERCEPT_KEY], 0x11, &_hotsampleInterceptorContinue, &hotsampleInterceptor);
	}
}
