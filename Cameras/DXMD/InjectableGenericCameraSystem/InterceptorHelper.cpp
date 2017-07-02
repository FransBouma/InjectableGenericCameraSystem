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
	void cameraAddressInterceptor();
	void gameSpeedInterceptor();
	void cameraWriteInterceptor();
	void hudToggleInterceptor();
}

// external addresses used in asm.
extern "C" {
	// The continue address for continuing execution after camera values address interception. 
	LPBYTE _cameraStructInterceptionContinue = nullptr;
	// the continue address for continuing execution after camera write interception. 
	LPBYTE _cameraWriteInterceptionContinue = nullptr;
	// the continue addresss for the continuing execution after interception of the game speed block of code.
	LPBYTE _gameSpeedInterceptorContinue = nullptr;
	// the continue addresss for the continuing execution after interception of the hud toggle block of code
	LPBYTE _hudToggleInterceptorContinue = nullptr;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*> &aobBlocks)
	{
		aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_ADDRESS_INTERCEPT_KEY, "F3 0F 10 9B 3C 01 00 00 F3 0F 10 8B 08 02 00 00 F3 0F 10 93 0C 02 00 00 0F 28 E3", 1);
		aobBlocks[CAMERA_WRITE_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE_INTERCEPT_KEY, "0F C6 C2 49 0F 29 4B 50 0F 28 CA 0F C6 CD 0A 0F 29 43 60 0F C6 CD 98 0F 29 4B 70 C6 43 18 00", 1);
		aobBlocks[GAMESPEED_ADDRESS_INTERCEPT_KEY] = new AOBBlock(GAMESPEED_ADDRESS_INTERCEPT_KEY, "F3 48 0F 2C C8 48 0F AF C8 48 C1 F9 14 49 89 48 30 49 01 48 20", 1);
		aobBlocks[HUD_TOGGLE_ADDRESS_INTERCEPT_KEY] = new AOBBlock(HUD_TOGGLE_ADDRESS_INTERCEPT_KEY, "80 79 20 00 74 ?? F3 0F 10 41", 1);
		aobBlocks[FOV_WRITE_ADDRESS_INTERCEPT_KEY] = new AOBBlock(FOV_WRITE_ADDRESS_INTERCEPT_KEY, "F3 0F 11 49 0C F3 0F 10 0D ?? ?? ?? ?? 48 81 C1 ?? ?? ?? ?? F3 0F 59 81 3C 01 00 00 0F 2F C1", 1);

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
		GameImageHooker::setHook(aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY], 0x18, &_cameraStructInterceptionContinue, &cameraAddressInterceptor);
	}

	void setCameraWriteInterceptorHook(map<string, AOBBlock*> &aobBlocks)
	{
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE_INTERCEPT_KEY], 0x1B, &_cameraWriteInterceptionContinue, &cameraWriteInterceptor);
	}

	void setTimestopInterceptorHook(map<string, AOBBlock*> &aobBlocks)
	{
		GameImageHooker::setHook(aobBlocks[GAMESPEED_ADDRESS_INTERCEPT_KEY], 0x11, &_gameSpeedInterceptorContinue, &gameSpeedInterceptor);
	}

	void setHudToggleInterceptorHook(map<string, AOBBlock*> &aobBlocks)
	{
		GameImageHooker::setHook(aobBlocks[HUD_TOGGLE_ADDRESS_INTERCEPT_KEY], 0x11, &_hudToggleInterceptorContinue, &hudToggleInterceptor);
	}

	void toggleFoVWriteState(map<string, AOBBlock*> &aobBlocks, bool enabled)
	{
		// if enabled, we'll nop the range where the FoV write is placed, otherwise we'll write back the original statement bytes. 
		if (enabled)
		{
			// enable writes
			byte originalStatementBytes[5] = { 0xF3, 0x0F, 0x11, 0x49, 0x0C };      // DXMD.exe+383F18E - F3 0F11 49 0C - movss [rcx+0C],xmm1
			GameImageHooker::writeRange(aobBlocks[FOV_WRITE_ADDRESS_INTERCEPT_KEY], originalStatementBytes, 5);
		}
		else
		{
			// disable writes, by nopping the range.
			GameImageHooker::nopRange(aobBlocks[FOV_WRITE_ADDRESS_INTERCEPT_KEY], 5);
		}
	}

}
