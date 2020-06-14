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
	static LPBYTE _gamePauseAddress = nullptr;

	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*>& aobBlocks)
	{
		// AOBs in hostimage
		aobBlocks[ANSEL_START_SESSION_KEY] = new AOBBlock(ANSEL_START_SESSION_KEY, "74 14 85 C0 74 10 B8 01 00 00 00", 1);
		aobBlocks[ANSEL_RANGELIMITER_KEY] = new AOBBlock(ANSEL_RANGELIMITER_KEY, "F3 0F 5D 1D ?? ?? ?? ?? F3 0F 5C C8 F3 0F 5F 5D", 1);
		aobBlocks[ANSEL_FOV_WRITE_KEY] = new AOBBlock(ANSEL_FOV_WRITE_KEY, "F3 0F 11 25 ?? ?? ?? ?? F3 0F 10 53 4C 0F 2F D0", 1);
		aobBlocks[CAMERA_ADDRESS_KEY] = new AOBBlock(CAMERA_ADDRESS_KEY, "0F 29 05 | ?? ?? ?? ?? 0F 28 45 B0 0F 29 0D ?? ?? ?? ?? 0F 28 4D A0", 1);
		aobBlocks[FOV_READ_KEY] = new AOBBlock(FOV_READ_KEY, "F3 0F 59 25 | ?? ?? ?? ?? 8B 44 24 50 F3 0F 10 1D ?? ?? ?? ?? F3 0F 11 85", 1);
		aobBlocks[CAMERA_WRITE1_KEY] = new AOBBlock(CAMERA_WRITE1_KEY, "44 0F 29 25 ?? ?? ?? ?? F3 0F 10 53 48 0F 28 E2", 1);
		aobBlocks[CAMERA_WRITE2_KEY] = new AOBBlock(CAMERA_WRITE2_KEY, "0F 11 1E F3 0F 10 5C 24 6C F3 0F 5D 1D ?? ?? ?? ?? F3 0F 11", 1);
		aobBlocks[CAMERA_WRITE3_KEY] = new AOBBlock(CAMERA_WRITE3_KEY, "F3 0F 11 9E ?? ?? ?? ?? 0F 11 4E 10 0F 28 4D 90 0F 11 46 20 0F 11 4E 30", 1);
		aobBlocks[CAMERA_WRITE4_KEY] = new AOBBlock(CAMERA_WRITE4_KEY, "0F 29 35 ?? ?? ?? ?? F2 0F 10 45 C8 45 0F C6 C9 39", 1);
		// write 5& 6 are relative to write4
		aobBlocks[CAMERA_WRITE7_KEY] = new AOBBlock(CAMERA_WRITE7_KEY, "0F 29 05 ?? ?? ?? ?? 0F 28 45 B0 0F 29 0D ?? ?? ?? ?? 0F 28 4D A0", 1);
		aobBlocks[GAME_PAUSE_ADDRESS_KEY] = new AOBBlock(GAME_PAUSE_ADDRESS_KEY, "38 05 | ?? ?? ?? ?? 74 0D 38 05 ?? ?? ?? ?? 75 05 45 ?? C0", 1);
		aobBlocks[TIME_DILATION_ADDRESS_KEY] = new AOBBlock(TIME_DILATION_ADDRESS_KEY, "F3 0F 11 05 | ?? ?? ?? ?? F3 0F 11 0D ?? ?? ?? ?? 74 06 ", 1);

		map<string, AOBBlock*>::iterator it;
		bool result = true;
		for (it = aobBlocks.begin(); it != aobBlocks.end(); it++)
		{
			result &= it->second->scan(hostImageAddress, hostImageSize);
		}

		if (aobBlocks[CAMERA_ADDRESS_KEY]->found())
		{
			CameraManipulator::setCameraStructAddress(Utils::calculateAbsoluteAddress(aobBlocks[CAMERA_ADDRESS_KEY], 4));

			if(aobBlocks[FOV_READ_KEY]->found())
			{
				CameraManipulator::setFoVAddress(Utils::calculateAbsoluteAddress(aobBlocks[FOV_READ_KEY], 4));
			}
		}
		if(aobBlocks[GAME_PAUSE_ADDRESS_KEY]->found())
		{
			_gamePauseAddress = Utils::calculateAbsoluteAddress(aobBlocks[GAME_PAUSE_ADDRESS_KEY], 4);
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


	// If enable is true, the writes to all camera values are blocked, fov write for ansel is blocked. Otherwise they're re-enabled as normal.
	void enableDisableCamera(bool enable, map<string, AOBBlock*>& aobBlocks)
	{
		// write1:	MetroExodus.exe+FE28004 - 44 0F29 25 F40991F1	- movaps [MetroExodus.exe+1738A00],xmm12 		<<<<<< write coords (matrix row 0)
		// write2:	MetroExodus.exe+123F049A - 0F11 1E              - movups [rsi],xmm3								<< Write coords
		// write3:	MetroExodus.exe+123F04BF - F3 0F11 9E B4040000  - movss [rsi+000004B4],xmm3						<< Write Fov (Ansel)
		//			MetroExodus.exe+123F04C7 - 0F11 4E 10           - movups [rsi+10],xmm1							<< Row 1
		//			MetroExodus.exe+123F04CB - 0F28 4D 90           - movaps xmm1,[rbp-70]						<<<<<<< leave as-is
		//			MetroExodus.exe+123F04CF - 0F11 46 20           - movups [rsi+20],xmm0							<< Row 2
		//			MetroExodus.exe+123F04D3 - 0F11 4E 30           - movups [rsi+30],xmm1							<< Row 3
		// write4:	MetroExodus.exe+FE27F71 - 0F29 35 A80A91F1      - movaps [MetroExodus.exe+1738A20],xmm6  		<< Row 2
		//										Write 5 & 6 are relative to write 4
		// write5:	MetroExodus.exe+FE27F86 - 44 0F29 0D 820A91F1   - movaps [MetroExodus.exe+1738A10],xmm9			<< Row 1
		// write6:	MetroExodus.exe+FE27FAB - 0F29 1D 7E0A91F1      - movaps [MetroExodus.exe+1738A30],xmm3 		<< Row 3
		// write7:	MetroExodus.exe+FE2C521 - 0F29 05 D8C490F1      - movaps [MetroExodus.exe+1738A00],xmm0 		<< coords row
		//			MetroExodus.exe+FE2C528 - 0F28 45 B0			- movaps xmm0, [rbp - 50]					<<<<<<< leave as-is
		//			MetroExodus.exe+FE2C52C - 0F29 0D DDC490F1		- movaps[MetroExodus.exe + 1738A10], xmm1		<< row 1
		//			MetroExodus.exe+FE2C533 - 0F28 4D A0			- movaps xmm1, [rbp - 60]					<<<<<<< leave as-is
		//			MetroExodus.exe+FE2C537 - 0F29 05 E2C490F1		- movaps[MetroExodus.exe + 1738A20], xmm0		<< row 2
		//			MetroExodus.exe+FE2C53E - F3 0F10 05 DAC890F1	- movss xmm0, [MetroExodus.exe + 1738E20]	<<<<<<< leave as-is
		//			MetroExodus.exe+FE2C546 - 0F29 0D E3C490F1		- movaps[MetroExodus.exe + 1738A30], xmm1		<< row 3
		// we first have to make sure the original bytes are preserved, as we're dealing with RIP relative addresses.
		static bool bytesPreserved = false;
		static BYTE originalWrite1Bytes[8];
		static BYTE originalWrite2Bytes[3];
		static BYTE originalWrite3Bytes[24];
		static BYTE originalWrite4Bytes[7];
		static BYTE originalWrite5Bytes[8];
		static BYTE originalWrite6Bytes[7];
		static BYTE originalWrite7Bytes[44];

		if(!bytesPreserved)
		{
			// cache bytes
			memcpy(originalWrite1Bytes, aobBlocks[CAMERA_WRITE1_KEY]->absoluteAddress(), 8);
			memcpy(originalWrite2Bytes, aobBlocks[CAMERA_WRITE2_KEY]->absoluteAddress(), 3);
			memcpy(originalWrite3Bytes, aobBlocks[CAMERA_WRITE3_KEY]->absoluteAddress(), 24);
			memcpy(originalWrite4Bytes, aobBlocks[CAMERA_WRITE4_KEY]->absoluteAddress(), 7);
			memcpy(originalWrite5Bytes, aobBlocks[CAMERA_WRITE4_KEY]->absoluteAddress()+CAMERA_WRITE5_OFFSET, 8);
			memcpy(originalWrite6Bytes, aobBlocks[CAMERA_WRITE4_KEY]->absoluteAddress()+CAMERA_WRITE6_OFFSET, 7);
			memcpy(originalWrite7Bytes, aobBlocks[CAMERA_WRITE7_KEY]->absoluteAddress(), 44);
			bytesPreserved = true;
		}
		
		if(enable)
		{
			// nop writes
			GameImageHooker::nopRange(aobBlocks[CAMERA_WRITE1_KEY]->absoluteAddress(), 8);
			GameImageHooker::nopRange(aobBlocks[CAMERA_WRITE2_KEY]->absoluteAddress(), 3);
			GameImageHooker::nopRange(aobBlocks[CAMERA_WRITE3_KEY]->absoluteAddress(), 12);	// 2 instructions
			GameImageHooker::nopRange(aobBlocks[CAMERA_WRITE3_KEY]->absoluteAddress() + CAMERA_WRITE3_2OFFSET, 8);	// 2 instructions
			GameImageHooker::nopRange(aobBlocks[CAMERA_WRITE4_KEY]->absoluteAddress(), 7);
			GameImageHooker::nopRange(aobBlocks[CAMERA_WRITE4_KEY]->absoluteAddress() + CAMERA_WRITE5_OFFSET, 8);
			GameImageHooker::nopRange(aobBlocks[CAMERA_WRITE4_KEY]->absoluteAddress() + CAMERA_WRITE6_OFFSET, 7);
			GameImageHooker::nopRange(aobBlocks[CAMERA_WRITE7_KEY]->absoluteAddress(), 7);
			GameImageHooker::nopRange(aobBlocks[CAMERA_WRITE7_KEY]->absoluteAddress() + CAMERA_WRITE7_2OFFSET, 7);
			GameImageHooker::nopRange(aobBlocks[CAMERA_WRITE7_KEY]->absoluteAddress() + CAMERA_WRITE7_3OFFSET, 7);
			GameImageHooker::nopRange(aobBlocks[CAMERA_WRITE7_KEY]->absoluteAddress() + CAMERA_WRITE7_4OFFSET, 7);
		}
		else
		{
			// re-enable writes.
			GameImageHooker::writeRange(aobBlocks[CAMERA_WRITE1_KEY]->absoluteAddress(), originalWrite1Bytes, 8);
			GameImageHooker::writeRange(aobBlocks[CAMERA_WRITE2_KEY]->absoluteAddress(), originalWrite2Bytes, 3);
			GameImageHooker::writeRange(aobBlocks[CAMERA_WRITE3_KEY]->absoluteAddress(), originalWrite3Bytes, 24);
			GameImageHooker::writeRange(aobBlocks[CAMERA_WRITE4_KEY]->absoluteAddress(), originalWrite4Bytes, 7);
			GameImageHooker::writeRange(aobBlocks[CAMERA_WRITE4_KEY]->absoluteAddress() + CAMERA_WRITE5_OFFSET, originalWrite5Bytes, 8);
			GameImageHooker::writeRange(aobBlocks[CAMERA_WRITE4_KEY]->absoluteAddress() + CAMERA_WRITE6_OFFSET, originalWrite6Bytes, 7);
			GameImageHooker::writeRange(aobBlocks[CAMERA_WRITE7_KEY]->absoluteAddress(), originalWrite7Bytes, 44);
		}
	}

	
	void toggleGamePause(map<string, AOBBlock*>& aobBlocks)
	{
		if(nullptr==_gamePauseAddress)
		{
			return;
		}
		bool pauseGame = (*_gamePauseAddress == (BYTE)0) ? true : false;

		if (pauseGame)
		{
			// first toggle the time dilation, then wait 66ms to skip a few frames to let the TAA bleed out, then set the pause. 
			toggleTimeDilationPause(aobBlocks, true);
			Sleep(66);
			*_gamePauseAddress = (BYTE)1;

			// then restore the time dilation.
			toggleTimeDilationPause(aobBlocks, false);
		}
		else
		{
			// just reset the pause to 0
			*_gamePauseAddress = (BYTE)0;
		}
	}

		
	void toggleTimeDilationPause(map<string, AOBBlock*>& aobBlocks, bool pauseGame)
	{
		// MetroExodus.exe+ACF852 - F3 0F11 05 C2E8CA00   - movss [MetroExodus.exe+177E11C],xmm0
		static uint8_t originalBytes[8];
		
		if(pauseGame)
		{
			// cache original bytes
			memcpy(originalBytes, aobBlocks[TIME_DILATION_ADDRESS_KEY]->absoluteAddress()-4, 8);
			LPBYTE timeDilationAddress = Utils::calculateAbsoluteAddress(aobBlocks[TIME_DILATION_ADDRESS_KEY], 4);
			// the AOB is 4 bytes ahead of the instruction start.
			GameImageHooker::nopRange(aobBlocks[TIME_DILATION_ADDRESS_KEY]->absoluteAddress() - 4, 8);
			// set the time dilation to 0.0001 or smaller, so the engine pauses. 
			*reinterpret_cast<float*>(timeDilationAddress) = 0.001f;
		}
		else
		{
			// restore original bytes
			// AOB is 4 bytes ahead of the address to write to
			GameImageHooker::writeRange(aobBlocks[TIME_DILATION_ADDRESS_KEY]->absoluteAddress()-4, originalBytes, 8);
		}
	}


	void fixAnsel(map<string, AOBBlock*>& aobBlocks)
	{
		if (aobBlocks[ANSEL_START_SESSION_KEY]->found())
		{
			// nop 2 relative jumps
			// MetroExodus.exe+7B82C3A - 74 14                 - je MetroExodus.exe+7B82C50
			GameImageHooker::nopRange(aobBlocks[ANSEL_START_SESSION_KEY]->absoluteAddress() + STARTSESSION_JMP_OFFSET1, 2);
			// MetroExodus.exe+7B82C3E - 74 10                 - je MetroExodus.exe+7B82C50
			GameImageHooker::nopRange(aobBlocks[ANSEL_START_SESSION_KEY]->absoluteAddress() + STARTSESSION_JMP_OFFSET2, 2);
			Console::WriteLine("Ansel enabled everywhere.");
		}
		
		if (aobBlocks[ANSEL_RANGELIMITER_KEY]->found())
		{
			// remove range limit
			// nop a minss.
			// MetroExodus.exe+7429F44 - F3 0F5D 1D D4D31BFA   - minss xmm3,[MetroExodus.exe+15E7320]
			GameImageHooker::nopRange(aobBlocks[ANSEL_RANGELIMITER_KEY]->absoluteAddress(), 8);
			Console::WriteLine("Ansel range limit removed.");
		}
	}
}
