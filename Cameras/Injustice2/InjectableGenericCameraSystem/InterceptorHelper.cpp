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
	void timeDilationInterceptor();
	void dofEnableInterceptor();
}

// external addresses used in asm.
extern "C" {
	LPBYTE _cameraStructInterceptionContinue = nullptr;
	LPBYTE _cameraWrite1InterceptionContinue = nullptr;
	LPBYTE _timeDilationInterceptionContinue = nullptr;
	LPBYTE _dofEnableInterceptionContinue = nullptr;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*> &aobBlocks)
	{
		aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_ADDRESS_INTERCEPT_KEY, "F2 0F 10 81 84 05 00 00 F2 0F 11 02 8B 81 8C 05 00 00", 1);	
		aobBlocks[AR_FIX_KEY] = new AOBBlock(AR_FIX_KEY, "F3 0F 5C C3 0F 5A C0 0F 54 05 ?? ?? ?? ?? 66 0F 5A C0 0F 5A C8 66 0F 2F 0D 00", 1);
		aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE1_INTERCEPT_KEY, "0F 11 87 84 05 00 00 89 8F 80 05 00 00 48 89 F9 F2 0F 11 8F 94 05 00 00", 1);
		aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE2_INTERCEPT_KEY, "E8 ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 8B 48 30 48 85 C9 74 11", 1);
		aobBlocks[TIME_DILATION_INTERCEPT_KEY] = new AOBBlock(TIME_DILATION_INTERCEPT_KEY, "F3 0F 11 B1 88 00 00 00 0F 28 74 24 30 48 83 C4 40", 1);
		aobBlocks[HUD_TOGGLE_ADDRESS_INTERCEPT_KEY] = new AOBBlock(HUD_TOGGLE_ADDRESS_INTERCEPT_KEY, "41 0F 28 00 48 89 D0 41 0F 28 48 10 0F 29 02 0F", 1);
		aobBlocks[DOF_ENABLE_ADDRESS_INTERCEPT_KEY] = new AOBBlock(DOF_ENABLE_ADDRESS_INTERCEPT_KEY, "88 43 68 41 8B 46 6C 89 43 6C 41 8B 46 70 89 43 70 41 8B 46 74 89 43 74", 1);

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
		if(aobBlocks[AR_FIX_KEY]->absoluteAddress() !=nullptr)
		{
			BYTE xmm0XorpsBytes[4] = { 0x0f, 0x57, 0xc0, 0x90 };
			GameImageHooker::writeRange(aobBlocks[AR_FIX_KEY], xmm0XorpsBytes, 4);
			Console::WriteLine("AR Fix enabled");
		}
	}


	void setCameraStructInterceptorHook(map<string, AOBBlock*> &aobBlocks)
	{
		GameImageHooker::setHook(aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY], 0x2C, &_cameraStructInterceptionContinue, &cameraStructInterceptor);
	}


	void setPostCameraStructHooks(map<string, AOBBlock*> &aobBlocks)
	{
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY], 0x1E, &_cameraWrite1InterceptionContinue, &cameraWrite1Interceptor);
		GameImageHooker::setHook(aobBlocks[TIME_DILATION_INTERCEPT_KEY], 0x11, &_timeDilationInterceptionContinue, &timeDilationInterceptor);
		GameImageHooker::setHook(aobBlocks[DOF_ENABLE_ADDRESS_INTERCEPT_KEY], 0x0E, &_dofEnableInterceptionContinue, &dofEnableInterceptor);
	}

	
	void toggleSuperMoveCamera(map<string, AOBBlock*>& aobBlocks, bool cameraEnabled)
	{
		if(cameraEnabled)
		{
			// camera is enabled, disable supermove camera writes
			GameImageHooker::nopRange(aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY], 5);	//0000000146C59AAE | E8 1DC25A01 | call injustice2_dump.148205CD0    << NOP if camera active
		}
		else
		{
			// camera is disabled, enable supermove camera writes
			BYTE callBytes[5] = { 0xE8, 0x1D, 0xC2, 0x5A, 0x01 };
			GameImageHooker::writeRange(aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY], callBytes, 5);
		}
	}

	
	void toggleHud(map<string, AOBBlock*>& aobBlocks, bool showHud)
	{
		static BYTE xmm0OriginalBytes[4] = { 0x41, 0x0f, 0x28, 0x00 };
		static BYTE xmm0XorpsBytes[4] = { 0x0f, 0x57, 0xc0, 0x90 };
		static BYTE xmm1OriginalBytes[5] = { 0x41, 0x0f, 0x28, 0x48, 0x10 };
		static BYTE xmm1XorpsBytes[5] = { 0x0f, 0x57, 0xc9, 0x90, 0x90 };
		if(showHud)
		{
			// emit the original code
			GameImageHooker::writeRange(aobBlocks[HUD_TOGGLE_ADDRESS_INTERCEPT_KEY], xmm0OriginalBytes, 4);
			GameImageHooker::writeRange(aobBlocks[HUD_TOGGLE_ADDRESS_INTERCEPT_KEY]->absoluteAddress() + 7, xmm1OriginalBytes, 5);
		}
		else
		{
			// write xorps statements
			GameImageHooker::writeRange(aobBlocks[HUD_TOGGLE_ADDRESS_INTERCEPT_KEY], xmm0XorpsBytes, 4);
			GameImageHooker::writeRange(aobBlocks[HUD_TOGGLE_ADDRESS_INTERCEPT_KEY]->absoluteAddress() + 7, xmm1XorpsBytes, 5);
		}
	}
}
