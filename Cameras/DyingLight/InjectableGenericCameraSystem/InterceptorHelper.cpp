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
	void lodReadInterceptor();
	void timestopReadInterceptor();
	void todWriteInterceptor();
	void todReadInterceptor();
}

// external addresses used in asm.
extern "C" {
	LPBYTE _cameraStructInterceptionContinue = nullptr;
	LPBYTE _cameraWrite1InterceptionContinue = nullptr;
	LPBYTE _lodReadInterceptionContinue = nullptr;
	LPBYTE _timestopReadInterceptionContinue = nullptr;
	LPBYTE _todWriteInterceptionContinue = nullptr;
	LPBYTE _todReadInterceptionContinue = nullptr;
}


namespace IGCS::GameSpecific::InterceptorHelper
{
	void initializeAOBBlocks(LPBYTE hostImageAddress, DWORD hostImageSize, map<string, AOBBlock*> &aobBlocks)
	{
		aobBlocks[CAMERA_ADDRESS_INTERCEPT_KEY] = new AOBBlock(CAMERA_ADDRESS_INTERCEPT_KEY, "48 8B 41 08 48 85 C0 74 05 48 83 C0 10 C3 48 8D 05", 1);	
		aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY] = new AOBBlock(CAMERA_WRITE1_INTERCEPT_KEY, "48 83 EC 48 0F 29 74 24 30 F3 41 0F 10 70 08 0F 29 7C 24 20 F3 41 0F 10 38 44 0F 29 44 24 10", 1);
		aobBlocks[FOV_WRITE_INTERCEPT_KEY] = new AOBBlock(FOV_WRITE_INTERCEPT_KEY, "F3 0F 11 B3 D4 02 00 00 F3 0F 59 35 B1 A2 6C 00 0F 28 C6", 1);
		aobBlocks[LOD_READ_INTERCEPT_KEY] = new AOBBlock(LOD_READ_INTERCEPT_KEY, "F3 0F 10 88 30 02 00 00 F3 0F 59 4A 28 F3 0F 59 C9 F3 0F 5E C1 F3 41 0F 11 86 84 01 00 00", 1);
		aobBlocks[TOD_WRITE_INTERCEPT_KEY] = new AOBBlock(TOD_WRITE_INTERCEPT_KEY, "48 85 C0 74 1C F3 0F 11 30 8B 43 08 83 F8 FE 72 10 E8 14 AA DD FF 48 8B D3", 1);
		aobBlocks[TIMESTOP_READ_INTERCEPT_KEY] = new AOBBlock(TIMESTOP_READ_INTERCEPT_KEY, "48 8B 41 08 48 05 28 0F 00 00 80 78 30 00 74 09 80 78 48 00", 1);
		aobBlocks[HUD_RENDER_INTERCEPT_KEY] = new AOBBlock(HUD_RENDER_INTERCEPT_KEY, "40 53 48 83 EC 40 0F 29 74 24 30 0F 29 7C 24 20 48 8B D9 0F 28 FB", 1);

		map<string, AOBBlock*>::iterator it;
		bool result = true;
		for (it = aobBlocks.begin(); it != aobBlocks.end(); it++)
		{
			result &= it->second->scan(hostImageAddress, hostImageSize);
		}

		// we need to do a separate scan here, as it's done on another in-memory image. This is specific for this camera so we have some custom code here which pulls the address of the
		// particular dll and makes the scan here. 
		MODULEINFO gamedllModuleInfo = Utils::getModuleInfoOfDll(L"gamedll_x64_rwdi.dll");
		if (nullptr == gamedllModuleInfo.lpBaseOfDll)
		{
			OverlayConsole::instance().logError("Can't determine the address for the gamedll, can't apply gamedll specific hooks");
			result = false;
		}
		else
		{
			auto todReadAOB = new AOBBlock(TOD_READ_INTERCEPT_KEY, "48 8B CF 48 85 C0 48 0F 45 C8 F3 0F 10 39", 1);
			aobBlocks[TOD_READ_INTERCEPT_KEY] = todReadAOB;
			result &= todReadAOB->scan((LPBYTE)gamedllModuleInfo.lpBaseOfDll, gamedllModuleInfo.SizeOfImage);
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
		GameImageHooker::setHook(aobBlocks[CAMERA_WRITE1_INTERCEPT_KEY], 0x0F, &_cameraWrite1InterceptionContinue, &cameraWrite1Interceptor);
		GameImageHooker::setHook(aobBlocks[LOD_READ_INTERCEPT_KEY], 0x11, &_lodReadInterceptionContinue, &lodReadInterceptor);
		GameImageHooker::setHook(aobBlocks[TIMESTOP_READ_INTERCEPT_KEY], 0x0E, &_timestopReadInterceptionContinue, &timestopReadInterceptor);
		GameImageHooker::setHook(aobBlocks[TOD_WRITE_INTERCEPT_KEY], 0x0F, &_todWriteInterceptionContinue, &todWriteInterceptor);
		GameImageHooker::setHook(aobBlocks[TOD_READ_INTERCEPT_KEY], 0x0E, &_todReadInterceptionContinue, &todReadInterceptor);
	}


