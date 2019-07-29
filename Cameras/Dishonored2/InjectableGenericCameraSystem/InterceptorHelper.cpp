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

using namespace std;

namespace IGCS::GameSpecific::InterceptorHelper
{
	typedef void(__stdcall* AnselSessionStartStopFunction) ();
	
	void fixAnsel(LPBYTE hostImageAddress)
	{
		// v1.77.9
		//Dishonored2.exe+A2DD90 - 48 83 EC 28           - sub rsp,28 { 40 }
		//Dishonored2.exe+A2DD94 - 48 8B 0D CD2FCE01     - mov rcx,[Dishonored2.exe+2710D68] 
		//Dishonored2.exe+A2DD9B - 48 85 C9              - test rcx,rcx
		//Dishonored2.exe+A2DD9E - 0F84 EB000000         - je Dishonored2.exe+A2DE8F				<<< NOP
		//Dishonored2.exe+A2DDA4 - 48 8B 01              - mov rax,[rcx]
		//Dishonored2.exe+A2DDA7 - FF 90 E8000000        - call qword ptr [rax+000000E8]
		//Dishonored2.exe+A2DDAD - 84 C0                 - test al,al
		//Dishonored2.exe+A2DDAF - 0F85 DA000000         - jne Dishonored2.exe+A2DE8F				<<< NOP
		//Dishonored2.exe+A2DDB5 - 48 8B 0D AC2FCE01     - mov rcx,[Dishonored2.exe+2710D68] 
		//Dishonored2.exe+A2DDBC - 48 8B 01              - mov rax,[rcx]
		//Dishonored2.exe+A2DDBF - FF 90 F8000000        - call qword ptr [rax+000000F8]
		//Dishonored2.exe+A2DDC5 - 84 C0                 - test al,al
		//Dishonored2.exe+A2DDC7 - 0F85 C2000000         - jne Dishonored2.exe+A2DE8F				<<< NOP
		//Dishonored2.exe+A2DDCD - 48 8B 0D 942FCE01     - mov rcx,[Dishonored2.exe+2710D68] 
		//Dishonored2.exe+A2DDD4 - 48 8B 01              - mov rax,[rcx]
		//Dishonored2.exe+A2DDD7 - FF 50 40              - call qword ptr [rax+40]
		//Dishonored2.exe+A2DDDA - 84 C0                 - test al,al
		//Dishonored2.exe+A2DDDC - 0F84 AD000000         - je Dishonored2.exe+A2DE8F				<<< NOP
		//Dishonored2.exe+A2DDE2 - 48 8B 05 371DB702     - mov rax,[Dishonored2.exe+359FB20] 
		//Dishonored2.exe+A2DDE9 - 48 8D 0D 301DB702     - lea rcx,[Dishonored2.exe+359FB20] 
		//Dishonored2.exe+A2DDF0 - FF 90 10010000        - call qword ptr [rax+00000110]
		//Dishonored2.exe+A2DDF6 - 84 C0                 - test al,al
		//Dishonored2.exe+A2DDF8 - 0F85 91000000         - jne Dishonored2.exe+A2DE8F				<<< NOP
		//Dishonored2.exe+A2DDFE - 48 8B 0D 632FCE01     - mov rcx,[Dishonored2.exe+2710D68] 
		//Dishonored2.exe+A2DE05 - 48 81 C1 C86E1F00     - add rcx,001F6EC8 { 2059976 }
		//Dishonored2.exe+A2DE0C - E8 9F4DEAFF           - call Dishonored2.exe+8D2BB0
		//Dishonored2.exe+A2DE11 - 83 F8 05              - cmp eax,05 { 5 }
		//Dishonored2.exe+A2DE14 - 74 79                 - je Dishonored2.exe+A2DE8F				<<< NOP
		//Dishonored2.exe+A2DE16 - E8 A5066EFF           - call Dishonored2.exe+10E4C0
		//Dishonored2.exe+A2DE1B - 8B 05 B7FE1A03        - mov eax,[Dishonored2.exe+3BDDCD8] 
		//Dishonored2.exe+A2DE21 - 48 8D 0D 88FE1A03     - lea rcx,[Dishonored2.exe+3BDDCB0] 
		//Dishonored2.exe+A2DE28 - 33 D2                 - xor edx,edx
		//Dishonored2.exe+A2DE2A - 89 05 482FCE01        - mov [Dishonored2.exe+2710D78],eax 
		//Dishonored2.exe+A2DE30 - E8 2B9571FF           - call Dishonored2.exe+147360
		//Dishonored2.exe+A2DE35 - 48 8D 0D 64B1B602     - lea rcx,[Dishonored2.exe+3598FA0] 
		//Dishonored2.exe+A2DE3C - B2 01                 - mov dl,01 { 1 }
		GameImageHooker::nopRange(hostImageAddress + 0xA2DD9E, 6);
		GameImageHooker::nopRange(hostImageAddress + 0xA2DDAF, 6);
		GameImageHooker::nopRange(hostImageAddress + 0xA2DDC7, 6);
		GameImageHooker::nopRange(hostImageAddress + 0xA2DDDC, 6);
		GameImageHooker::nopRange(hostImageAddress + 0xA2DDF8, 6);
		GameImageHooker::nopRange(hostImageAddress + 0xA2DE14, 2);

		// switch off win_pauseOnLossOfFocus. This is needed when playing windowed and enabling ansel (for the people who want to).
		// Dishonored2.exe+152CF8A - 89 1D 282BEE01        - mov [Dishonored2.exe+340FAB8],ebx { (0.01) }
		GameImageHooker::writeBytesToProcessMemory(hostImageAddress + 0x340FAB8, 1, (BYTE)0);

		Console::WriteLine("Ansel enabled everywhere.");

		// set the movement speed inside the ansel struct in memory to 2.0f. 
		LPBYTE ansel64SDKdllAddress = (LPBYTE)Utils::getBaseAddressOfDll(L"AnselSDK64.dll"); 
		if (nullptr != ansel64SDKdllAddress)
		{
			float speedValue = 2.0f;
			BYTE* speedValueFirstByte;
			speedValueFirstByte = (BYTE*)& speedValue;
			// address is obtained from Ansel64SDK::getConfiguration -> configuration address -> check which location the movement variable is located. 
			GameImageHooker::writeRange(ansel64SDKdllAddress + 0x1B7F4, speedValueFirstByte, 4);

			Console::WriteLine("Ansel fixed so movement now works.");
		}
	}


