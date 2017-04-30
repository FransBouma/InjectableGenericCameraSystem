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
	void fovWriteInterceptor();
	void crossHairRenderInterceptor();
	void widgetRenderInterceptor();
}

// external addresses used in asm.
extern "C" {
	LPBYTE _cameraStructInterceptionContinue = nullptr;
	LPBYTE _fovWriteInterceptionContinue = nullptr;
	LPBYTE _crossHairRenderInterceptionContinue = nullptr;
	LPBYTE _widgetRenderInterceptionContinue = nullptr;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*> &aobBlocks)
	{
		aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_ADDRESS_INTERCEPT_KEY, "8B 81 1C 04 00 00 89 02 8B 81 20 04 00 00 89 42 04 8B 81 24 04 00 00 89 42 08 8B 81 28 04 00 00 41 89 00", 1);	
		aobBlocks[FOV_INTERCEPT_KEY] = new AOBBlock(FOV_INTERCEPT_KEY, "4C 8B 44 24 50 F3 0F10 4C 24 48 48 89 F1 E8 ?? ?? ?? ?? | 48 8B 5C 24 40 F3 0F 11 07 48 83 C4 20 5F 5E 5D C3", 1);
		aobBlocks[CROSSHAIR_INTERCEPT_KEY] = new AOBBlock(CROSSHAIR_INTERCEPT_KEY, "53 48 83 EC 20 48 89 CB 41 B9 01000000 48 8D 15 ?? ?? ?? ??", 1);
		aobBlocks[WIDGET_RENDER_INTERCEPT_KEY] = new AOBBlock(WIDGET_RENDER_INTERCEPT_KEY, "48 8B C4 41 54 41 56 41 57 48 83 EC 30 48 C7 40 E0 FE FF FF FF 48 89 58 08 48 89 68 10", 1);

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
		GameImageHooker::setHook(aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY], 0x37, &_cameraStructInterceptionContinue, &cameraStructInterceptor);
	}


	void setPostCameraStructHooks(map<string, AOBBlock*> &aobBlocks)
	{
		GameImageHooker::setHook(aobBlocks[FOV_INTERCEPT_KEY], 0x10, &_fovWriteInterceptionContinue, &fovWriteInterceptor);
		GameImageHooker::setHook(aobBlocks[CROSSHAIR_INTERCEPT_KEY], 0xE, &_crossHairRenderInterceptionContinue, &crossHairRenderInterceptor);
		GameImageHooker::setHook(aobBlocks[WIDGET_RENDER_INTERCEPT_KEY], 0x15, &_widgetRenderInterceptionContinue, &widgetRenderInterceptor);
	}
}