	// if enabled is true, we'll place a 'ret' at the start of the code block, making the game skip rendering any hud element. If false, we'll reset
	// the original first statement so code will proceed as normal. 
	void toggleHudRenderState(map<string, AOBBlock*> &aobBlocks, bool enabled)
	{
		// start of HUD widget render code
		//engine_x64_rwdi.dll+5B8B20 - 40 53                 - push rbx										<< REPLACE WITH RET to hide hud.
		//engine_x64_rwdi.dll+5B8B22 - 48 83 EC 40           - sub rsp,40
		//engine_x64_rwdi.dll+5B8B26 - 0F29 74 24 30         - movaps [rsp+30],xmm6
		//engine_x64_rwdi.dll+5B8B2B - 0F29 7C 24 20         - movaps [rsp+20],xmm7
		//engine_x64_rwdi.dll+5B8B30 - 48 8B D9              - mov rbx,rcx
		//engine_x64_rwdi.dll+5B8B33 - 0F28 FB               - movaps xmm7,xmm3
		//engine_x64_rwdi.dll+5B8B36 - E8 4509FEFF           - call engine_x64_rwdi.dll+599480
		//engine_x64_rwdi.dll+5B8B3B - E8 70EFA5FF           - call engine_x64_rwdi.dll+17AB0
		//engine_x64_rwdi.dll+5B8B40 - 83 F8 01              - cmp eax,01 { 1 }
		//engine_x64_rwdi.dll+5B8B43 - 74 0B                 - je engine_x64_rwdi.dll+5B8B50
		//engine_x64_rwdi.dll+5B8B45 - 85 C0                 - test eax,eax
		//engine_x64_rwdi.dll+5B8B47 - 48 8D 83 9C010000     - lea rax,[rbx+0000019C]
		//engine_x64_rwdi.dll+5B8B4E - 75 07                 - jne engine_x64_rwdi.dll+5B8B57
		//engine_x64_rwdi.dll+5B8B50 - 48 8D 83 60010000     - lea rax,[rbx+00000160]
		if (enabled)
		{
			// set ret
			BYTE statementBytes[2] = { 0xC3, 0xC3 };					// engine_x64_rwdi.dll + 5B8B20 - 40 53 - push rbx. Set 2 times ret, for alignment.
			GameImageHooker::writeRange(aobBlocks[HUD_RENDER_INTERCEPT_KEY], statementBytes, 2);
		}
		else
		{
			// set original statement
			BYTE statementBytes[2] = { 0x40, 0x53 };			// engine_x64_rwdi.dll + 5B8B20 - 40 53 - push rbx
			GameImageHooker::writeRange(aobBlocks[HUD_RENDER_INTERCEPT_KEY], statementBytes, 2);
		}
	}


	// if 'enabled' is true, we'll nop the range where the FoV writes are placed, otherwise we'll write back the original statement bytes. 
	void toggleFoVWriteState(map<string, AOBBlock*> &aobBlocks, bool enabled)
	{
		// fov write 
		//engine_x64_rwdi.dll+11F573 - F3 0F11 B3 D4020000   - movss [rbx+000002D4],xmm6						<< WRITE FOV
		//engine_x64_rwdi.dll+11F57B - F3 0F59 35 B1A26C00   - mulss xmm6,[engine_x64_rwdi.dll+7E9834]
		//engine_x64_rwdi.dll+11F583 - 0F28 C6               - movaps xmm0,xmm6
		//engine_x64_rwdi.dll+11F586 - E8 DB606900           - call engine_x64_rwdi.dll+7B5666
		//engine_x64_rwdi.dll+11F58B - 48 8B 05 76A69000     - mov rax,[engine_x64_rwdi.dll+A29C08]
		//engine_x64_rwdi.dll+11F592 - F3 0F59 C7            - mulss xmm0,xmm7

		if (enabled)
		{
			// enable writes
			BYTE originalStatementBytes[8] = { 0xF3, 0x0F, 0x11, 0xB3, 0xD4, 0x02, 0x00, 0x00 };			// engine_x64_rwdi.dll+11F573 - F3 0F11 B3 D4020000   - movss [rbx+000002D4],xmm6
			GameImageHooker::writeRange(aobBlocks[FOV_WRITE_INTERCEPT_KEY], originalStatementBytes, 8);
		}
		else
		{
			// disable writes, by nopping the range.
			GameImageHooker::nopRange(aobBlocks[FOV_WRITE_INTERCEPT_KEY], 8);
		}
	}
}
