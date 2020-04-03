////////////////////////////////////////////////////////////////////////////////////////////////////////
// Part of Injectable Generic Camera System
// Copyright(c) 2020, Frans Bouma
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
#include "MessageHandler.h"
#include "CameraManipulator.h"
#include "Globals.h"

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
	LPBYTE _cameraWrite6InterceptionContinue = nullptr;
	LPBYTE _resolutionScaleReadInterceptionContinue = nullptr;
	LPBYTE _timestopReadInterceptionContinue = nullptr;
	LPBYTE _displayTypeInterceptionContinue = nullptr;
	LPBYTE _dofSelectorWriteInterceptionContinue = nullptr;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*> &aobBlocks)
	{
		aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_ADDRESS_INTERCEPT_KEY, "8B 40 38 89 82 B4 00 00 00 48 83 79 18 00", 1);
		aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE1_INTERCEPT_KEY, "F3 44 0F 10 48 34 8B 40 38 89 87 B4 00 00 00 48 8B 43 50", 1);
		aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE2_INTERCEPT_KEY, "F2 0F 5A C0 F3 0F 11 87 B4 00 00 00 48 8B 43 50", 1);
		aobBlocks[CAMERA_WRITE3_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE3_INTERCEPT_KEY, "F3 0F 10 54 24 58 | F3 0F 11 87 80 00 00 00 F3 0F 11 8F 84 00 00 00 F3 0F 11 97 88 00 00 00", 1);
		aobBlocks[CAMERA_WRITE4_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE4_INTERCEPT_KEY, "F3 0F 10 96 88 00 00 00 | F3 0F 11 87 80 00 00 00 F3 0F 11 8F 84 00 00 00 F3 0F 11 97 88 00 00 00", 1);
		aobBlocks[CAMERA_WRITE5_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE5_INTERCEPT_KEY, "0F 10 86 90 00 00 00 0F 11 87 A0 00 00 00 48 8B 43 50", 1);
		aobBlocks[CAMERA_WRITE6_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE6_INTERCEPT_KEY, "F3 0F 11 B7 A0 00 00 00 F3 44 0F 11 87 A4 00 00 00 F3 44 0F 11 97 A8 00 00 00", 1);
		aobBlocks[TIMESTOP_READ_INTERCEPT_KEY] = new AOBBlock(TIMESTOP_READ_INTERCEPT_KEY, "F3 0F 59 8B 80 03 00 00 0F 5A C0 F3 0F 11 8B 84 03 00 00", 1);
		aobBlocks[RESOLUTION_SCALE_INTERCEPT_KEY] = new AOBBlock(RESOLUTION_SCALE_INTERCEPT_KEY, "F3 0F 59 80 B4 14 00 00 48 8D 44 24 2C 0F 2F C7", 1);
		aobBlocks[DISPLAYTYPE_INTERCEPT_KEY] = new AOBBlock(DISPLAYTYPE_INTERCEPT_KEY, "44 0F 29 44 24 60 44 0F 29 4C 24 50 F3 0F 11 45 10", 1);
		aobBlocks[DOF_SELECTOR_WRITE_INTERCEPT_KEY] = new AOBBlock(DOF_SELECTOR_WRITE_INTERCEPT_KEY, "89 51 4C 85 D2 74 0E 83 EA 01 74 09 83 FA 01 75 08 88 51 50 C3", 1);
		aobBlocks[DOF_FAR_BLUR_STRENGTH_WRITE_KEY] = new AOBBlock(DOF_FAR_BLUR_STRENGTH_WRITE_KEY, "8B 00 89 82 C4 01 00 00 48 8D 44 24 10 48 0F 46 C1", 1);
		aobBlocks[HUD_TOGGLE_ADDRESS_KEY] = new AOBBlock(HUD_TOGGLE_ADDRESS_KEY, "48 8B 3D | ?? ?? ?? ?? 80 7F 08 00 66 C7 47 0A 00 00", 1);
		aobBlocks[VIGNETTE_REMOVAL_ADDRESS_KEY] = new AOBBlock(VIGNETTE_REMOVAL_ADDRESS_KEY, "8B 87 3C 01 00 00 89 83 DC 00 00 00 8B 87 CC 01 00 00", 1);
		aobBlocks[SHARPENING_DISABLE_ADDRESS_KEY] = new AOBBlock(SHARPENING_DISABLE_ADDRESS_KEY, "74 05 0F 57 C0 EB 2C 8B 87 A4 01 00 00", 1);

		map<string, AOBBlock*>::iterator it;
		bool result = true;
		for (it = aobBlocks.begin(); it != aobBlocks.end(); it++)
		{
			result &= it->second->scan(hostImageAddress, hostImageSize);
		}
		if (result)
		{
			MessageHandler::logLine("All interception offsets found.");
		}
		else
		{
			MessageHandler::logError("One or more interception offsets weren't found: tools aren't compatible with this game's version.");
		}
	}


	void setCameraStructInterceptorHook(map<string, AOBBlock*> &aobBlocks)
	{
		GameImageHooker::setHook(aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY], 0x0E, &_cameraStructInterceptionContinue, &cameraStructInterceptor);
	}


	void setPostCameraStructHooks(map<string, AOBBlock*> &aobBlocks)
	{
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY], 0x0F, &_cameraWrite1InterceptionContinue, &cameraWrite1Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY], 0x10, &_cameraWrite2InterceptionContinue, &cameraWrite2Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE3_INTERCEPT_KEY], 0x18, &_cameraWrite3InterceptionContinue, &cameraWrite3Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE4_INTERCEPT_KEY], 0x18, &_cameraWrite4InterceptionContinue, &cameraWrite4Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE5_INTERCEPT_KEY], 0x0E, &_cameraWrite5InterceptionContinue, &cameraWrite5Interceptor);
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE6_INTERCEPT_KEY], 0x33, &_cameraWrite6InterceptionContinue, &cameraWrite6Interceptor);
		GameImageHooker::setHook(aobBlocks[TIMESTOP_READ_INTERCEPT_KEY], 0x13, &_timestopReadInterceptionContinue, &timestopReadInterceptor);
		GameImageHooker::setHook(aobBlocks[RESOLUTION_SCALE_INTERCEPT_KEY], 0x10, &_resolutionScaleReadInterceptionContinue, &resolutionScaleReadInterceptor);
		GameImageHooker::setHook(aobBlocks[DISPLAYTYPE_INTERCEPT_KEY], 0x11, &_displayTypeInterceptionContinue, &displayTypeInterceptor);
		GameImageHooker::setHook(aobBlocks[DOF_SELECTOR_WRITE_INTERCEPT_KEY], 0x19, &_dofSelectorWriteInterceptionContinue, &dofSelectorWriteInterceptor);

		toggleVignetteOff(aobBlocks);
	}


	void toggleVignetteOff(map<string, AOBBlock*>& aobBlocks)
	{
		//re3demo.exe+1AB007AD - 89 83 D4000000        - mov [rbx+000000D4],eax
		//re3demo.exe+1AB007B3 - 8B 87 38010000        - mov eax,[rdi+00000138]
		//re3demo.exe+1AB007B9 - 89 83 D8000000        - mov [rbx+000000D8],eax
		//re3demo.exe+1AB007BF - 8B 87 3C010000        - mov eax,[rdi+0000013C]				<< Replace with xor eax, eax + 4 nops
		//re3demo.exe+1AB007C5 - 89 83 DC000000        - mov [rbx+000000DC],eax
		//re3demo.exe+1AB007CB - 8B 87 CC010000        - mov eax,[rdi+000001CC]
		//re3demo.exe+1AB007D1 - 89 83 88010000        - mov [rbx+00000188],eax
		//re3demo.exe+1AB007D7 - 8B 87 DC010000        - mov eax,[rdi+000001DC]
		//re3demo.exe+1AB007DD - 89 83 8C010000        - mov [rbx+0000018C],eax
		//re3demo.exe+1AB007E3 - 0F10 87 E0010000      - movups xmm0,[rdi+000001E0]
		//re3demo.exe+1AB007EA - 0F11 83 90010000      - movups [rbx+00000190],xmm0
		//re3demo.exe+1AB007F1 - 0F10 87 F0010000      - movups xmm0,[rdi+000001F0]
		//re3demo.exe+1AB007F8 - 0F11 83 A0010000      - movups [rbx+000001A0],xmm0
		// change that too
		//re3demo.exe+1AB007B9 - 89 83 D8000000        - mov [rbx+000000D8],eax
		//re3demo.exe+1AB007BF - 31 C0                 - xor eax,eax
		//re3demo.exe+1AB007C1 - 90					   - nop
		//re3demo.exe+1AB007C2 - 90					   - nop
		//re3demo.exe+1AB007C3 - 90					   - nop
		//re3demo.exe+1AB007C4 - 90					   - nop
		//re3demo.exe+1AB007C5 - 89 83 DC000000        - mov [rbx+000000DC],eax
		//re3demo.exe+1AB007CB - 8B 87 CC010000        - mov eax,[rdi+000001CC]
		//re3demo.exe+1AB007D1 - 89 83 88010000        - mov [rbx+00000188],eax
		//re3demo.exe+1AB007D7 - 8B 87 DC010000        - mov eax,[rdi+000001DC]
		//re3demo.exe+1AB007DD - 89 83 8C010000        - mov [rbx+0000018C],eax
		//re3demo.exe+1AB007E3 - 0F10 87 E0010000      - movups xmm0,[rdi+000001E0]
		//re3demo.exe+1AB007EA - 0F11 83 90010000      - movups [rbx+00000190],xmm0
		//re3demo.exe+1AB007F1 - 0F10 87 F0010000      - movups xmm0,[rdi+000001F0]
		//re3demo.exe+1AB007F8 - 0F11 83 A0010000      - movups [rbx+000001A0],xmm0
		// we're not going to re-enable it, so just write over it. xor eax, eax is 31, c0, plus 4 nops
		BYTE statementBytes[6] = { 0x31, 0xC0, 0x90, 0x90, 0x90, 0x90 };			//re3demo.exe+1AB007BF - 8B 87 3C010000        - mov eax,[rdi+0000013C]				<< Replace with xor eax, eax + 4 nops
		GameImageHooker::writeRange(aobBlocks[VIGNETTE_REMOVAL_ADDRESS_KEY], statementBytes, 6);
	}

	
	void toggleSharpening(map<string, AOBBlock*>& aobBlocks, bool enableSharpening)
	{
		//re3demo.exe+1AEB6B4E - 0F84 BC010000         - je re3demo.exe+1AEB6D10
		//re3demo.exe+1AEB6B54 - 48 8B 8D 904A0000     - mov rcx,[rbp+00004A90]
		//re3demo.exe+1AEB6B5B - 48 89 5C 24 60        - mov [rsp+60],rbx
		//re3demo.exe+1AEB6B60 - E8 DB844EE7           - call re3demo.exe+239F040
		//re3demo.exe+1AEB6B65 - 80 BF 0D010000 00     - cmp byte ptr [rdi+0000010D],00 { 0 }
		//re3demo.exe+1AEB6B6C - 48 89 C3              - mov rbx,rax
		//re3demo.exe+1AEB6B6F - 74 05                 - je re3demo.exe+1AEB6B76				<< NOP so it ends up using the jmp to BA2
		//re3demo.exe+1AEB6B71 - 0F57 C0               - xorps xmm0,xmm0
		//re3demo.exe+1AEB6B74 - EB 2C                 - jmp re3demo.exe+1AEB6BA2
		//re3demo.exe+1AEB6B76 - 8B 87 A4010000        - mov eax,[rdi+000001A4]				<< AA type check. TAA uses '4'. 
		//re3demo.exe+1AEB6B7C - 83 F8 03              - cmp eax,03 { 3 }
		//re3demo.exe+1AEB6B7F - 75 0A                 - jne re3demo.exe+1AEB6B8B
		//re3demo.exe+1AEB6B81 - F3 0F10 05 47FEABE9   - movss xmm0,[re3demo.exe+49769D0] { (0,33) }
	
		Settings& currentSettings = Globals::instance().settings();
		if(currentSettings.enableSharpening || enableSharpening)
		{
			// restore the jump
			BYTE statementBytes[2] = { 0x74, 0x05 };			//re3demo.exe+1AEB6B6F - 74 05                 - je re3demo.exe+1AEB6B76				<< NOP so it ends up using the jmp to BA2
			GameImageHooker::writeRange(aobBlocks[SHARPENING_DISABLE_ADDRESS_KEY], statementBytes, 2);

		}
		else
		{
			// nop the jump.
			GameImageHooker::nopRange(aobBlocks[SHARPENING_DISABLE_ADDRESS_KEY], 2);
		}
	}


	void toggleHud(map<string, AOBBlock*>& aobBlocks, bool hudVisible)
	{
		//re3demo.exe+19BAB3F2 - 49 89 D5              - mov r13,rdx
		//re3demo.exe+19BAB3F5 - 80 78 08 00           - cmp byte ptr [rax+08],00 { 0 }
		//re3demo.exe+19BAB3F9 - 0F84 E3010000         - je re3demo.exe+19BAB5E2
		//re3demo.exe+19BAB3FF - 48 89 7C 24 30        - mov [rsp+30],rdi
		//re3demo.exe+19BAB404 - 48 8B 3D ED692BEF     - mov rdi,[re3demo.exe+8E61DF8] 			<<< Read address, add 8, 0 == no HUD, 1 == HUD
		//re3demo.exe+19BAB40B - 80 7F 08 00           - cmp byte ptr [rdi+08],00 { 0 }
		//re3demo.exe+19BAB40F - 66 C7 47 0A 0000      - mov word ptr [rdi+0A],0000 { 0 }
		//re3demo.exe+19BAB415 - C6 47 0C 00           - mov byte ptr [rdi+0C],00 { 0 }
		//re3demo.exe+19BAB419 - 0F84 9A010000         - je re3demo.exe+19BAB5B9
		//re3demo.exe+19BAB41F - 48 8B 05 CA6115EF     - mov rax,[re3demo.exe+8D015F0] { (128) }
		//re3demo.exe+19BAB426 - 48 8B 48 50           - mov rcx,[rax+50]
		//re3demo.exe+19BAB42A - 48 85 C9              - test rcx,rcx
		//re3demo.exe+19BAB42D - 74 19                 - je re3demo.exe+19BAB448
		//re3demo.exe+19BAB42F - 80 B9 11020000 00     - cmp byte ptr [rcx+00000211],00 { 0 }
		LPBYTE drawComponentStructAddress = (LPBYTE) * (__int64*)Utils::calculateAbsoluteAddress(aobBlocks[HUD_TOGGLE_ADDRESS_KEY], 4); //re3demo.exe+19BAB404 - 48 8B 3D ED692BEF     - mov rdi,[re3demo.exe+8E61DF8] 			<<< Read address, add 8, 0 == no HUD, 1 == HUD
		BYTE* drawComponentResultAddress = reinterpret_cast<BYTE*>(drawComponentStructAddress + HUD_TOGGLE_OFFSET_IN_STRUCT);
		*drawComponentResultAddress = hudVisible ? (BYTE)1 : (BYTE)0;
	}


	// Toggles the in-game dof. it selects dof '1' and sets far blur to 0. 
	void toggleInGameDoFOff(map<string, AOBBlock*>& aobBlocks, bool switchOff)
	{
		if (switchOff)
		{
			// first select the dof, type 1. 
			CameraManipulator::selectDoF(1);

			// write xor eax, eax at the far blur strength value read, so it's always 0
			//re3demo.exe+1AEF8CDA - C7 44 24 08 17B7D138  - mov [rsp+08],38D1B717 { (0) }
			//re3demo.exe+1AEF8CE2 - 48 8D 4C 24 08        - lea rcx,[rsp+08]
			//re3demo.exe+1AEF8CE7 - 48 8D 44 24 10        - lea rax,[rsp+10]
			//re3demo.exe+1AEF8CEC - 49 8B 50 30           - mov rdx,[r8+30]
			//re3demo.exe+1AEF8CF0 - F3 0F11 4C 24 10      - movss [rsp+10],xmm1
			//re3demo.exe+1AEF8CF6 - 48 0F46 C1            - cmovbe rax,rcx
			//re3demo.exe+1AEF8CFA - 48 8D 4C 24 08        - lea rcx,[rsp+08]
			//re3demo.exe+1AEF8CFF - 8B 00                 - mov eax,[rax]							<<< replace with XOR eax, eax
			//re3demo.exe+1AEF8D01 - 89 82 C4010000        - mov [rdx+000001C4],eax					<< FAR BLUR WRITE. Set to 0 to make sure there's no dof
			//re3demo.exe+1AEF8D07 - 48 8D 44 24 10        - lea rax,[rsp+10]
			//re3demo.exe+1AEF8D0C - 48 0F46 C1            - cmovbe rax,rcx
			//re3demo.exe+1AEF8D10 - F3 0F11 4C 24 10      - movss [rsp+10],xmm1
			//re3demo.exe+1AEF8D16 - 49 8B 48 38           - mov rcx,[r8+38]
			//re3demo.exe+1AEF8D1A - C7 44 24 08 17B7D138  - mov [rsp+08],38D1B717 { (0) }
			//re3demo.exe+1AEF8D22 - 8B 00                 - mov eax,[rax]
			//re3demo.exe+1AEF8D24 - 89 41 74              - mov [rcx+74],eax
			//re3demo.exe+1AEF8D27 - 48 8D 44 24 10        - lea rax,[rsp+10]
			//re3demo.exe+1AEF8D2C - 48 8D 4C 24 08        - lea rcx,[rsp+08]
			BYTE statementBytes[2] = { 0x31, 0xC0 };			//re3demo.exe+1AEF8CFF - 8B 00                 - mov eax,[rax]							<<< replace with XOR eax, eax
			GameImageHooker::writeRange(aobBlocks[DOF_FAR_BLUR_STRENGTH_WRITE_KEY], statementBytes, 2);
		}
		else
		{
			// only restore the xor eax, eax with the original statement. The camera disable will enable the right dof as it will enable the write to the dof selector.
			BYTE statementBytes[2] = { 0x8B, 0x00 };			//re3demo.exe+1AEF8CFF - 8B 00                 - mov eax,[rax]							<<< replace with XOR eax, eax
			GameImageHooker::writeRange(aobBlocks[DOF_FAR_BLUR_STRENGTH_WRITE_KEY], statementBytes, 2);
		}
	}
}
