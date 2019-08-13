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
#include "CameraManipulator.h"
#include "Console.h"
#include "AOBBlock.h"

using namespace std;

//--------------------------------------------------------------------------------------------------------------------------------
// external asm functions
extern "C" {
	void cameraInitInterceptor();
}

// external addresses used in asm.
extern "C" {
	LPBYTE _cameraInitInterceptionContinue = nullptr;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	typedef void(__stdcall* AnselSessionStartStopFunction) ();

	static bool _anselSDKFound = false;
	static AnselSessionStartStopFunction _startSessionFunc = nullptr;
	static AnselSessionStartStopFunction _stopSessionFunc = nullptr;

	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*>& aobBlocks)
	{
		// AOBs in hostimage
		aobBlocks[ANSEL_START_SESSION_KEY] = new AOBBlock(ANSEL_START_SESSION_KEY, "40 53 48 83 EC 20 48 83 3D ?? ?? ?? ?? 00 48 8B D9 75 04", 1);
		aobBlocks[ANSEL_STOP_SESSION_KEY] = new AOBBlock(ANSEL_STOP_SESSION_KEY, "48 83 EC 28 48 83 3D ?? ?? ?? ?? 00 74 10 E8 ?? ?? ?? ?? 48 8B 88 80 00 00 00 C6 41 0A 01", 1);
		aobBlocks[ANSEL_SETUP_UPDATECAMERA_KEY] = new AOBBlock(ANSEL_SETUP_UPDATECAMERA_KEY, "F3 0F 10 0D | ?? ?? ?? ?? F3 0F 11 0E F3 0F 10 05 ?? ?? ?? ?? F3 0F 11 46 04 F3 0F 10 0D ?? ?? ?? ?? F3 0F 11 4E 08", 1);
		aobBlocks[ANSEL_RANGELIMITER_KEY] = new AOBBlock(ANSEL_RANGELIMITER_KEY, "0F 86 ?? ?? ?? ?? 48 8B 4B 38 F3 41 0F 58 F5 48 8D 55 DC 48 8B 01", 1);
		aobBlocks[ANSEL_CAMERADATA_WRITES_KEY] = new AOBBlock(ANSEL_CAMERADATA_WRITES_KEY, "F3 0F 10 0D | ?? ?? ?? ?? F3 0F 11 0E F3 0F 10 05 ?? ?? ?? ?? F3 0F 11 46 04 F3 0F 10 0D ?? ?? ?? ?? F3 0F 11 4E 08", 1);
		aobBlocks[ANSEL_CAMERADATA_INIT_KEY] = new AOBBlock(ANSEL_CAMERADATA_INIT_KEY, "F3 0F 11 05 ?? ?? ?? ?? F3 0F 10 4E 04 F3 0F 11 0D ?? ?? ?? ?? 48 8D  98 E8 00 00 00 F3 0F 10 46 08 F3 0F 11 05 ?? ?? ?? ??", 1);
		aobBlocks[ANSEL_FOV_WRITE_KEY] = new AOBBlock(ANSEL_FOV_WRITE_KEY, "F3 0F 11 0D ?? ?? ?? ?? F3 0F 10 46 2C F3 0F 11 05 ?? ?? ?? ?? F3 0F 10 4B 6C F3 0F 11 0D ?? ?? ?? ?? F3 0F 10 43 40", 1);
		aobBlocks[ANSEL_NOCLIP_CHECK_KEY] = new AOBBlock(ANSEL_NOCLIP_CHECK_KEY, "FF 90 90 05 00 00 4C 8B C6 48 8D 54 24 40 | E8 ?? ?? ?? ?? 48 8B 5C 24 60 48 8B 74 24 68 48 83 C4 50", 1);
		aobBlocks[ENABLE_CHEATS_KEY] = new AOBBlock(ENABLE_CHEATS_KEY, "80 BE DE 01 00 00 00 | 75 ?? 40 84 ED 0F 85 ?? ?? ?? ?? 85 DB", 1);

		map<string, AOBBlock*>::iterator it;
		bool result = true;
		for (it = aobBlocks.begin(); it != aobBlocks.end(); it++)
		{
			result &= it->second->scan(hostImageAddress, hostImageSize);
		}

		if (aobBlocks[ANSEL_START_SESSION_KEY]->found())
		{
			_startSessionFunc = (AnselSessionStartStopFunction)aobBlocks[ANSEL_START_SESSION_KEY]->absoluteAddress();
		}
		if (aobBlocks[ANSEL_STOP_SESSION_KEY]->found())
		{
			_stopSessionFunc = (AnselSessionStartStopFunction)aobBlocks[ANSEL_STOP_SESSION_KEY]->absoluteAddress();
		}
		if (aobBlocks[ANSEL_SETUP_UPDATECAMERA_KEY]->found())
		{
			CameraManipulator::setCameraStructAddress(Utils::calculateAbsoluteAddress(aobBlocks[ANSEL_SETUP_UPDATECAMERA_KEY], 4));
		}
		if (aobBlocks[ANSEL_CAMERADATA_INIT_KEY]->found())
		{
			GameImageHooker::setHook(aobBlocks[ANSEL_CAMERADATA_INIT_KEY], 0x5D, &_cameraInitInterceptionContinue, &cameraInitInterceptor);
		}
		if (aobBlocks[ANSEL_NOCLIP_CHECK_KEY]->found())
		{
			GameImageHooker::nopRange(aobBlocks[ANSEL_NOCLIP_CHECK_KEY]->absoluteAddress(), 5);
		}
		if (aobBlocks[ENABLE_CHEATS_KEY]->found())
		{
			// replace jne ... to jmp ...
			BYTE jmpInstruction[1] = { 0xEB };
			GameImageHooker::writeRange(aobBlocks[ENABLE_CHEATS_KEY]->absoluteAddress(), jmpInstruction, 1);
		}

		// AOBs in AnselSDK64 dll
		MODULEINFO ansel64SDKdllInfo = Utils::getModuleInfoOfDll(L"AnselSDK64.dll");
		if (nullptr == ansel64SDKdllInfo.lpBaseOfDll)
		{
			Console::WriteLine("Ansel SDK not found, movement fix for Ansel not applied.");
		}
		else
		{
			aobBlocks[ANSEL_SDK_GETCONFIGURATION_KEY] = new AOBBlock(ANSEL_SDK_GETCONFIGURATION_KEY, "0F 28 05 | ?? ?? ?? ?? 0F 11 01 0F 28 0D ?? ?? ?? ?? 0F 11 49 10 0F 28 05 ?? ?? ?? ?? 0F 11 41 20", 1);
			_anselSDKFound = aobBlocks[ANSEL_SDK_GETCONFIGURATION_KEY]->scan((LPBYTE)ansel64SDKdllInfo.lpBaseOfDll, ansel64SDKdllInfo.SizeOfImage);
			result &= _anselSDKFound;
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


	void disableFovWrite(map<string, AOBBlock*>& aobBlocks)
	{
		// if the writes have to be disabled, we first cache the original writes
		if (!aobBlocks[ANSEL_FOV_WRITE_KEY]->found())
		{
			return;
		}
		// overwrite them with nops
		GameImageHooker::nopRange(aobBlocks[ANSEL_FOV_WRITE_KEY]->absoluteAddress(), 8);
	}
	

	void fixAnsel(map<string, AOBBlock*>& aobBlocks)
	{
		// change the jne to jmp
		BYTE instructionBytes1[1] = { 0xEB };
		GameImageHooker::writeRange(aobBlocks[ANSEL_START_SESSION_KEY]->absoluteAddress() + STARTSESSION_JMP_OFFSET1, instructionBytes1, 1);
		// nop the je
		GameImageHooker::nopRange(aobBlocks[ANSEL_START_SESSION_KEY]->absoluteAddress() + STARTSESSION_JMP_OFFSET2, 2);

		Console::WriteLine("Ansel enabled everywhere.");

		// remove range limit
		//WHGame.DLL+1F939DC - F3 0F58 F0            - addss xmm6,xmm0
		//WHGame.DLL+1F939E0 - 41 0F28 C3            - movaps xmm0,xmm11
		//WHGame.DLL+1F939E4 - F3 0F58 F1            - addss xmm6,xmm1
		//WHGame.DLL+1F939E8 - F3 0F10 0D B0023A00   - movss xmm1,[WHGame.DLL+2333CA0] { (2.00) }
		//WHGame.DLL+1F939F0 - E8 DD7119FF           - call WHGame.DLL+112ABD2 { ->->ucrtbase.dll+2CA60 }
		//WHGame.DLL+1F939F5 - 0F2F F0               - comiss xmm6,xmm0
		//WHGame.DLL+1F939F8 - 0F86 91000000         - jbe WHGame.DLL+1F93A8F			>> change to JMP, (E9 92 00 00 00 00 90)
		//WHGame.DLL+1F939FE - 48 8B 4B 38           - mov rcx,[rbx+38]
		//WHGame.DLL+1F93A02 - F3 41 0F58 F5         - addss xmm6,xmm13
		//WHGame.DLL+1F93A07 - 48 8D 55 DC           - lea rdx,[rbp-24]
		//WHGame.DLL+1F93A0B - 48 8B 01              - mov rax,[rcx]
		//WHGame.DLL+1F93A0E - F3 0F52 CE            - rsqrtss xmm1,xmm6
		//WHGame.DLL+1F93A12 - 0F28 C6               - movaps xmm0,xmm6
		//WHGame.DLL+1F93A15 - F3 0F11 8D C0000000   - movss [rbp+000000C0],xmm1
		//WHGame.DLL+1F93A1D - 41 0F28 CA            - movaps xmm1,xmm10

		Console::WriteLine("Ansel range limit removed.");
		BYTE instructionBytes2[6] = { 0xE9, 0x92, 0x0, 0x0, 0x0, 0x90 };
		GameImageHooker::writeRange(aobBlocks[ANSEL_RANGELIMITER_KEY]->absoluteAddress(), instructionBytes2, 6);
	}


	void startAnselSession()
	{
		if (nullptr != _startSessionFunc)
		{
			_startSessionFunc();
		}
	}


	void stopAnselSession()
	{
		if (nullptr != _stopSessionFunc)
		{
			_stopSessionFunc();
		}
	}
}
