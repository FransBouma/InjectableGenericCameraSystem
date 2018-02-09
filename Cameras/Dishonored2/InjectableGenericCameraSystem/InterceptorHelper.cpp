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

using namespace std;

namespace IGCS::GameSpecific::InterceptorHelper
{
	void disableAnselChecks(LPBYTE hostImageAddress)
	{
		//Dishonored2.exe+4C79450 - 48 83 EC 28           - sub rsp,28 { 40 }
		//Dishonored2.exe+4C79454 - 48 8B 0D 0D3892FD     - mov rcx,[Dishonored2.exe+259CC68] 
		//Dishonored2.exe+4C7945B - 48 85 C9              - test rcx,rcx
		//Dishonored2.exe+4C7945E - 0F84 F1000000         - je Dishonored2.exe+4C79555				<<< NOP
		//Dishonored2.exe+4C79464 - 48 8B 01              - mov rax,[rcx]
		//Dishonored2.exe+4C79467 - FF 90 D0000000        - call qword ptr [rax+000000D0]
		//Dishonored2.exe+4C7946D - 84 C0                 - test al,al
		//Dishonored2.exe+4C7946F - 0F85 E0000000         - jne Dishonored2.exe+4C79555				<<< NOP
		//Dishonored2.exe+4C79475 - 48 8B 0D EC3792FD     - mov rcx,[Dishonored2.exe+259CC68] 
		//Dishonored2.exe+4C7947C - 48 8B 01              - mov rax,[rcx]
		//Dishonored2.exe+4C7947F - FF 90 E0000000        - call qword ptr [rax+000000E0]
		//Dishonored2.exe+4C79485 - 84 C0                 - test al,al
		//Dishonored2.exe+4C79487 - 0F85 C8000000         - jne Dishonored2.exe+4C79555				<<< NOP
		//Dishonored2.exe+4C7948D - 48 8B 0D D43792FD     - mov rcx,[Dishonored2.exe+259CC68] 
		//Dishonored2.exe+4C79494 - 48 8B 01              - mov rax,[rcx]
		//Dishonored2.exe+4C79497 - FF 50 40              - call qword ptr [rax+40]
		//Dishonored2.exe+4C7949A - 84 C0                 - test al,al
		//Dishonored2.exe+4C7949C - 0F84 B3000000         - je Dishonored2.exe+4C79555				<<< NOP
		//Dishonored2.exe+4C794A2 - 48 8B 05 47227BFE     - mov rax,[Dishonored2.exe+342B6F0] 
		//Dishonored2.exe+4C794A9 - 48 8D 0D 40227BFE     - lea rcx,[Dishonored2.exe+342B6F0] 
		//Dishonored2.exe+4C794B0 - FF 90 10010000        - call qword ptr [rax+00000110]
		//Dishonored2.exe+4C794B6 - 84 C0                 - test al,al
		//Dishonored2.exe+4C794B8 - 0F85 97000000         - jne Dishonored2.exe+4C79555				<<< NOP
		//Dishonored2.exe+4C794BE - 48 8B 0D A33792FD     - mov rcx,[Dishonored2.exe+259CC68] 
		//Dishonored2.exe+4C794C5 - 48 81 C1 C86E1F00     - add rcx,001F6EC8 { [00000000] }
		//Dishonored2.exe+4C794CC - E8 DF2DE9FF           - call Dishonored2.exe+4B0C2B0
		//Dishonored2.exe+4C794D1 - 83 F8 05              - cmp eax,05 { 5 }
		//Dishonored2.exe+4C794D4 - 74 7F                 - je Dishonored2.exe+4C79555				<<< NOP
		//Dishonored2.exe+4C794D6 - E8 F55F4CFF           - call Dishonored2.exe+413F4D0
		//Dishonored2.exe+4C794DB - 8B 05 C703DFFE        - mov eax,[Dishonored2.exe+3A698A8]

		GameImageHooker::nopRange(hostImageAddress + 0x4C7945E, 6);
		GameImageHooker::nopRange(hostImageAddress + 0x4C7946F, 6);
		GameImageHooker::nopRange(hostImageAddress + 0x4C79487, 6);
		GameImageHooker::nopRange(hostImageAddress + 0x4C7949C, 6);
		GameImageHooker::nopRange(hostImageAddress + 0x4C794B8, 6);
		GameImageHooker::nopRange(hostImageAddress + 0x4C794D4, 2);
	}
}
