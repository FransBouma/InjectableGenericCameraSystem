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
	void fovWriteInterceptor1();
	void fovWriteInterceptor2();
	void hudToggleInterceptor();
	void camera2WriteInterceptor1();
	void camera2WriteInterceptor2();
}

// external addresses used in asm.
extern "C" {
	LPBYTE _cameraStructInterceptionContinue = nullptr;
	// the continue address for continuing execution after camera write interception. 
	LPBYTE _cameraWriteInterceptionContinue = nullptr;
	// the continue address for continuing execution after fov write interception.
	LPBYTE _fovWriteInterception1Continue = nullptr;
	// the continue address for continuing execution after fov write interception.
	LPBYTE _fovWriteInterception2Continue = nullptr;
	// the continue address for continuing execution after hud toggle interception.
	LPBYTE _hudToggleInterceptionContinue = nullptr;
	// the address of the core camera coords in the image. We'll write to this struct too, so the core camera coords are up to date with the view matrix as they're
	// used by other sub systems. 
	LPBYTE _camera2CoordsAddress = nullptr;
	// the address of the quaternion in the image. We'll write to this struct too, so the core camera coords are up to date with the view matrix as they're
	// used by other sub systems. 
	LPBYTE _camera2QuaternionAddress = nullptr;
	// the continue address for continuing execution after camera write interception. 
	LPBYTE _camera2WriteInterception1Continue = nullptr;
	// the continue address for continuing execution after camera write interception. 
	LPBYTE _camera2WriteInterception2Continue = nullptr;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*> &aobBlocks)
	{
		aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_ADDRESS_INTERCEPT_KEY, "F3 0F 10 48 4C F3 0F 10 40 48 F3 0F 11 4C 24 34 F3 0F 10 49 18", 1);
		aobBlocks[CAMERA_WRITE_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE_INTERCEPT_KEY, "89 82 BC 00 00 00 8B 41 38 89 82 B0 00 00 00 8B 41 3C 89 82 B4 00 00 00", 1);
		aobBlocks[FOV_WRITE_INTERCEPT1_KEY] = new AOBBlock(FOV_WRITE_INTERCEPT1_KEY, "F3 0F 11 40 18 C6 80 4C 01 00 00 01 66 C7 80 4E 01 00 00 01 01 C6 80 50 01 00 00 01 48 8B 49 58", 1);
		aobBlocks[FOV_WRITE_INTERCEPT2_KEY] = new AOBBlock(FOV_WRITE_INTERCEPT2_KEY, "F3 0F 11 40 18 C6 80 4C 01 00 00 01 66 C7 80 4E 01 00 00 01 01 C6 80 50 01 00 00 01 F3 C3", 1);
		aobBlocks[HUD_TOGGLE_INTERCEPT_KEY] = new AOBBlock(HUD_TOGGLE_INTERCEPT_KEY, "F3 0F 10 43 50 48 83 C4 20 5B C3", 1);
		aobBlocks[CAMERA2_WRITE_INTERCEPT1_KEY] = new AOBBlock(CAMERA2_WRITE_INTERCEPT1_KEY, "F3 0F 11 49 10 8B 42 04 89 41 14 8B 42 08 B2 15 89 41 18", 1);
		aobBlocks[CAMERA2_WRITE_INTERCEPT2_KEY] = new AOBBlock(CAMERA2_WRITE_INTERCEPT2_KEY, "41 89 40 0C 8B 02 41 89 00 8B 42 04 41 89 40 04 8B 42 08 B2 15 41 89 40 08 ", 1);

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
	

	void initializeCamera2Addresses(LPBYTE camera2StructAddress)
	{
		// sub 0x10 as that's the offset used in the intercepted asm block so the address compared with the register is 0x10 lower than the actual coords address
		_camera2CoordsAddress = camera2StructAddress + CAMERA2_COORDS_IN_CAMERA_STRUCT_OFFSET - 0x10;
		_camera2QuaternionAddress = camera2StructAddress + LOOK_DATA_IN_CAMERA2_STRUCT_OFFSET;
	}


	void setCameraStructInterceptorHook(map<string, AOBBlock*> &aobBlocks)
	{
		GameImageHooker::setHook(aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY], 0x1D, &_cameraStructInterceptionContinue, &cameraStructInterceptor);
	}


	void setPostCameraStructHooks(map<string, AOBBlock*> &aobBlocks)
	{
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE_INTERCEPT_KEY], 0x3C, &_cameraWriteInterceptionContinue, &cameraWriteInterceptor);
		GameImageHooker::setHook(aobBlocks[FOV_WRITE_INTERCEPT1_KEY], 0x1C, &_fovWriteInterception1Continue, &fovWriteInterceptor1);
		GameImageHooker::setHook(aobBlocks[FOV_WRITE_INTERCEPT2_KEY], 0x1C, &_fovWriteInterception2Continue, &fovWriteInterceptor2);
		GameImageHooker::setHook(aobBlocks[HUD_TOGGLE_INTERCEPT_KEY], 0x12, &_hudToggleInterceptionContinue, &hudToggleInterceptor);		// length not important, we'll return, not jump back.
		GameImageHooker::setHook(aobBlocks[CAMERA2_WRITE_INTERCEPT1_KEY], 0x13, &_camera2WriteInterception1Continue, &camera2WriteInterceptor1);
		GameImageHooker::setHook(aobBlocks[CAMERA2_WRITE_INTERCEPT2_KEY], 0x19, &_camera2WriteInterception2Continue, &camera2WriteInterceptor2);
	}
}
