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
	void cameraWrite3Interceptor();
	void cameraWrite4Interceptor();
	void cameraWrite5Interceptor();
	void cameraWrite6Interceptor();
	void cameraWrite7Interceptor();
	void cameraWrite8Interceptor();
	void cameraWrite9Interceptor();
	void cameraWrite10Interceptor();
	void cameraWrite11Interceptor();
	void cameraWrite12Interceptor();
	void cameraWrite13Interceptor();
	void cameraWrite14Interceptor();
	void cameraWrite15Interceptor();
}

// external addresses used in asm.
extern "C" {
	LPBYTE _cameraStructInterceptionContinue = nullptr;
	LPBYTE _cameraWrite1InterceptionContinue = nullptr;
	LPBYTE _cameraWrite2InterceptionContinue = nullptr;
	LPBYTE _cameraWrite3InterceptionContinue = nullptr;
	LPBYTE _cameraWrite4InterceptionContinue = nullptr;
	LPBYTE _cameraWrite5InterceptionContinue = nullptr;
	LPBYTE _cameraWrite6InterceptionContinue = nullptr;
	LPBYTE _cameraWrite7InterceptionContinue1 = nullptr;
	LPBYTE _cameraWrite7InterceptionContinue2 = nullptr;
	LPBYTE _cameraWrite8InterceptionContinue = nullptr;
	LPBYTE _cameraWrite9InterceptionContinue = nullptr;
	LPBYTE _cameraWrite10InterceptionContinue = nullptr;
	LPBYTE _cameraWrite11InterceptionContinue = nullptr;
	LPBYTE _cameraWrite12InterceptionContinue = nullptr;
	LPBYTE _cameraWrite13InterceptionContinue = nullptr;
	LPBYTE _cameraWrite14InterceptionContinue = nullptr;
	LPBYTE _cameraWrite15InterceptionContinue = nullptr;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*> &aobBlocks)
	{
		aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_ADDRESS_INTERCEPT_KEY, "89 0E 8B 48 04 89 4E 04 8B 40 08 48 8B CB 89 46 08 48 8B 03 FF 90 ?? ?? ?? ?? 8B 08 89 0F 8B 48 04 89 4F 04 8B 40 08 89 47 08 E8 ?? ?? ?? ?? 33 D2", 1);	
		aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE1_INTERCEPT_KEY, "F3 0F 11 07 F3 0F 10 44 24 4C F3 0F 11 4F 04 F3 0F 11 47 08 4C 8D 9C 24", 1);
		aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE2_INTERCEPT_KEY, "F3 0F 11 07 F3 0F 10 44 24 6C F3 0F 11 4F 04 F3 0F 11 47 08 4C 8D 9C 24", 1);
		aobBlocks[CAMERA_WRITE3_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE3_INTERCEPT_KEY, "F3 0F 11 06 F3 0F 10 45 AF F3 0F 11 4E 04 F3 0F 10 4D 97 F3 0F 11 46 08 F3 0F 10 45 9B F3 0F 11 0F F3 0F 10 4D 9F F3 0F 11 47 04 F3 0F 11 4F 08 EB 56", 1);
		aobBlocks[CAMERA_WRITE4_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE4_INTERCEPT_KEY, "89 0F 8B 48 04 89 4F 04 8B 40 08 48 8B CE 89 47 08 C7 46 08 00 00 00 00 F3 0F 11 06 F3 0F 11 4E 04 E8", 1);
		aobBlocks[CAMERA_WRITE5_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE5_INTERCEPT_KEY, "F3 0F 11 7F 08 0F 28 7C 24 70 F3 0F 11 07 F3 0F 11 4F 04 49 8B E3 5F", 1);
		aobBlocks[CAMERA_WRITE6_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE6_INTERCEPT_KEY, "89 0B 8B 48 04 89 4B 04 8B 48 08 89 4B 08 49 8B CC FF 52 48 8B 08 89 0F 8B 48 04 89 4F 04 8B 40 08 49 8B CC 89 47 08 49 8B 04 24 FF 90", 1);
		aobBlocks[CAMERA_WRITE7_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE7_INTERCEPT_KEY, "41 0F 28 CA F3 0F 59 C6 F3 0F 11 47 04 41 0F 28 C0 E8 9C 3D 1D 00 41 0F 28 CC F3 0F 59 C6 F3 0F 11 07 41 0F 28 C3 E8 87 3D 1D 00 F3 0F 59 C6 F3 0F 11 47 08", 1);
		aobBlocks[CAMERA_WRITE8_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE8_INTERCEPT_KEY, "F3 0F 11 06 F3 0F 10 45 AF F3 0F 11 4E 04 F3 0F 10 4D 97 F3 0F 11 46 08 F3 0F 10 45 9B F3 0F 11 0F F3 0F 10 4D 9F F3 0F 11 47 04 F3 0F 11 4F 08 E8", 1);
		aobBlocks[CAMERA_WRITE9_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE9_INTERCEPT_KEY, "F3 0F 11 07 F3 0F 10 48 04 F3 0F 58 4F 04 F3 0F 11 4F 04 F3 0F 10 40 08 F3 0F 58 47 08 F3 0F 11 47 08 48 8D 4D B7 48 8B D3 E8", 1);
		aobBlocks[CAMERA_WRITE10_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE10_INTERCEPT_KEY, "F3 0F 11 07 F3 0F 10 48 04 F3 0F 58 4F 04 F3 0F 11 4F 04 F3 0F 10 40 08 F3 0F 58 47 08 F3 0F 11 47 08 E8", 1);
		aobBlocks[CAMERA_WRITE11_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE11_INTERCEPT_KEY, "F3 0F 11 06 F3 0F 10 45 AF F3 0F 11 4E 04 F3 0F 58 46 08 F3 0F 11 4D AB F3 0F 11 46 08 F3 0F 10 06", 1);
		aobBlocks[CAMERA_WRITE12_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE12_INTERCEPT_KEY, "F3 0F 11 02 F3 0F 59 89 FC 03 00 00 F3 0F 58 4A 04 F3 0F 11 4A 04 F3 0F 10 4C 24 28 F3 0F 59 99 00 04 00 00", 1);
		aobBlocks[CAMERA_WRITE13_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE13_INTERCEPT_KEY, "F3 0F 11 03 F3 0F 10 8F 14 1E 00 00 F3 0F 58 4B 04 F3 0F 11 4B 04 F3 0F 10 87 18 1E 00 00 F3 0F 58 43 08 F3 0F 11 43 08 48 8B 5C 24 30", 1);
		aobBlocks[CAMERA_WRITE14_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE14_INTERCEPT_KEY, "F3 0F 11 07 F3 0F 10 44 24 5C F3 0F 11 4F 04 F3 0F 11 47 08 E8 AD 5F 01 00", 1);
		aobBlocks[CAMERA_WRITE15_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE15_INTERCEPT_KEY, "41 89 8F EC 00 00 00 8B 48 04 41 89 8F F0 00 00 00 8B 48 08 41 89 8F F4 00 00 00 48 8B CB FF 52 48 8B 08", 1);
		aobBlocks[FOV_MIN_CLAMP_LOCATION_KEY] = new AOBBlock(FOV_MIN_CLAMP_LOCATION_KEY, "F3 0F 10 58 58 | F3 0F 5F 1D ?? ?? ?? ?? 0F 28 C3 F3 0F 5D 05 ?? ?? ?? ?? 48", 1);
		aobBlocks[FOV_MAX_CLAMP_LOCATION_KEY] = new AOBBlock(FOV_MAX_CLAMP_LOCATION_KEY, "F3 0F 10 58 58 F3 0F 5F 1D ?? ?? ?? ?? 0F 28 C3 | F3 0F 5D 05 ?? ?? ?? ?? 48", 1);
		aobBlocks[FOV_ADDRESS_LOCATION_KEY] = new AOBBlock(FOV_ADDRESS_LOCATION_KEY, "48 8B 05 | ?? ?? ?? ?? F3 0F 10 58 58 F3 0F5F 1D ?? ?? ?? ?? 0F 28 C3 F3 0F 5D 05 ?? ?? ?? ?? 48", 1);
		aobBlocks[ENGINECLIENT_LOCATION_KEY] = new AOBBlock(ENGINECLIENT_LOCATION_KEY, "48 8B 0D | ?? ?? ?? ?? 48 8D 15 ?? ?? ?? ?? 48 8B 01 FF 90 D8 00 00 00 48 63 0D ?? ?? ?? ?? 48 8B D9", 1);

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
		GameImageHooker::setHook(aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY], 0x2A, &_cameraStructInterceptionContinue, &cameraStructInterceptor);
	}


	void setPostCameraStructHooks(map<string, AOBBlock*> &aobBlocks)
	{
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY], 0x14, &_cameraWrite1InterceptionContinue, &cameraWrite1Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY], 0x14, &_cameraWrite2InterceptionContinue, &cameraWrite2Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE3_INTERCEPT_KEY], 0x30, &_cameraWrite3InterceptionContinue, &cameraWrite3Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE4_INTERCEPT_KEY], 0x21, &_cameraWrite4InterceptionContinue, &cameraWrite4Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE5_INTERCEPT_KEY], 0x13, &_cameraWrite5InterceptionContinue, &cameraWrite5Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE6_INTERCEPT_KEY], 0x27, &_cameraWrite6InterceptionContinue, &cameraWrite6Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE7_INTERCEPT_KEY], 0x11, &_cameraWrite7InterceptionContinue1, &cameraWrite7Interceptor, 0x34, &_cameraWrite7InterceptionContinue2);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE8_INTERCEPT_KEY], 0x30, &_cameraWrite8InterceptionContinue, &cameraWrite8Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE9_INTERCEPT_KEY], 0x22, &_cameraWrite9InterceptionContinue, &cameraWrite9Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE10_INTERCEPT_KEY], 0x22, &_cameraWrite10InterceptionContinue, &cameraWrite10Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE11_INTERCEPT_KEY], 0x1D, &_cameraWrite11InterceptionContinue, &cameraWrite11Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE12_INTERCEPT_KEY], 0x57, &_cameraWrite12InterceptionContinue, &cameraWrite12Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE13_INTERCEPT_KEY], 0x28, &_cameraWrite13InterceptionContinue, &cameraWrite13Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE14_INTERCEPT_KEY], 0x14, &_cameraWrite14InterceptionContinue, &cameraWrite14Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE15_INTERCEPT_KEY], 0x3A, &_cameraWrite15InterceptionContinue, &cameraWrite15Interceptor);

		CameraManipulator::setFoVAddress(Utils::calculateAbsoluteAddress(aobBlocks[FOV_ADDRESS_LOCATION_KEY], 4));	// client.dll+2C5A44 - 48 8B 05 E5CAF200     - mov rax,[client.dll+11F2530]
		CameraManipulator::setEngineClientLocationAddress(Utils::calculateAbsoluteAddress(aobBlocks[ENGINECLIENT_LOCATION_KEY], 4));
		nopFoVClamps(aobBlocks);
	}


	// nops the statements which clamps the fov min/max. 
	void nopFoVClamps(map<string, AOBBlock*> &aobBlocks)
	{
		//client.dll+2C5A44 - 48 8B 05 E5CAF200     - mov rax,[client.dll+11F2530] { [7FF89021DE00] }
		//client.dll+2C5A4B - F3 0F10 58 58         - movss xmm3,[rax+58]								<<< READ FOV
		//client.dll+2C5A50 - F3 0F5F 1D 20277400   - maxss xmm3,[client.dll+A08178] { [1.00] }			<< CLAMP MIN
		//client.dll+2C5A58 - 0F28 C3               - movaps xmm0,xmm3
		//client.dll+2C5A5B - F3 0F5D 05 D9906400   - minss xmm0,[client.dll+90EB3C] { [1.70] }			<< CLAMP MAX
		//client.dll+2C5A63 - 48 83 C4 28           - add rsp,28 { 40 }

		// we simply write the maxss and minss statements with NOPs. 
		GameImageHooker::nopRange(aobBlocks[FOV_MIN_CLAMP_LOCATION_KEY]->absoluteAddress(), 8);
		GameImageHooker::nopRange(aobBlocks[FOV_MAX_CLAMP_LOCATION_KEY]->absoluteAddress(), 8);
	}
}