	void startAnselSession(LPBYTE hostImageAddress)
	{
		AnselSessionStartStopFunction funcToCall = (AnselSessionStartStopFunction)(hostImageAddress + 0xA2DD90); //(see above)
		funcToCall();
	}


	void stopAnselSession(LPBYTE hostImageAddress)
	{
		// Function called by ansel as stopsession callback
		//Dishonored2.exe+A2DEA0 - 48 83 EC 68           - sub rsp,68 { 104 }
		//Dishonored2.exe+A2DEA4 - 48 C7 44 24 30 FEFFFFFF - mov qword ptr [rsp+30],FFFFFFFE { -2 }
		//Dishonored2.exe+A2DEAD - 8B 15 C52ECE01        - mov edx,[Dishonored2.exe+2710D78] { (0) }
		//Dishonored2.exe+A2DEB3 - 48 8D 4C 24 38        - lea rcx,[rsp+38]
		//Dishonored2.exe+A2DEB8 - E8 53C776FF           - call Dishonored2.exe+19A610
		//Dishonored2.exe+A2DEBD - 90                    - nop 
		//Dishonored2.exe+A2DEBE - 41 B0 01              - mov r8l,01 { 1 }
		//Dishonored2.exe+A2DEC1 - 48 8B 10              - mov rdx,[rax]
		//Dishonored2.exe+A2DEC4 - 48 8D 0D E5FD1A03     - lea rcx,[Dishonored2.exe+3BDDCB0] { (7FF7F9CDDCC0) }
		//Dishonored2.exe+A2DECB - E8 909271FF           - call Dishonored2.exe+147160
		//Dishonored2.exe+A2DED0 - 90                    - nop 
		//Dishonored2.exe+A2DED1 - 8B 4C 24 44           - mov ecx,[rsp+44]
		//Dishonored2.exe+A2DED5 - 8B C1                 - mov eax,ecx
		//Dishonored2.exe+A2DED7 - C1 E8 1F              - shr eax,1F { 31 }
		//Dishonored2.exe+A2DEDA - A8 01                 - test al,01 { 1 }
		//Dishonored2.exe+A2DEDC - 75 3A                 - jne Dishonored2.exe+A2DF18
		//Dishonored2.exe+A2DEDE - 48 83 7C 24 38 00     - cmp qword ptr [rsp+38],00 { 0 }
		//Dishonored2.exe+A2DEE4 - 74 32                 - je Dishonored2.exe+A2DF18
		//Dishonored2.exe+A2DEE6 - C1 E9 1E              - shr ecx,1E { 30 }
		//Dishonored2.exe+A2DEE9 - F6 C1 01              - test cl,01 { 1 }
		//Dishonored2.exe+A2DEEC - 74 2A                 - je Dishonored2.exe+A2DF18
		//Dishonored2.exe+A2DEEE - E8 5D2E78FF           - call Dishonored2.exe+1B0D50
		AnselSessionStartStopFunction funcToCall = (AnselSessionStartStopFunction)(hostImageAddress + 0xA2DEA0);
		funcToCall();
	}
}
