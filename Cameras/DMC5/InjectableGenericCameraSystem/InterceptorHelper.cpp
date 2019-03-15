////////////////////////////////////////////////////////////////////////////////////////////////////////
// Part of Injectable Generic Camera System
// Copyright(c) 2019, Frans Bouma
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
	void resolutionScaleReadInterceptor();
	void timestopReadInterceptor();
	void displayTypeInterceptor();
	void dofSelectorWriteInterceptor();
}

// external addresses used in asm.
extern "C" {
	LPBYTE _cameraStructInterceptionContinue = nullptr;
	LPBYTE _cameraWrite1InterceptionContinue = nullptr;
	LPBYTE _cameraWrite2InterceptionContinue = nullptr;
	LPBYTE _cameraWrite3InterceptionContinue = nullptr;
	LPBYTE _resolutionScaleReadInterceptionContinue = nullptr;
	LPBYTE _timestopReadInterceptionContinue = nullptr;
	LPBYTE _displayTypeInterceptionContinue = nullptr;
	LPBYTE _dofSelectorWriteInterceptionContinue = nullptr;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*> &aobBlocks)
	{
		aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_ADDRESS_INTERCEPT_KEY, "F3 0F 11 46 38 48 8B 43 50 48 83 78 18 00 75 ?? F3 0F", 1);	
		aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE1_INTERCEPT_KEY, "41 8B 06 89 47 40 41 8B 46 04 89 47 44 41 8B 46 08 89 47 48 41 8B 46 0C 89 47 4C 80 BF D2 00 00 00 00", 1);
		aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE2_INTERCEPT_KEY, "8B 08 89 4F 40 8B 48 04 89 4F 44 8B 48 08 89 4F 48 8B 40 0C 89 47 4C 80 BF D2 00 00 00 00", 1);
		aobBlocks[CAMERA_WRITE3_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE3_INTERCEPT_KEY, "EB ?? | 41 8B 06 89 47 30 41 8B 46 04 89 47 34 41 8B 46 08 89 47 38 80 BF D2 00 00 00 00", 1);
		aobBlocks[TIMESTOP_READ_INTERCEPT_KEY] = new AOBBlock(TIMESTOP_READ_INTERCEPT_KEY, "F3 0F 10 87 84 03 00 00 0F 2F C2 F3 0F 10 9F 80 03 00 00", 1);
		aobBlocks[RESOLUTION_SCALE_INTERCEPT_KEY] = new AOBBlock(RESOLUTION_SCALE_INTERCEPT_KEY, "F3 0F 10 80 6C 12 00 00 48 8D 44 24 3C F3 41 0F 59 46 40", 1);
		aobBlocks[DISPLAYTYPE_INTERCEPT_KEY] = new AOBBlock(DISPLAYTYPE_INTERCEPT_KEY, "44 0F 29 44 24 40 44 0F 29 4C 24 30 F3 0F 11 45 10 F3 0F 11 4D 14", 1);
		aobBlocks[DOF_SELECTOR_WRITE_INTERCEPT_KEY] = new AOBBlock(DOF_SELECTOR_WRITE_INTERCEPT_KEY, "89 51 4C 85 D2 74 0E 83 EA 01 74 09 83 FA 01 75 08 88 51 50 C3", 1);
		aobBlocks[DOF_FAR_BLUR_STRENGTH_WRITE_KEY] = new AOBBlock(DOF_FAR_BLUR_STRENGTH_WRITE_KEY, "8B 00 89 41 74 48 8D 44 24 10 48 8D 4C 24 08 F3 0F 11 4C 24 10", 1);

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
		GameImageHooker::setHook(aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY], 0x0E, &_cameraStructInterceptionContinue, &cameraStructInterceptor);
	}


	void setPostCameraStructHooks(map<string, AOBBlock*> &aobBlocks)
	{
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY], 0x1B, &_cameraWrite1InterceptionContinue, &cameraWrite1Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY], 0x17, &_cameraWrite2InterceptionContinue, &cameraWrite2Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE3_INTERCEPT_KEY], 0x14, &_cameraWrite3InterceptionContinue, &cameraWrite3Interceptor);
		GameImageHooker::setHook(aobBlocks[TIMESTOP_READ_INTERCEPT_KEY], 0x13, &_timestopReadInterceptionContinue, &timestopReadInterceptor);
		GameImageHooker::setHook(aobBlocks[RESOLUTION_SCALE_INTERCEPT_KEY], 0x13, &_resolutionScaleReadInterceptionContinue, &resolutionScaleReadInterceptor);
		GameImageHooker::setHook(aobBlocks[DISPLAYTYPE_INTERCEPT_KEY], 0x11, &_displayTypeInterceptionContinue, &displayTypeInterceptor);
		GameImageHooker::setHook(aobBlocks[DOF_SELECTOR_WRITE_INTERCEPT_KEY], 0x19, &_dofSelectorWriteInterceptionContinue, &dofSelectorWriteInterceptor);
	}


	// if enabled is true, we'll place a 'ret' at the start of the code block, making the game skip rendering any hud element. If false, we'll reset
	// the original first statement so code will proceed as normal. 
	void toggleHudRenderState(map<string, AOBBlock*> &aobBlocks, bool enabled)
	{
		//DevilMayCry5.exe+17688C49 - 48 8B 7C 24 30        - mov rdi,[rsp+30]
		//DevilMayCry5.exe+17688C4E - 48 8B 46 50           - mov rax,[rsi+50]
		//DevilMayCry5.exe+17688C52 - 48 83 78 18 00        - cmp qword ptr [rax+18],00 { 0 }
		//DevilMayCry5.exe+17688C57 - 75 0B                 - jne DevilMayCry5.exe+17688C64
		//DevilMayCry5.exe+17688C59 - 48 89 DA              - mov rdx,rbx
		//DevilMayCry5.exe+17688C5C - 48 89 F1              - mov rcx,rsi
		//DevilMayCry5.exe+17688C5F - E8 3CD6C8EA           - call DevilMayCry5.exe+23162A0		<< NOP this to hide hud/targeting rectangle
		//DevilMayCry5.exe+17688C64 - 48 8B 5C 24 38        - mov rbx,[rsp+38]
		//DevilMayCry5.exe+17688C69 - 48 83 C4 20           - add rsp,20 { 32 }
		//DevilMayCry5.exe+17688C6D - 5E                    - pop rsi
		//DevilMayCry5.exe+17688C6E - C3                    - ret 
		//if (enabled)
		//{
		//	// nop the call
		//	GameImageHooker::nopRange(aobBlocks[HUD_RENDER_LOCATION_KEY], 5);		// DevilMayCry5.exe+17688C5F - E8 3CD6C8EA           - call DevilMayCry5.exe+23162A0
		//}
		//else
		//{
		//	// set original statement
		//	BYTE statementBytes[3] = { 0x48, 0x8B, 0xC4 };			// DevilMayCry5.exe+17688C5F - E8 3CD6C8EA           - call DevilMayCry5.exe+23162A0
		//	GameImageHooker::writeRange(aobBlocks[HUD_RENDER_LOCATION_KEY], statementBytes, 3);
		//}
	}


	// Toggles the in-game dof. it selects dof '1' and sets far blur to 0. 
	void toggleInGameDoFOff(map<string, AOBBlock*> &aobBlocks, bool switchOff)
	{
		if (switchOff)
		{
			// first select the dof, type 1. 
			CameraManipulator::selectDoF(1);

			// write xor eax, eax at the far blur strength value read, so it's always 0
			//DevilMayCry5.exe+194C767A - 48 8D 4C 24 08        - lea rcx,[rsp+08]
			//DevilMayCry5.exe+194C767F - 8B 00                 - mov eax,[rax]
			//DevilMayCry5.exe+194C7681 - 89 82 C4010000        - mov [rdx+000001C4],eax
			//DevilMayCry5.exe+194C7687 - 48 8D 44 24 10        - lea rax,[rsp+10]
			//DevilMayCry5.exe+194C768C - 48 0F46 C1            - cmovbe rax,rcx
			//DevilMayCry5.exe+194C7690 - F3 0F11 4C 24 10      - movss [rsp+10],xmm1
			//DevilMayCry5.exe+194C7696 - 49 8B 48 38           - mov rcx,[r8+38]
			//DevilMayCry5.exe+194C769A - C7 44 24 08 17B7D138  - mov [rsp+08],38D1B717 { (0) }
			//DevilMayCry5.exe+194C76A2 - 8B 00                 - mov eax,[rax]						<< REPLACE WITH XOR eax, eax (31 C0 xor eax, eax)
			//DevilMayCry5.exe+194C76A4 - 89 41 74              - mov [rcx+74],eax					<< FAR BLUR WRITE. Set to 0 to make sure there's no dof
			//DevilMayCry5.exe+194C76A7 - 48 8D 44 24 10        - lea rax,[rsp+10]
			//DevilMayCry5.exe+194C76AC - 48 8D 4C 24 08        - lea rcx,[rsp+08]
			//DevilMayCry5.exe+194C76B1 - F3 0F11 4C 24 10      - movss [rsp+10],xmm1
			//DevilMayCry5.exe+194C76B7 - 48 0F46 C1            - cmovbe rax,rcx
			//DevilMayCry5.exe+194C76BB - C7 44 24 08 17B7D138  - mov [rsp+08],38D1B717 { (0) }
			//DevilMayCry5.exe+194C76C3 - 49 8B 48 40           - mov rcx,[r8+40]
			//DevilMayCry5.exe+194C76C7 - 8B 00                 - mov eax,[rax]
			//DevilMayCry5.exe+194C76C9 - 89 81 9C000000        - mov [rcx+0000009C],eax
			//DevilMayCry5.exe+194C76CF - C3                    - ret 
			BYTE statementBytes[2] = { 0x31, 0xC0 };			//DevilMayCry5.exe+194C76A2 - 8B 00 - mov eax,[rax] -> xor eax, eax
			GameImageHooker::writeRange(aobBlocks[DOF_FAR_BLUR_STRENGTH_WRITE_KEY], statementBytes, 2);
		}
		else
		{
			// only restore the xor eax, eax with the original statement. The camera disable will enable the right dof as it will enable the write to the dof selector.
			BYTE statementBytes[2] = { 0x8B, 0x00 };			//DevilMayCry5.exe+194C76A2 - 8B 00 - mov eax,[rax]
			GameImageHooker::writeRange(aobBlocks[DOF_FAR_BLUR_STRENGTH_WRITE_KEY], statementBytes, 2);
		}
	}
}
