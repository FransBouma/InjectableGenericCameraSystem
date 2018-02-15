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
#include <map>
#include "InterceptorHelper.h"
#include "GameConstants.h"
#include "GameImageHooker.h"
#include "Utils.h"
#include "AOBBlock.h"
#include "Console.h"
#include "CameraManipulator.h"

using namespace std;

//--------------------------------------------------------------------------------------------------------------------------------
// external asm functions
extern "C" {
	void cameraAddressInterceptor();
	void fovAddressInterceptor();
}

// external addresses used in asm.
extern "C" {
	LPBYTE _cameraStructInterceptionContinue = nullptr;
	LPBYTE _fovAddressInterceptionContinue = nullptr;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*> &aobBlocks)
	{
		aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_ADDRESS_INTERCEPT_KEY, "F3 0F 11 87 ?? ?? ?? ?? F3 0F 10 45 ?? F3 0F 11 87 ?? ?? ?? ?? 0F 10 45", 1);
		aobBlocks[FOV_ADDRESS_INTERCEPT_KEY] = new AOBBlock(FOV_ADDRESS_INTERCEPT_KEY, "48 8B 06 48 8D 8B ?? ?? ?? ?? 48 8D 55 67 48 89 45", 1);
		aobBlocks[SHOWHUD_CVAR_ADDRESS_INTERCEPT_KEY] = new AOBBlock(SHOWHUD_CVAR_ADDRESS_INTERCEPT_KEY, "48 8D 54 24 ?? 48 89 44 24 ?? 48 8B CB E8 ?? ?? ?? ?? 83 3D | ?? ?? ?? ?? 00", 1);
		aobBlocks[STOPTIME_CVAR_ADDRESS_INTERCEPT_KEY] = new AOBBlock(STOPTIME_CVAR_ADDRESS_INTERCEPT_KEY, "48 89 6C 24 ?? 48 89 74 24 ?? 48 89 7C 24 ?? 48 8B D9 83 3D | ?? ?? ?? ?? 00 74", 1);
#error THIS IS FREEZETIME, WE NEED TO INTERCEPT RUNFRAMES
		aobBlocks[RUNFRAMES_CVAR_ADDRESS_INTERCEPT_KEY] = new AOBBlock(RUNFRAMES_CVAR_ADDRESS_INTERCEPT_KEY, "48 8B 06 48 8B CE FF 90 ?? ?? ?? ?? 81 25 ?? ?? ?? ?? FFFFFBFF 83 3D | ?? ?? ?? ?? 00", 1);

		map<string, AOBBlock*>::iterator it;
		bool result = true;
		for(it = aobBlocks.begin(); it!=aobBlocks.end();it++)
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
		GameImageHooker::setHook(aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY], 0x40, &_cameraStructInterceptionContinue, &cameraAddressInterceptor);
	}
	

	void setPostCameraStructHooks(map<string, AOBBlock*> &aobBlocks)
	{
		GameImageHooker::setHook(aobBlocks[FOV_ADDRESS_INTERCEPT_KEY], 0x12, &_fovAddressInterceptionContinue, &fovAddressInterceptor);
		CameraManipulator::setShowHudAddress(Utils::calculateAbsoluteAddress(aobBlocks[SHOWHUD_CVAR_ADDRESS_INTERCEPT_KEY], 5));		//NewColossus_x64vk.exe+F9C173 - 83 3D 669FFF01 00  - cmp dword ptr [NewColossus_x64vk.exe+2F960E0],00 << g_showhud read here.
		CameraManipulator::setStopTimeAddress(Utils::calculateAbsoluteAddress(aobBlocks[STOPTIME_CVAR_ADDRESS_INTERCEPT_KEY], 5));		//NewColossus_x64vk.exe+A8F886 - 83 3D 839E4A02 00  - cmp dword ptr [NewColossus_x64vk.exe+2F39710],00 << g_stopTime read here.
		CameraManipulator::setRunFramesAddress(Utils::calculateAbsoluteAddress(aobBlocks[RUNFRAMES_CVAR_ADDRESS_INTERCEPT_KEY], 5));	//NewColossus_x64vk.exe+11B4048 - 83 3D E12AE101 00 - cmp dword ptr [NewColossus_x64vk.exe+2FC6B30],00 << g_runFrames read here.
	}
}
