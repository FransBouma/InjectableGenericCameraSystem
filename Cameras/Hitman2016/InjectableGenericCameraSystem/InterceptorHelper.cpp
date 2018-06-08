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
	void cameraWriteInterceptor1();		// create as much interceptors for write interception as needed. In the example game, there are 4.
	void cameraWriteInterceptor2();
	void cameraWriteInterceptor3();
	void cameraReadInterceptor1();
	void gamespeedAddressInterceptor();
	void fovWriteInterceptor();
}

// external addresses used in asm.
extern "C" {
	LPBYTE _cameraStructInterceptionContinue = nullptr;
	LPBYTE _cameraWriteInterceptionContinue1 = nullptr;
	LPBYTE _cameraWriteInterceptionContinue2 = nullptr;
	LPBYTE _cameraWriteInterceptionContinue3 = nullptr;
	LPBYTE _gamespeedInterceptionContinue = nullptr;
	LPBYTE _cameraReadInterceptionContinue1 = nullptr;
	LPBYTE _fovWriteInterceptionContinue = nullptr;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*> &aobBlocks)
	{
		aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_ADDRESS_INTERCEPT_KEY, "F3 0F 11 83 90 00 00 00 F3 0F 10 44 24 58 F3 0F 11 83 98 00 00 00 F3 0F 11 8B 94 00 00 00", 3);
		aobBlocks[CAMERA_WRITE_INTERCEPT1_KEY] = new AOBBlock(CAMERA_WRITE_INTERCEPT1_KEY, "0F 28 00 0F 11 83 80 00 00 00 F3 0F 10 44 24 50 F3 0F 11 83 90 00 00 00", 3);
		aobBlocks[CAMERA_WRITE_INTERCEPT2_KEY] = new AOBBlock(CAMERA_WRITE_INTERCEPT2_KEY, "F3 0F 11 83 98 00 00 00 F3 0F 11 8B 94 00 00 00 EB", 3);
		aobBlocks[CAMERA_WRITE_INTERCEPT3_KEY] = new AOBBlock(CAMERA_WRITE_INTERCEPT3_KEY, "0F 11 83 80 00 00 00 0F 28 4F 30 0F 28 C1 F3 0F 11 8B 90 00 00 00 0F C6 C1 55 0F C6 C9 AA", 3);
		aobBlocks[CAMERA_READ_INTERCEPT_KEY] = new AOBBlock(CAMERA_READ_INTERCEPT_KEY, "40 53 48 81 EC ?? 00 00 00 F6 81 ?? 00 00 00 ??", 1);
		aobBlocks[GAMESPEED_ADDRESS_INTERCEPT_KEY] = new AOBBlock(GAMESPEED_ADDRESS_INTERCEPT_KEY, "48 89 43 28 48 8B 4B 18 48 89 4B 20 48 01 43 18 EB", 1);
		aobBlocks[FOV_WRITE_INTERCEPT_KEY] = new AOBBlock(FOV_WRITE_INTERCEPT_KEY, "F3 0F 11 83 00 07 00 00 48 8B CB 48 83 C4 20", 1);
		aobBlocks[SUPERSAMPLING_ADDRESS_KEY] = new AOBBlock(SUPERSAMPLING_ADDRESS_KEY, "8B 0D ?? ?? ?? ?? F3 0F 10 05 | ?? ?? ?? ?? 85 C9", 1);

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
		GameImageHooker::setHook(aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY], 0xE, &_cameraStructInterceptionContinue, &cameraAddressInterceptor);
	}
	

	void setCameraWriteInterceptorHooks(map<string, AOBBlock*> &aobBlocks)
	{
		// for each block of code that writes to the camera values we're manipulating we need an interception to block it. For the example game there are 3. 
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE_INTERCEPT1_KEY], 0x10, &_cameraWriteInterceptionContinue1, &cameraWriteInterceptor1);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE_INTERCEPT2_KEY], 0x10, &_cameraWriteInterceptionContinue2, &cameraWriteInterceptor2);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE_INTERCEPT3_KEY], 0x2E, &_cameraWriteInterceptionContinue3, &cameraWriteInterceptor3);
		GameImageHooker::setHook(aobBlocks[CAMERA_READ_INTERCEPT_KEY], 0x10, &_cameraReadInterceptionContinue1, &cameraReadInterceptor1);
		GameImageHooker::setHook(aobBlocks[FOV_WRITE_INTERCEPT_KEY], 0xF, &_fovWriteInterceptionContinue, &fovWriteInterceptor);
		CameraManipulator::setResolutionScaleMenuValueAddress(Utils::calculateAbsoluteAddress(aobBlocks[SUPERSAMPLING_ADDRESS_KEY], 4));	//hitman.exe+E609E3 - F3 0F10 05 9D6C0C02   - movss xmm0,[hitman.exe+2F27688] << supersampling read here
	}


	void setTimestopInterceptorHook(map<string, AOBBlock*> &aobBlocks)
	{
		GameImageHooker::setHook(aobBlocks[GAMESPEED_ADDRESS_INTERCEPT_KEY], 0x10, &_gamespeedInterceptionContinue, &gamespeedAddressInterceptor);
	}
}
