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
#include "OverlayConsole.h"
#include "CameraManipulator.h"

using namespace std;

//--------------------------------------------------------------------------------------------------------------------------------
// external asm functions
extern "C" {
	void cameraStructInterceptor();
	void cameraWrite1Interceptor();
	void cameraWrite2Interceptor();
}

// external addresses used in asm.
extern "C" {
	LPBYTE _cameraStructInterceptionContinue = nullptr;
	LPBYTE _cameraWrite1InterceptionContinue = nullptr;
	LPBYTE _cameraWrite2InterceptionContinue = nullptr;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*> &aobBlocks)
	{
		aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_ADDRESS_INTERCEPT_KEY, "F3 0F 10 0A F3 41 0F 10 00 0F 5A C0 0F 5A C9 F2 0F 58 C8 F2 0F 11 89 88 00 00 00 F3 0F 10 52 04", 1);	
		aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE1_INTERCEPT_KEY, "F3 0F 10 02 0F 5A C0 F2 0F 11 81 88 00 00 00 F3 0F 10 4A 04 0F 5A C9 F2 0F 11 89 90 00 00 00", 1);
		aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE1_INTERCEPT_KEY, "F2 0F 11 86 88 00 00 00 0F 11 8E 90 00 00 00 0F 10 4C 24 50 0F 11 8E A0 00 00 00 F3 0F10 4C 24 60", 1);
		aobBlocks[FOV_WRITE_INTERCEPT_KEY] = new AOBBlock(FOV_WRITE_INTERCEPT_KEY, "E8 ?? ?? ?? ?? F3 0F 59 05 ?? ?? ?? ?? F3 0F 58 C0 | F3 0F 11 43 2C F3 0F 10 05", 1);
		aobBlocks[TIMESTOP_READ_INTERCEPT_KEY] = new AOBBlock(TIMESTOP_READ_INTERCEPT_KEY, "83 3D | ?? ?? ?? ?? 00 75 0A C7 81 30 01 00 00 00 00 00 00 8B 92 38 02 00 00 03 91 30 01 00", 1);

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
		GameImageHooker::setHook(aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY], 0x0F, &_cameraStructInterceptionContinue, &cameraStructInterceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY], 0x0F, &_cameraWrite1InterceptionContinue, &cameraWrite1Interceptor);
	}


	void setPostCameraStructHooks(map<string, AOBBlock*> &aobBlocks, LPBYTE hostImageAddress)
	{
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY], 0x29, &_cameraWrite2InterceptionContinue, &cameraWrite2Interceptor);
		CameraManipulator::setTimestopValueAddress(Utils::calculateAbsoluteAddress(aobBlocks[TIMESTOP_READ_INTERCEPT_KEY], 5)); // TEW2.exe+444830 - 83 3D 41B25702 00		- cmp dword ptr [TEW2.exe+29BFA78],00
	}

	// if 'enabled' is false, we'll nop the range where the FoV writes are placed, otherwise we'll write back the original statement bytes. 
	void toggleFoVWriteState(map<string, AOBBlock*> &aobBlocks, bool enabled)
	{
		//  TEW2.exe + 6B4101 - E8 66055800				- call TEW2.exe + C3466C
		//	TEW2.exe + 6B4106 - F3 0F59 05 EA55F500		- mulss xmm0, [TEW2.exe + 16096F8]{ [57.30] }
		//	TEW2.exe + 6B410E - F3 0F58 C0				- addss xmm0, xmm0
		//	TEW2.exe + 6B4112 - F3 0F11 43 2C			- movss[rbx + 2C], xmm0 << FOV WRITE
		//	TEW2.exe + 6B4117 - F3 0F10 05 E5F53002		- movss xmm0, [TEW2.exe + 29C3704]{ [1.78] }
		//	TEW2.exe + 6B411F - 0F2E C6					- ucomiss xmm0, xmm6
		//	TEW2.exe + 6B4122 - 7A 02					- jp TEW2.exe + 6B4126

		if (enabled)
		{
			// enable writes
			byte originalStatementBytes1[5] = { 0xF3, 0x0F, 0x11, 0x43, 0x2C };      // TEW2.exe + 6B4112 - F3 0F11 43 2C - movss[rbx + 2C], xmm0
			GameImageHooker::writeRange(aobBlocks[FOV_WRITE_INTERCEPT_KEY], originalStatementBytes1, 5);
		}
		else
		{
			// disable writes, by nopping the range.
			GameImageHooker::nopRange(aobBlocks[FOV_WRITE_INTERCEPT_KEY], 5);
		}
	}
}
