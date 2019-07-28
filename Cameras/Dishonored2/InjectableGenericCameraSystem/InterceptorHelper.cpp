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
		// v1.77.9
		//Dishonored2.exe+A2DD90 - 48 83 EC 28           - sub rsp,28 { 40 }
		//Dishonored2.exe+A2DD94 - 48 8B 0D CD2FCE01     - mov rcx,[Dishonored2.exe+2710D68] { (2E2896B5050) }
		//Dishonored2.exe+A2DD9B - 48 85 C9              - test rcx,rcx
		//Dishonored2.exe+A2DD9E - 0F84 EB000000         - je Dishonored2.exe+A2DE8F
		//Dishonored2.exe+A2DDA4 - 48 8B 01              - mov rax,[rcx]
		//Dishonored2.exe+A2DDA7 - FF 90 E8000000        - call qword ptr [rax+000000E8]
		//Dishonored2.exe+A2DDAD - 84 C0                 - test al,al
		//Dishonored2.exe+A2DDAF - 0F85 DA000000         - jne Dishonored2.exe+A2DE8F
		//Dishonored2.exe+A2DDB5 - 48 8B 0D AC2FCE01     - mov rcx,[Dishonored2.exe+2710D68] { (2E2896B5050) }
		//Dishonored2.exe+A2DDBC - 48 8B 01              - mov rax,[rcx]
		//Dishonored2.exe+A2DDBF - FF 90 F8000000        - call qword ptr [rax+000000F8]
		//Dishonored2.exe+A2DDC5 - 84 C0                 - test al,al
		//Dishonored2.exe+A2DDC7 - 0F85 C2000000         - jne Dishonored2.exe+A2DE8F
		//Dishonored2.exe+A2DDCD - 48 8B 0D 942FCE01     - mov rcx,[Dishonored2.exe+2710D68] { (2E2896B5050) }
		//Dishonored2.exe+A2DDD4 - 48 8B 01              - mov rax,[rcx]
		//Dishonored2.exe+A2DDD7 - FF 50 40              - call qword ptr [rax+40]
		//Dishonored2.exe+A2DDDA - 84 C0                 - test al,al
		//Dishonored2.exe+A2DDDC - 0F84 AD000000         - je Dishonored2.exe+A2DE8F
		//Dishonored2.exe+A2DDE2 - 48 8B 05 371DB702     - mov rax,[Dishonored2.exe+359FB20] { (7FF6907304F8) }
		//Dishonored2.exe+A2DDE9 - 48 8D 0D 301DB702     - lea rcx,[Dishonored2.exe+359FB20] { (7FF6907304F8) }
		//Dishonored2.exe+A2DDF0 - FF 90 10010000        - call qword ptr [rax+00000110]
		//Dishonored2.exe+A2DDF6 - 84 C0                 - test al,al
		//Dishonored2.exe+A2DDF8 - 0F85 91000000         - jne Dishonored2.exe+A2DE8F
		//Dishonored2.exe+A2DDFE - 48 8B 0D 632FCE01     - mov rcx,[Dishonored2.exe+2710D68] { (2E2896B5050) }
		//Dishonored2.exe+A2DE05 - 48 81 C1 C86E1F00     - add rcx,001F6EC8 { 2059976 }
		//Dishonored2.exe+A2DE0C - E8 9F4DEAFF           - call Dishonored2.exe+8D2BB0
		//Dishonored2.exe+A2DE11 - 83 F8 05              - cmp eax,05 { 5 }
		//Dishonored2.exe+A2DE14 - 74 79                 - je Dishonored2.exe+A2DE8F
		//Dishonored2.exe+A2DE16 - E8 A5066EFF           - call Dishonored2.exe+10E4C0
		//Dishonored2.exe+A2DE1B - 8B 05 B7FE1A03        - mov eax,[Dishonored2.exe+3BDDCD8] { (0) }
		//Dishonored2.exe+A2DE21 - 48 8D 0D 88FE1A03     - lea rcx,[Dishonored2.exe+3BDDCB0] { (7FF69249DCC0) }
		//Dishonored2.exe+A2DE28 - 33 D2                 - xor edx,edx
		//Dishonored2.exe+A2DE2A - 89 05 482FCE01        - mov [Dishonored2.exe+2710D78],eax { (0) }
		//Dishonored2.exe+A2DE30 - E8 2B9571FF           - call Dishonored2.exe+147360
		//Dishonored2.exe+A2DE35 - 48 8D 0D 64B1B602     - lea rcx,[Dishonored2.exe+3598FA0] { (7FF691E58FB0) }
		//Dishonored2.exe+A2DE3C - B2 01                 - mov dl,01 { 1 }

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
