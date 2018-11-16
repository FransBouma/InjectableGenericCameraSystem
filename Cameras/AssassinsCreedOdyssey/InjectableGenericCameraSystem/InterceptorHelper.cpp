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
	void fovReadInterceptor();
	void resolutionScaleReadInterceptor();
	void todWriteInterceptor();
	void timestopReadInterceptor();
}

// external addresses used in asm.
extern "C" {
	LPBYTE _cameraStructInterceptionContinue = nullptr;
	LPBYTE _cameraWrite1InterceptionContinue = nullptr;
	LPBYTE _cameraWrite2InterceptionContinue = nullptr;
	LPBYTE _fovReadInterceptionContinue = nullptr;
	LPBYTE _resolutionScaleReadInterceptionContinue = nullptr;
	LPBYTE _todWriteInterceptionContinue = nullptr;
	LPBYTE _timestopReadInterceptionContinue = nullptr;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*> &aobBlocks)
	{
		aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_ADDRESS_INTERCEPT_KEY, "66 0F 7F 86 90 00 00 00 66 0F 7F 0F", 1);	
		aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE1_INTERCEPT_KEY, "0F 29 02 0F 28 71 20 41 0F 28 10 41 0F 28 38 0F 28 E2", 1);
		aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE2_INTERCEPT_KEY, "41 0F 29 38 F3 0F 10 41 30 F3 41 0F 58 01 0F 28 3C 24 F3 41 0F 11 01", 1);
		aobBlocks[TIMESTOP_READ_INTERCEPT_KEY] = new AOBBlock(TIMESTOP_READ_INTERCEPT_KEY, "48 89 54 24 28 48 8B 57 18 83 E1 10 4C 31 C1 41 0F 29 7B A8", 1);
		aobBlocks[TOD_WRITE_INTERCEPT_KEY] = new AOBBlock(TOD_WRITE_INTERCEPT_KEY, "F3 0F 58 00 F3 0F 11 00 48 8B 81 60 02 00 00 F3 0F 10 08", 1);
		aobBlocks[RESOLUTION_SCALE_INTERCEPT_KEY] = new AOBBlock(RESOLUTION_SCALE_INTERCEPT_KEY, "48 8B 70 60 48 8B 82 48 02 00 00 48 89 B4 24 B8 00 00 00", 1);
		aobBlocks[PAUSE_FUNCTION_LOCATION_KEY] = new AOBBlock(PAUSE_FUNCTION_LOCATION_KEY, "53 48 83 EC 20 48 89 CB 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? FF 83 6C 18 00 00 83 BB 6C 18 00 00 01", 1);
		aobBlocks[UNPAUSE_FUNCTION_LOCATION_KEY] = new AOBBlock(UNPAUSE_FUNCTION_LOCATION_KEY, "53 48 83 EC 20 48 89 CB 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 83 ?? ?? ?? ?? 85 C0", 1);
		aobBlocks[HUD_RENDER_INTERCEPT_KEY] = new AOBBlock(HUD_RENDER_INTERCEPT_KEY, "48 89 E0 48 89 58 08 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 B0 48 81 EC ?? ?? ?? ?? 0F 29 70 B8 0F 29 78 A8", 1);
		aobBlocks[PHOTOMODE_RANGE_DISABLE_KEY] = new AOBBlock(PHOTOMODE_RANGE_DISABLE_KEY, "F3 41 0F 5D D0 0F 28 C1 0F C6 D2 00 41 0F 59 D2", 1);
		aobBlocks[DOF_ENABLE_WRITE_LOCATION_KEY] = new AOBBlock(DOF_ENABLE_WRITE_LOCATION_KEY, "88 83 11 01 00 00 E8 ?? ?? ?? ?? 88 83 13 01 00 00 84 C0", 1);
		aobBlocks[AR_LIMIT1_LOCATION_KEY] = new AOBBlock(AR_LIMIT1_LOCATION_KEY, "74 ?? F3 44 0F 10 05 ?? ?? ?? ?? EB ?? F3 44 0F 10 05 ?? ?? ?? ?? 48 8D 0D", 1);
		aobBlocks[AR_LIMIT2_LOCATION_KEY] = new AOBBlock(AR_LIMIT2_LOCATION_KEY, "F3 44 0F 59 CF 41 0F 28 D0 F3 0F 5C C2 0F 28 FA 44 0F 28 F2", 1);

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
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE2_INTERCEPT_KEY], 0x17, &_cameraWrite2InterceptionContinue, &cameraWrite2Interceptor);
		GameImageHooker::setHook(aobBlocks[TOD_WRITE_INTERCEPT_KEY], 0x0F, &_todWriteInterceptionContinue, &todWriteInterceptor);
		GameImageHooker::setHook(aobBlocks[TIMESTOP_READ_INTERCEPT_KEY], 0x14, &_timestopReadInterceptionContinue, &timestopReadInterceptor);
		GameImageHooker::setHook(aobBlocks[RESOLUTION_SCALE_INTERCEPT_KEY], 0x17, &_resolutionScaleReadInterceptionContinue, &resolutionScaleReadInterceptor);
		CameraManipulator::setPauseUnpauseGameFunctionPointers(aobBlocks[PAUSE_FUNCTION_LOCATION_KEY]->absoluteAddress(), aobBlocks[UNPAUSE_FUNCTION_LOCATION_KEY]->absoluteAddress());
		disablePhotomodeRangeLimit(aobBlocks);
		disableARLimits(aobBlocks);
	}


	void disableARLimits(map<string, AOBBlock*> &aobBlocks)
	{
		// (v1.0.7)
		//ACOdyssey.exe+B053CFD - F3 44 0F10 9F 30070000  - movss xmm11,[rdi+00000730]
		//ACOdyssey.exe+B053D06 - F3 0F10 35 66A1D9F8   - movss xmm6,[ACOdyssey.exe+3DEDE74] { [1.00] }
		//ACOdyssey.exe+B053D0E - F3 44 0F5F 1D 6DB7D9F8  - maxss xmm11,[ACOdyssey.exe+3DEF484] { [0.10] }
		//ACOdyssey.exe+B053D17 - 0F28 FE               - movaps xmm7,xmm6
		//ACOdyssey.exe+B053D1A - E8 8142B4F5           - call ACOdyssey.exe+B97FA0
		//ACOdyssey.exe+B053D1F - 84 C0                 - test al,al
		//ACOdyssey.exe+B053D21 - 74 0B                 - je ACOdyssey.exe+B053D2E								>> Change to JMP to fix wide screen AR larger than monitor.
		//ACOdyssey.exe+B053D23 - F3 44 0F10 05 9C4CDEF8  - movss xmm8,[ACOdyssey.exe+3E389C8] { [1.78] }
		//ACOdyssey.exe+B053D2C - EB 09                 - jmp ACOdyssey.exe+B053D37
		//ACOdyssey.exe+B053D2E - F3 44 0F10 05 B54CDEF8  - movss xmm8,[ACOdyssey.exe+3E389EC] { [5.33] }
		//ACOdyssey.exe+B053D37 - 48 8D 0D 62B43AFA     - lea rcx,[ACOdyssey.exe+53FF1A0] { [00000000] }
		//ACOdyssey.exe+B053D3E - E8 3D30B2F5           - call ACOdyssey.exe+B76D80
		//ACOdyssey.exe+B053D43 - 0F2F 05 5AB9D9F8      - comiss xmm0,[ACOdyssey.exe+3DEF6A4] { [1.60] }
		//ACOdyssey.exe+B053D4A - 73 0D                 - jae ACOdyssey.exe+B053D59
		//ACOdyssey.exe+B053D4C - 0F28 F8               - movaps xmm7,xmm0
		//ACOdyssey.exe+B053D4F - F3 0F59 3D 594CDEF8   - mulss xmm7,[ACOdyssey.exe+3E389B0] { [0.63] }
		//ACOdyssey.exe+B053D57 - EB 0E                 - jmp ACOdyssey.exe+B053D67
		BYTE statementBytes[1] = { 0xEB };						// ACOdyssey.exe+B053D21 - 74 0B              - je ACOdyssey.exe+B053D2E
		GameImageHooker::writeRange(aobBlocks[AR_LIMIT1_LOCATION_KEY], statementBytes, 1);

		// v1.0.7
		//ACOdyssey.exe+B053D79 - 8B 43 08              - mov eax,[rbx+08]
		//ACOdyssey.exe+B053D7C - 0F28 C5               - movaps xmm0,xmm5
		//ACOdyssey.exe+B053D7F - F3 44 0F59 C6         - mulss xmm8,xmm6
		//ACOdyssey.exe+B053D84 - F3 0F10 74 24 5C      - movss xmm6,[rsp+5C]
		//ACOdyssey.exe+B053D8A - 44 0F28 CE            - movaps xmm9,xmm6
		//ACOdyssey.exe+B053D8E - 0F28 CE               - movaps xmm1,xmm6
		//ACOdyssey.exe+B053D91 - F3 44 0F59 CF         - mulss xmm9,xmm7		>> NOP to have custom AR 
		//ACOdyssey.exe+B053D96 - 41 0F28 D0            - movaps xmm2,xmm8
		//ACOdyssey.exe+B053D9A - F3 0F5C C2            - subss xmm0,xmm2
		//ACOdyssey.exe+B053D9E - 0F28 FA               - movaps xmm7,xmm2
		//ACOdyssey.exe+B053DA1 - 44 0F28 F2            - movaps xmm14,xmm2
		//ACOdyssey.exe+B053DA5 - 41 0F28 D9            - movaps xmm3,xmm9
		//ACOdyssey.exe+B053DA9 - F3 0F5C CB            - subss xmm1,xmm3

		GameImageHooker::nopRange(aobBlocks[AR_LIMIT2_LOCATION_KEY], 5);		//ACOdyssey.exe+B053D91 - F3 44 0F59 CF         - mulss xmm9,xmm7		>> NOP to have custom AR 
	}


	void disablePhotomodeRangeLimit(map<string, AOBBlock*> &aobBlocks)
	{
		// v1.0.7:
		// ACOdyssey.exe+2A8FCCD - 41 0F28 C2            - movaps xmm0,xmm10
		// ACOdyssey.exe+2A8FCD1 - 66 41 0F3A40 C2 7F    - dpps xmm0,xmm10,7F
		// ACOdyssey.exe+2A8FCD8 - 0F51 C8               - sqrtps xmm1,xmm0
		// ACOdyssey.exe+2A8FCDB - 0F28 D1               - movaps xmm2,xmm1
		// ACOdyssey.exe+2A8FCDE - F3 41 0F5F CB         - maxss xmm1,xmm11
		// ACOdyssey.exe+2A8FCE3 - F3 41 0F5D D0         - minss xmm2,xmm8					<< simply NOP the statement. 
		// ACOdyssey.exe+2A8FCE8 - 0F28 C1               - movaps xmm0,xmm1
		// ACOdyssey.exe+2A8FCEB - 0FC6 D2 00            - shufps xmm2,xmm200 { 0 }
		// ACOdyssey.exe+2A8FCEF - 41 0F59 D2            - mulps xmm2,xmm10
		// ACOdyssey.exe+2A8FCF3 - 0FC6 C0 00            - shufps xmm0,xmm000 { 0 }
		// ACOdyssey.exe+2A8FCF7 - 0F5E D0               - divps xmm2,xmm0
		// ACOdyssey.exe+2A8FCFA - F3 0F10 87 FC000000   - movss xmm0,[rdi+000000FC]
		GameImageHooker::nopRange(aobBlocks[PHOTOMODE_RANGE_DISABLE_KEY], 5);
	}


	// if enabled is true, we'll place a 'ret' at the start of the code block, making the game skip rendering any hud element. If false, we'll reset
	// the original first statement so code will proceed as normal. 
	void toggleHudRenderState(map<string, AOBBlock*> &aobBlocks, bool enabled)
	{
		// start of HUD widget render code
		//ACOdyssey.exe+C0DFCC0 - 48 8B C4              - mov rax,rsp					<< Place RET here to hide hud completely. Restore-> hud is there again.
		//ACOdyssey.exe+C0DFCC3 - 48 89 58 08           - mov [rax+08],rbx
		//ACOdyssey.exe+C0DFCC7 - 55                    - push rbp
		//ACOdyssey.exe+C0DFCC8 - 56                    - push rsi
		//ACOdyssey.exe+C0DFCC9 - 57                    - push rdi
		//ACOdyssey.exe+C0DFCCA - 41 54                 - push r12
		//ACOdyssey.exe+C0DFCCC - 41 55                 - push r13
		//ACOdyssey.exe+C0DFCCE - 41 56                 - push r14
		//ACOdyssey.exe+C0DFCD0 - 41 57                 - push r15
		//ACOdyssey.exe+C0DFCD2 - 48 8D 6C 24 B0        - lea rbp,[rsp-50]
		//ACOdyssey.exe+C0DFCD7 - 48 81 EC 50010000     - sub rsp,00000150 { 336 }
		//ACOdyssey.exe+C0DFCDE - 0F29 70 B8            - movaps [rax-48],xmm6
		//ACOdyssey.exe+C0DFCE2 - 0F29 78 A8            - movaps [rax-58],xmm7
		//ACOdyssey.exe+C0DFCE6 - 44 0F29 40 98         - movaps [rax-68],xmm8
		//ACOdyssey.exe+C0DFCEB - 48 8B 05 2E3A1FF9     - mov rax,[ACOdyssey.exe+52D3720] { [53E4231C] }
		//ACOdyssey.exe+C0DFCF2 - 48 31 E0              - xor rax,rsp
		//ACOdyssey.exe+C0DFCF5 - 48 89 45 10           - mov [rbp+10],rax
		//ACOdyssey.exe+C0DFCF9 - 4C 8B 62 18           - mov r12,[rdx+18]
		//ACOdyssey.exe+C0DFCFD - 48 89 D3              - mov rbx,rdx
		//ACOdyssey.exe+C0DFD00 - 48 8B 32              - mov rsi,[rdx]
		//ACOdyssey.exe+C0DFD03 - 45 89 C6              - mov r14d,r8d
		if (enabled)
		{
			// set ret
			BYTE statementBytes[1] = { 0xC3 };						// ACOdyssey.exe+C0DFCC0 - 48 8B C4              - mov rax,rsp
			GameImageHooker::writeRange(aobBlocks[HUD_RENDER_INTERCEPT_KEY], statementBytes, 1);
		}
		else
		{
			// set original statement
			BYTE statementBytes[3] = { 0x48, 0x8B, 0xC4 };			// ACOdyssey.exe+C0DFCC0 - 48 8B C4              - mov rax,rsp
			GameImageHooker::writeRange(aobBlocks[HUD_RENDER_INTERCEPT_KEY], statementBytes, 3);
		}
	}


	// Toggles the dof write. Enabled means the write is enabled (so the default game code is restored). Disabled means the 
	// writes are disabled and we can write what we want there. 
	void toggleDofEnableWrite(map<string, AOBBlock*> &aobBlocks, bool enabled)
	{
		// DOF (In camera struct).
		// We have to NOP both writes (see below).
		//ACOdyssey.exe+E7FE935 - 57                    - push rdi
		//ACOdyssey.exe+E7FE936 - 48 83 EC 30           - sub rsp,30 { 48 }
		//ACOdyssey.exe+E7FE93A - 48 8B B9 30050000     - mov rdi,[rcx+00000530]
		//ACOdyssey.exe+E7FE941 - 48 89 CB              - mov rbx,rcx
		//ACOdyssey.exe+E7FE944 - 48 89 F9              - mov rcx,rdi
		//ACOdyssey.exe+E7FE947 - E8 C4F2B0F2           - call ACOdyssey.exe+130DC10
		//ACOdyssey.exe+E7FE94C - 48 89 F9              - mov rcx,rdi
		//ACOdyssey.exe+E7FE94F - 88 83 11010000        - mov [rbx+00000111],al					>> Enable (1) or disable (0) DOF. 
		//ACOdyssey.exe+E7FE955 - E8 86C7B1F2           - call ACOdyssey.exe+131B0E0
		//ACOdyssey.exe+E7FE95A - 88 83 13010000        - mov [rbx+00000113],al					>> Enable (1) or disable (0) DOF. 
		//ACOdyssey.exe+E7FE960 - 84 C0                 - test al,al
		//ACOdyssey.exe+E7FE962 - 0F84 97000000         - je ACOdyssey.exe+E7FE9FF
		//ACOdyssey.exe+E7FE968 - 0F29 74 24 20         - movaps [rsp+20],xmm6
		//ACOdyssey.exe+E7FE96D - 48 89 F9              - mov rcx,rdi
		if (enabled)
		{
			// write original code
			//ACOdyssey.exe+E7FE94F - 88 83 11010000        - mov [rbx+00000111],al					>> Enable (1) or disable (0) DOF. 
			BYTE statementBytes1[6] = { 0x88, 0x83, 0x11, 0x01, 0x00, 0x00 };
			GameImageHooker::writeRange(aobBlocks[DOF_ENABLE_WRITE_LOCATION_KEY], statementBytes1, 6);
			//ACOdyssey.exe+E7FE95A - 88 83 13010000        - mov [rbx+00000113],al					>> Enable (1) or disable (0) DOF. 
			BYTE statementBytes2[6] = { 0x88, 0x83, 0x13, 0x01, 0x00, 0x00 };
			// skip first statement plus the call after it.
			GameImageHooker::writeRange(aobBlocks[DOF_ENABLE_WRITE_LOCATION_KEY]->absoluteAddress()+6+5, statementBytes2, 6);
		}
		else
		{
			// nop two ranges
			//ACOdyssey.exe+E7FE94F - 88 83 11010000        - mov [rbx+00000111],al					>> Enable (1) or disable (0) DOF. 
			GameImageHooker::nopRange(aobBlocks[DOF_ENABLE_WRITE_LOCATION_KEY], 6);
			//ACOdyssey.exe+E7FE95A - 88 83 13010000        - mov [rbx+00000113],al					>> Enable (1) or disable (0) DOF. 
			// skip first statement plus the call after it.
			GameImageHooker::nopRange(aobBlocks[DOF_ENABLE_WRITE_LOCATION_KEY]->absoluteAddress() + 6 + 5, 6);
		}

	}
}
