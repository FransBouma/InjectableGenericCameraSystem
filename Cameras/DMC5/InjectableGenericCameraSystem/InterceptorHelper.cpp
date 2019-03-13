////////////////////////////////////////////////////////////////////////////////////////////////////////
// Part of Injectable Generic Camera System
// Copyright(c) 2019, Frans Bouma
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
	void resolutionScaleReadInterceptor();
	void timestopReadInterceptor();
}

// external addresses used in asm.
extern "C" {
	LPBYTE _cameraStructInterceptionContinue = nullptr;
	LPBYTE _cameraWrite1InterceptionContinue = nullptr;
	LPBYTE _cameraWrite2InterceptionContinue = nullptr;
	LPBYTE _cameraWrite3InterceptionContinue = nullptr;
	LPBYTE _resolutionScaleReadInterceptionContinue = nullptr;
	LPBYTE _timestopReadInterceptionContinue = nullptr;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*> &aobBlocks)
	{
		aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_ADDRESS_INTERCEPT_KEY, "F3 0F 11 46 38 48 8B 43 50 48 83 78 18 00 75 ?? F3 0F", 1);	
		aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE1_INTERCEPT_KEY, "41 8B 06 89 47 40 41 8B 46 04 89 47 44 41 8B 46 08 89 47 48 41 8B 46 0C 89 47 4C 80 BF D2 00 00 00 00", 1);
		aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE2_INTERCEPT_KEY, "8B 08 89 4F 40 8B 48 04 89 4F 44 8B 48 08 89 4F 48 8B 40 0C 89 47 4C 80 BF D2 00 00 00 00", 1);
		aobBlocks[CAMERA_WRITE3_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE3_INTERCEPT_KEY, "EB ?? | 41 8B 06 89 47 30 41 8B 46 04 89 47 34 41 8B 46 08 89 47 38 80 BF D2 00 00 00 00", 1);
		aobBlocks[TIMESTOP_READ_INTERCEPT_KEY] = new AOBBlock(TIMESTOP_READ_INTERCEPT_KEY, "F3 0F 10 87 84 03 00 00 0F 2F C2 F3 0F 10 9F 80 03 00 00", 1);
		aobBlocks[RESOLUTION_SCALE_INTERCEPT_KEY] = new AOBBlock(RESOLUTION_SCALE_INTERCEPT_KEY, "F3 0F 10 80 6C 12 00 00 48 8D 44 24 3C F3 41 0F 59 46 40", 1);

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
		GameImageHooker::setHook(aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY], 0x0E, &_cameraStructInterceptionContinue, &cameraStructInterceptor);
	}


	void setPostCameraStructHooks(map<string, AOBBlock*> &aobBlocks)
	{
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY], 0x1B, &_cameraWrite1InterceptionContinue, &cameraWrite1Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY], 0x17, &_cameraWrite2InterceptionContinue, &cameraWrite2Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE3_INTERCEPT_KEY], 0x14, &_cameraWrite3InterceptionContinue, &cameraWrite3Interceptor);
		GameImageHooker::setHook(aobBlocks[TIMESTOP_READ_INTERCEPT_KEY], 0x13, &_timestopReadInterceptionContinue, &timestopReadInterceptor);
		GameImageHooker::setHook(aobBlocks[RESOLUTION_SCALE_INTERCEPT_KEY], 0x13, &_resolutionScaleReadInterceptionContinue, &resolutionScaleReadInterceptor);

		disableARLimits(aobBlocks);
	}


	void disableARLimits(map<string, AOBBlock*> &aobBlocks)
	{
		// NOP for now.
	}


	// if enabled is true, we'll place a 'ret' at the start of the code block, making the game skip rendering any hud element. If false, we'll reset
	// the original first statement so code will proceed as normal. 
	void toggleHudRenderState(map<string, AOBBlock*> &aobBlocks, bool enabled)
	{
		//if (enabled)
		//{
		//	// set ret
		//	BYTE statementBytes[1] = { 0xC3 };						// ACOdyssey.exe+C0DFCC0 - 48 8B C4              - mov rax,rsp
		//	GameImageHooker::writeRange(aobBlocks[HUD_RENDER_INTERCEPT_KEY], statementBytes, 1);
		//}
		//else
		//{
		//	// set original statement
		//	BYTE statementBytes[3] = { 0x48, 0x8B, 0xC4 };			// ACOdyssey.exe+C0DFCC0 - 48 8B C4              - mov rax,rsp
		//	GameImageHooker::writeRange(aobBlocks[HUD_RENDER_INTERCEPT_KEY], statementBytes, 3);
		//}
	}


	// Toggles the dof write. Enabled means the write is enabled (so the default game code is restored). Disabled means the 
	// writes are disabled and we can write what we want there. 
	void toggleDofEnableWrite(map<string, AOBBlock*> &aobBlocks, bool enabled)
	{
		// nop for now.
	}
}
