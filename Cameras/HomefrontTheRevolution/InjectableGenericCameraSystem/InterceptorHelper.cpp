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
	void timestopInterceptor();
	void fovWriteInterceptor();
}

// external addresses used in asm.
extern "C" {
	LPBYTE _cameraStructInterceptionContinue = nullptr;
	LPBYTE _cameraWrite1InterceptionContinue = nullptr;
	LPBYTE _cameraWrite2InterceptionContinue = nullptr;
	LPBYTE _cameraWrite3InterceptionContinue = nullptr;
	LPBYTE _timestopInterceptionContinue = nullptr;
	LPBYTE _fovWriteInterceptionContinue = nullptr;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*> &aobBlocks)
	{
		aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_ADDRESS_INTERCEPT_KEY, "F3 0F 10 4B 0C F3 0F 10 43 1C F3 0F 11 4D 24 F3 0F 10 4B 2C F3 0F 11 45 28 8B 45 24 89 87 2C 01 00 00", 1);	
		aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE1_INTERCEPT_KEY, "F3 0F 11 43 18 F3 0F 10 44 24 40 F3 0F 11 53 08 F3 0F 10 54 24 38 F3 0F 5C E7 F3 41 0F 58 F0 F3 0F 11 53 1C F3 0F 11 2B F3 0F 11 5B 04", 1);
		aobBlocks[CAMERA_WRITE3_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE3_INTERCEPT_KEY, "F3 0F 11 4C 24 3C F3 0F 58 C3 F3 0F 11 5C 24 20 F3 44 0F11 5B 0C F3 0F11 44 24 40", 1);
		aobBlocks[TIMESTOP_INTERCEPT_KEY] = new AOBBlock(TIMESTOP_INTERCEPT_KEY, "66 0F EF C0 F2 48 0F 2A 47 40 F2 0F 5E D0 66 0F EF C0 F2 48 0F 2A C1", 1);
		
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
		GameImageHooker::setHook(aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY], 0x14, &_cameraStructInterceptionContinue, &cameraStructInterceptor);
	}


	void setPostCameraStructHooks(map<string, AOBBlock*> &aobBlocks, LPBYTE hostImageAddress)
	{
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY], 0x4D, &_cameraWrite1InterceptionContinue, &cameraWrite1Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE3_INTERCEPT_KEY], 0x16, &_cameraWrite3InterceptionContinue, &cameraWrite3Interceptor);
		GameImageHooker::setHook(aobBlocks[TIMESTOP_INTERCEPT_KEY], 0x17, &_timestopInterceptionContinue, &timestopInterceptor);
		// for Write2 we'll use the good old hardcoded offsets, as it's a generic routine which is in the code many times so AOB scanning that is undoable, sadly. 
		GameImageHooker::setHook(hostImageAddress, CAMERA_WRITE_INTERCEPT2_START_OFFSET, CAMERA_WRITE_INTERCEPT2_BLOCK_LENGTH, &_cameraWrite2InterceptionContinue, &cameraWrite2Interceptor);
		GameImageHooker::setHook(hostImageAddress, FOV_WRITE1_INTERCEPT_START_OFFSET, FOV_WRITE1_INTERCEPT_BLOCK_LENGTH, &_fovWriteInterceptionContinue, &fovWriteInterceptor);
	}

	void disableFoVWrites(LPBYTE hostImageAddress)
	{
		// Dirty hack, but it's not possible to intercept the write as it's surrounded by RIP relative reads.
		// simply NOP the write opcodes
		// Write 1
		// Homefront2_Release.exe + 948ED2 - F3 0F59 05 4A5F6601 - mulss xmm0, [Homefront2_Release.exe + 1FAEE24]
		// Homefront2_Release.exe + 948EDA - F3 0F11 42 58		 - movss[rdx + 58], xmm0								<< WRITE FOV
		// Homefront2_Release.exe + 948EDF - E9 948BFFFF		 - jmp Homefront2_Release.exe + 941A78
		// Homefront2_Release.exe + 948EE4 - 48 8B C4			 - mov rax, rsp
		// 
		// Write 2
		// Homefront2_Release.exe + 93F341 - F3 0F59 05 DBFA6601 - mulss xmm0, [Homefront2_Release.exe + 1FAEE24]
		// Homefront2_Release.exe + 93F349 - F3 0F59 82 EC0A0000 - mulss xmm0, [rdx + 00000AEC]
		// Homefront2_Release.exe + 93F351 - F3 41 0F11 40 58	 - movss[r8 + 58], xmm0									<< WRITE FOV
		// Homefront2_Release.exe + 93F357 - 48 8B 02			 - mov rax, [rdx]
		// Homefront2_Release.exe + 93F35A - FF 90 28020000		 - call qword ptr[rax + 00000228]
		GameImageHooker::nopRange(hostImageAddress + FOV_WRITE2_RANGE_START_OFFSET, 5);
		GameImageHooker::nopRange(hostImageAddress + FOV_WRITE3_RANGE_START_OFFSET, 6);
	}

	void enableFoVWrites(LPBYTE hostImageAddress)
	{
		// Dirty hack, but it's not possible to intercept the write as it's surrounded by RIP relative reads.
		// simply restore the write opcodes. See disableFoVWrites for details, above.
		// first instruction:  Homefront2_Release.exe + 948EDA - F3 0F11 42 58		 - movss[rdx + 58], xmm0								<< WRITE FOV
		byte instruction1[5] = { 0xF3, 0x0F, 0x11, 0x42, 0x58 };
		GameImageHooker::writeRange(hostImageAddress + FOV_WRITE2_RANGE_START_OFFSET, &instruction1[0], 5);

		// second instruction: Homefront2_Release.exe + 93F351 - F3 41 0F11 40 58	 - movss[r8 + 58], xmm0									<< WRITE FOV
		byte instruction2[6] = { 0xF3, 0x41, 0x0F, 0x11, 0x40, 0x58 };
		GameImageHooker::writeRange(hostImageAddress + FOV_WRITE3_RANGE_START_OFFSET, &instruction2[0], 6);
	}
}
