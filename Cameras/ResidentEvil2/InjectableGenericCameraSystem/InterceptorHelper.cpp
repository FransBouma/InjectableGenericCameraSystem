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
	void cameraWrite4Interceptor();
	void cameraWrite5Interceptor();
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
	LPBYTE _cameraWrite4InterceptionContinue = nullptr;
	LPBYTE _cameraWrite5InterceptionContinue = nullptr;
	LPBYTE _resolutionScaleReadInterceptionContinue = nullptr;
	LPBYTE _timestopReadInterceptionContinue = nullptr;
	LPBYTE _displayTypeInterceptionContinue = nullptr;
	LPBYTE _dofSelectorWriteInterceptionContinue = nullptr;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*> &aobBlocks)
	{
		aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_ADDRESS_INTERCEPT_KEY, "F3 0F 10 40 38 F3 0F 11 82 A4 00 00 00 48 83 79 18 00", 1);	
		aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE1_INTERCEPT_KEY, "F3 44 0F 10 40 34 8B 40 38 89 87 A4 00 00 00", 1);
		aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE2_INTERCEPT_KEY, "F2 0F 5A C0 F3 0F 11 87 A4 00 00 00 48 8B 43 50 48 8B 48 18", 1);
		aobBlocks[CAMERA_WRITE3_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE3_INTERCEPT_KEY, "F3 0F 10 96 88 00 00 00 | F3 0F 11 87 80 00 00 00 F3 0F 11 8F 84 00 00 00 F3 0F 11 97 88 00 00 00 48 8B 43 50", 1);
		aobBlocks[CAMERA_WRITE4_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE4_INTERCEPT_KEY, "0F 10 86 90 00 00 00 0F 11 87 90 00 00 00 48 8B 43 50 48 83 78 18 00", 1);
		// Write5 has the same AOB as write4 (both writes write the quaternion). We select the second match for write 5. The code is the same. 
		aobBlocks[CAMERA_WRITE5_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE4_INTERCEPT_KEY, "0F 10 86 90 00 00 00 0F 11 87 90 00 00 00 48 8B 43 50 48 83 78 18 00", 2);
		aobBlocks[TIMESTOP_READ_INTERCEPT_KEY] = new AOBBlock(TIMESTOP_READ_INTERCEPT_KEY, "F3 0F 10 87 84 03 00 00 0F 2F C2 F3 0F 10 9F 80 03 00 00", 1);
		aobBlocks[RESOLUTION_SCALE_INTERCEPT_KEY] = new AOBBlock(RESOLUTION_SCALE_INTERCEPT_KEY, "F3 0F 10 80 CC 11 00 00 48 8D 44 24 54 F3 41 0F 59 46 40", 1);
		aobBlocks[DISPLAYTYPE_INTERCEPT_KEY] = new AOBBlock(DISPLAYTYPE_INTERCEPT_KEY, "44 0F 29 44 24 40 44 0F 29 4C 24 30 F3 0F 11 45 10 F3 0F 11 4D 14", 1);
		aobBlocks[DOF_SELECTOR_WRITE_INTERCEPT_KEY] = new AOBBlock(DOF_SELECTOR_WRITE_INTERCEPT_KEY, "89 51 4C 85 D2 74 0E 83 EA 01 74 09 83 FA 01 75 08 88 51 50 C3", 1);
		aobBlocks[DOF_FAR_BLUR_STRENGTH_WRITE_KEY] = new AOBBlock(DOF_FAR_BLUR_STRENGTH_WRITE_KEY, "8B 00 89 81 8C 00 00 00 48 8D 44 24 10 48 8D 4C 24 08", 1);
		aobBlocks[HUD_TOGGLE_ADDRESS_KEY] = new AOBBlock(HUD_TOGGLE_ADDRESS_KEY, "48 8B 3D | ?? ?? ?? ?? 80 7F 08 00 66 C7 47 0A 00 00 0F 84", 1);
		aobBlocks[VIGNETTE_REMOVAL_ADDRESS_KEY] = new AOBBlock(VIGNETTE_REMOVAL_ADDRESS_KEY, "8B 87 3C 01 00 00 89 83 DC 00 00 00 8B 87 CC 01 00 00", 1);

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
		GameImageHooker::setHook(aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY], 0x12, &_cameraStructInterceptionContinue, &cameraStructInterceptor);
	}


	void setPostCameraStructHooks(map<string, AOBBlock*> &aobBlocks)
	{
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY], 0x0F, &_cameraWrite1InterceptionContinue, &cameraWrite1Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY], 0x10, &_cameraWrite2InterceptionContinue, &cameraWrite2Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE3_INTERCEPT_KEY], 0x18, &_cameraWrite3InterceptionContinue, &cameraWrite3Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE4_INTERCEPT_KEY], 0x0E, &_cameraWrite4InterceptionContinue, &cameraWrite4Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE5_INTERCEPT_KEY], 0x0E, &_cameraWrite5InterceptionContinue, &cameraWrite5Interceptor);
		GameImageHooker::setHook(aobBlocks[TIMESTOP_READ_INTERCEPT_KEY], 0x13, &_timestopReadInterceptionContinue, &timestopReadInterceptor);
		GameImageHooker::setHook(aobBlocks[RESOLUTION_SCALE_INTERCEPT_KEY], 0x13, &_resolutionScaleReadInterceptionContinue, &resolutionScaleReadInterceptor);
		GameImageHooker::setHook(aobBlocks[DISPLAYTYPE_INTERCEPT_KEY], 0x11, &_displayTypeInterceptionContinue, &displayTypeInterceptor);
		GameImageHooker::setHook(aobBlocks[DOF_SELECTOR_WRITE_INTERCEPT_KEY], 0x19, &_dofSelectorWriteInterceptionContinue, &dofSelectorWriteInterceptor);

		toggleVignetteOff(aobBlocks[VIGNETTE_REMOVAL_ADDRESS_KEY]);
	}


	void toggleVignetteOff(AOBBlock* vignetteWriteAddress)
	{
		//re2.exe+FFA7BD8 - 89 83 D4000000        - mov [rbx+000000D4],eax
		//re2.exe+FFA7BDE - 8B 87 38010000        - mov eax,[rdi+00000138]
		//re2.exe+FFA7BE4 - 89 83 D8000000        - mov [rbx+000000D8],eax
		//re2.exe+FFA7BEA - 8B 87 3C010000        - mov eax,[rdi+0000013C]				<< CHANGE INTO: XOR eax, eax; + 4 nops. See below:
		//re2.exe+FFA7BF0 - 89 83 DC000000        - mov [rbx+000000DC],eax
		//re2.exe+FFA7BF6 - 8B 87 CC010000        - mov eax,[rdi+000001CC]
		//re2.exe+FFA7BFC - 89 83 88010000        - mov [rbx+00000188],eax
		//re2.exe+FFA7C02 - 8B 87 DC010000        - mov eax,[rdi+000001DC]
		//re2.exe+FFA7C08 - 89 83 8C010000        - mov [rbx+0000018C],eax
		//re2.exe+FFA7C0E - 0F10 87 E0010000      - movups xmm0,[rdi+000001E0]
		//re2.exe+FFA7C15 - 0F11 83 90010000      - movups [rbx+00000190],xmm0
		//re2.exe+FFA7C1C - 0F10 87 F0010000      - movups xmm0,[rdi+000001F0]
		//
		//change that into:
		//re2.exe+FFA7BDE - 8B 87 38010000        - mov eax,[rdi+00000138]
		//re2.exe+FFA7BE4 - 89 83 D8000000        - mov [rbx+000000D8],eax
		//re2.exe+FFA7BEA - 31 C0                 - xor eax,eax
		//re2.exe+FFA7BEC - 90                    - nop 
		//re2.exe+FFA7BED - 90                    - nop 
		//re2.exe+FFA7BEE - 90                    - nop 
		//re2.exe+FFA7BEF - 90                    - nop 
		//re2.exe+FFA7BF0 - 89 83 DC000000        - mov [rbx+000000DC],eax
		//re2.exe+FFA7BF6 - 8B 87 CC010000        - mov eax,[rdi+000001CC]
		//re2.exe+FFA7BFC - 89 83 88010000        - mov [rbx+00000188],eax
		//re2.exe+FFA7C02 - 8B 87 DC010000        - mov eax,[rdi+000001DC]

		// we're not going to re-enable it, so just write over it. xor eax, eax is 31, c0, plus 4 nops
		BYTE statementBytes[6] = { 0x31, 0xC0, 0x90, 0x90, 0x90, 0x90 };			//re2.exe+FFA7BEA - 8B 87 3C010000        - mov eax,[rdi+0000013C]				<< CHANGE INTO: XOR eax, eax; + 4 nops.
		GameImageHooker::writeRange(vignetteWriteAddress, statementBytes, 6);
	}


	// if hideHud is false, we'll place a 1 at the flag position which signals that the rendering of the hud was successful, which shows the hud.
	// otherwise we'll place a 0 there which makes the hud disappear.
	void toggleHud(map<string, AOBBlock*> &aobBlocks, bool hideHud)
	{
		//re2.exe+F5AD6D2 - 49 89 D5              - mov r13,rdx
		//re2.exe+F5AD6D5 - 80 78 08 00           - cmp byte ptr [rax+08],00 { 0 }
		//re2.exe+F5AD6D9 - 0F84 E3010000         - je re2.exe+F5AD8C2
		//re2.exe+F5AD6DF - 48 89 7C 24 30        - mov [rsp+30],rdi
		//re2.exe+F5AD6E4 - 48 8B 3D FDDCBBF7     - mov rdi,[re2.exe+716B3E8] { (0) }			<< READ ADDRESS, add 8 for hud toggle. Write 0 to toggle hud off, 1 for normal operation.
		//re2.exe+F5AD6EB - 80 7F 08 00           - cmp byte ptr [rdi+08],00 { 0 }
		//re2.exe+F5AD6EF - 66 C7 47 0A 0000      - mov word ptr [rdi+0A],0000 { 0 }
		//re2.exe+F5AD6F5 - 0F84 9E010000         - je re2.exe+F5AD899
		//re2.exe+F5AD6FB - 48 8B 05 16EAACF7     - mov rax,[re2.exe+707C118] { (2B51B380) }
		//re2.exe+F5AD702 - 48 8B 48 48           - mov rcx,[rax+48]
		//re2.exe+F5AD706 - 48 85 C9              - test rcx,rcx
		//re2.exe+F5AD709 - 74 19                 - je re2.exe+F5AD724
		//re2.exe+F5AD70B - 80 B9 11020000 00     - cmp byte ptr [rcx+00000211],00 { 0 }
		//re2.exe+F5AD712 - 75 09                 - jne re2.exe+F5AD71D
		//re2.exe+F5AD714 - E8 773D81F2           - call re2.exe+1DC1490
		//re2.exe+F5AD719 - 84 C0                 - test al,al
		LPBYTE drawComponentStructAddress = (LPBYTE)*(__int64*)Utils::calculateAbsoluteAddress(aobBlocks[HUD_TOGGLE_ADDRESS_KEY], 4); //re2.exe+F5AD6E4 - 48 8B 3D FDDCBBF7     - mov rdi,[re2.exe+716B3E8]
		BYTE* drawComponentResultAddress = reinterpret_cast<BYTE*>(drawComponentStructAddress + HUD_TOGGLE_OFFSET_IN_STRUCT);
		*drawComponentResultAddress = hideHud ? (BYTE)0 : (BYTE)1;
	}


	// Toggles the in-game dof. it selects dof '1' and sets far blur to 0. 
	void toggleInGameDoFOff(map<string, AOBBlock*> &aobBlocks, bool switchOff)
	{
		if (switchOff)
		{
			// first select the dof, type 1. 
			CameraManipulator::selectDoF(1);

			// write xor eax, eax at the far blur strength value read, so it's always 0
			//re2.exe+102BCEDC - 48 0F46 C1            - cmovbe rax,rcx
			//re2.exe+102BCEE0 - F3 0F11 4C 24 10      - movss [rsp+10],xmm1
			//re2.exe+102BCEE6 - 49 8B 48 38           - mov rcx,[r8+38]
			//re2.exe+102BCEEA - C7 44 24 08 17B7D138  - mov [rsp+08],38D1B717 { (-11776) }
			//re2.exe+102BCEF2 - 8B 00                 - mov eax,[rax]							<<< replace with XOR eax, eax
			//re2.exe+102BCEF4 - 89 81 8C000000        - mov [rcx+0000008C],eax					<< FAR BLUR WRITE. Set to 0 to make sure there's no dof
			//re2.exe+102BCEFA - 48 8D 44 24 10        - lea rax,[rsp+10]
			//re2.exe+102BCEFF - 48 8D 4C 24 08        - lea rcx,[rsp+08]
			//re2.exe+102BCF04 - F3 0F11 4C 24 10      - movss [rsp+10],xmm1
			//re2.exe+102BCF0A - 48 0F46 C1            - cmovbe rax,rcx
			//re2.exe+102BCF0E - C7 44 24 08 17B7D138  - mov [rsp+08],38D1B717 { (-11776) }
			//re2.exe+102BCF16 - 49 8B 48 40           - mov rcx,[r8+40]
			//re2.exe+102BCF1A - 8B 00                 - mov eax,[rax]
			//re2.exe+102BCF1C - 89 81 B4000000        - mov [rcx+000000B4],eax
			//re2.exe+102BCF22 - C3                    - ret 
			BYTE statementBytes[2] = { 0x31, 0xC0 };			//re2.exe+102BCEF2 - 8B 00                 - mov eax,[rax]							<<< replace with XOR eax, eax
			GameImageHooker::writeRange(aobBlocks[DOF_FAR_BLUR_STRENGTH_WRITE_KEY], statementBytes, 2);
		}
		else
		{
			// only restore the xor eax, eax with the original statement. The camera disable will enable the right dof as it will enable the write to the dof selector.
			BYTE statementBytes[2] = { 0x8B, 0x00 };			//re2.exe+102BCEF2 - 8B 00                 - mov eax,[rax]
			GameImageHooker::writeRange(aobBlocks[DOF_FAR_BLUR_STRENGTH_WRITE_KEY], statementBytes, 2);
		}
	}
}
