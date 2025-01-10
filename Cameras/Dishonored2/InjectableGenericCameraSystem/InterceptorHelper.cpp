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

namespace IGCS::GameSpecific::InterceptorHelper
{
	typedef void(__stdcall* AnselSessionStartStopFunction) ();

	static bool _anselSDKFound = false;
	static AnselSessionStartStopFunction _startSessionFunc = nullptr;
	static AnselSessionStartStopFunction _stopSessionFunc = nullptr;

	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*>& aobBlocks)
	{
		// AOBs in hostimage
		aobBlocks[ANSEL_START_SESSION_KEY] = new AOBBlock(ANSEL_START_SESSION_KEY, "48 83 EC 28 48 8B 0D ?? ?? ?? ?? 48 85 C9 0F 84 ?? ?? ?? ?? 48 8B 01 FF 90 ?? ?? ?? ?? 84 C0", 1);
		aobBlocks[ANSEL_STOP_SESSION_KEY] = new AOBBlock(ANSEL_STOP_SESSION_KEY, "48 83 EC 28 8B 15 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? 41 B0 01 E8", 1);
		aobBlocks[ANSEL_SETUP_UPDATECAMERA_KEY] = new AOBBlock(ANSEL_SETUP_UPDATECAMERA_KEY, "F3 0F 11 05 | ?? ?? ?? ?? F3 0F 10 8F ?? 3A 00 00 F3 0F 11 0D ?? ?? ?? ?? F3 0F 10 87 ?? 3A 00 00", 1);
		aobBlocks[WIN_PAUSEONFOCUSLOSS_KEY] = new AOBBlock(WIN_PAUSEONFOCUSLOSS_KEY, "83 3D | ?? ?? ?? ?? 00 74 4E 48 8B 0D ?? ?? ?? ?? 48 8B 01", 1);
	
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


	void fixAnsel(map<string, AOBBlock*>& aobBlocks)
	{
		// nop the jne's so start session always succeeds.
		GameImageHooker::nopRange(aobBlocks[ANSEL_START_SESSION_KEY]->absoluteAddress() + STARTSESSION_JMP_OFFSET1, 6);
		GameImageHooker::nopRange(aobBlocks[ANSEL_START_SESSION_KEY]->absoluteAddress() + STARTSESSION_JMP_OFFSET2, 6);
		GameImageHooker::nopRange(aobBlocks[ANSEL_START_SESSION_KEY]->absoluteAddress() + STARTSESSION_JMP_OFFSET3, 6);
		GameImageHooker::nopRange(aobBlocks[ANSEL_START_SESSION_KEY]->absoluteAddress() + STARTSESSION_JMP_OFFSET4, 6);
		GameImageHooker::nopRange(aobBlocks[ANSEL_START_SESSION_KEY]->absoluteAddress() + STARTSESSION_JMP_OFFSET5, 6);
		GameImageHooker::nopRange(aobBlocks[ANSEL_START_SESSION_KEY]->absoluteAddress() + STARTSESSION_JMP_OFFSET6, 6);

		// switch off win_pauseOnLossOfFocus. This is needed when playing windowed and enabling ansel (for the people who want to).
		if (aobBlocks[WIN_PAUSEONFOCUSLOSS_KEY]->found())
		{
			// Dishonored_DO.exe+12E868 - 83 3D 19C93C03 00     - cmp dword ptr [Dishonored_DO.exe+34FB188],00
			// pass 5 to calculate function, as next opcode is 5 bytes away (4 + the byte for 00).
			GameImageHooker::writeBytesToProcessMemory(Utils::calculateAbsoluteAddress(aobBlocks[WIN_PAUSEONFOCUSLOSS_KEY], 5), 1, (BYTE)0);
		}

		Console::WriteLine("Ansel enabled everywhere.");

		// set the movement speed inside the ansel struct in memory to 2.0f. 
		if (_anselSDKFound)
		{
			float speedValue = 2.0f;
			BYTE* speedValueFirstByte;
			speedValueFirstByte = (BYTE*)& speedValue;
			// calculate the address of the variable, relative to the RIP value lifted from the first getConfiguration statement. 
			// AnselSDK64.dll+1850 - 0F28 05 799F0100      - movaps xmm0,[AnselSDK64.dll+1B7D0]
			LPBYTE configStartAddress = Utils::calculateAbsoluteAddress(aobBlocks[ANSEL_SDK_GETCONFIGURATION_KEY], 4);
			// value is at offset 0x24, 3x3 floats first, then the value we have to set, translationalSpeedInWorldUnitsPerSecond
			// nv::Vec3 right, up, forward;
			// float translationalSpeedInWorldUnitsPerSecond;
			GameImageHooker::writeRange(configStartAddress + 0x24, speedValueFirstByte, 4);

			Console::WriteLine("Ansel fixed so movement now works.");
		}
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
