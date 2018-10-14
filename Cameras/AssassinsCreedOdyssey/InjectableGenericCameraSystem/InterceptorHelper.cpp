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
		aobBlocks[PHOTOMODE_RANGE_DISABLE_KEY] = new AOBBlock(PHOTOMODE_RANGE_DISABLE_KEY, "F3 0F 5D F1 F3 41 0F 5F CA 0F 28 D6 0F C6 D2 00 0F 28 C1 41 0F 59 D3 0F C6 C0 00", 1);
		aobBlocks[DOF_ENABLE_WRITE_LOCATION_KEY] = new AOBBlock(DOF_ENABLE_WRITE_LOCATION_KEY, "88 83 11 01 00 00 E8 ?? ?? ?? ?? 88 83 13 01 00 00 84 C0", 1);

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
	}


	void disablePhotomodeRangeLimit(map<string, AOBBlock*> &aobBlocks)
	{
		//ACOdyssey.exe+2A406B5 - 66 0F3814 D9          - blendvps xmm3,xmm1,xmm0
		//ACOdyssey.exe+2A406BA - 44 0F5C DB            - subps xmm11,xmm3
		//ACOdyssey.exe+2A406BE - 48 8B D6              - mov rdx,rsi
		//ACOdyssey.exe+2A406C1 - 41 0F28 C3            - movaps xmm0,xmm11
		//ACOdyssey.exe+2A406C5 - 66 41 0F3A40 C3 7F    - dpps xmm0,xmm11,7F
		//ACOdyssey.exe+2A406CC - 0F51 C8               - sqrtps xmm1,xmm0
		//ACOdyssey.exe+2A406CF - F3 0F5D F1            - minss xmm6,xmm1					<< Change into movss xmm6, xmm1 to remove the limit: F3 0F10 F1 - movss xmm6,xmm1
		//ACOdyssey.exe+2A406D3 - F3 41 0F5F CA         - maxss xmm1,xmm10
		//ACOdyssey.exe+2A406D8 - 0F28 D6               - movaps xmm2,xmm6
		//ACOdyssey.exe+2A406DB - 0FC6 D2 00            - shufps xmm2,xmm200
		//ACOdyssey.exe+2A406DF - 0F28 C1               - movaps xmm0,xmm1
		//ACOdyssey.exe+2A406E2 - 41 0F59 D3            - mulps xmm2,xmm11
		//ACOdyssey.exe+2A406E6 - 0FC6 C0 00            - shufps xmm0,xmm000
		//ACOdyssey.exe+2A406EA - 0F5E D0               - divps xmm2,xmm0
		BYTE statementBytes[4] = { 0xF3, 0x0F, 0x10, 0xF1 };				//ACOdyssey.exe+2A406CF - F3 0F5D F1 - minss xmm6,xmm1	--> F3 0F10 F1 - movss xmm6,xmm1
		GameImageHooker::writeRange(aobBlocks[PHOTOMODE_RANGE_DISABLE_KEY], statementBytes, 4);
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
