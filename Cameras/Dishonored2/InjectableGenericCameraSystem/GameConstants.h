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
#pragma once

namespace IGCS::GameSpecific
{
	// Mandatory constants to define for a game
	#define GAME_NAME									"Dishonored 2 (v1.77.10+) or Dishonored: Death of the Outsider (v1.145+)"
	#define CAMERA_VERSION								"1.0.3"
	#define CAMERA_CREDITS								"Otis_Inf"
	#define GAME_WINDOW_TITLE							"Dishonored"
	#define INITIAL_PITCH_RADIANS						0.0f
	#define INITIAL_YAW_RADIANS							0.0f
	#define INITIAL_ROLL_RADIANS						0.0f
	#define CONTROLLER_Y_INVERT							true
	#define FASTER_MULTIPLIER							7.0f
	#define SLOWER_MULTIPLIER							0.08f
	#define MOUSE_SPEED_CORRECTION						0.2f	// to correct for the mouse-deltas related to normal rotation.
	#define DEFAULT_MOVEMENT_SPEED						0.025f
	#define DEFAULT_ROTATION_SPEED						0.015f
	#define DEFAULT_FOV_SPEED							0.15f
	#define DEFAULT_FOV_IN_DEGREES						48.0f
	#define DEFAULT_Z_MOVEMENT_MULTIPLIER				0.5f
	// End Mandatory constants

	// AOB Keys for interceptor's AOB scanner
	#define ANSEL_START_SESSION_KEY				"AOB_ANSEL_START_SESSION_KEY"
	#define ANSEL_STOP_SESSION_KEY				"AOB_ANSEL_STOP_SESSION_KEY"
	#define ANSEL_SETUP_UPDATECAMERA_KEY		"AOB_ANSEL_SETUP_UPDATECAMERA_KEY"
	#define ANSEL_SDK_GETCONFIGURATION_KEY		"AOB_ANSEL_SDK_GETCONFIGURATION_KEY"
	#define WIN_PAUSEONFOCUSLOSS_KEY			"AOB_WIN_PAUSEONFOCUSLOSS_KEY"

	// StartSession jump offsets to nop, relative from start of function
	// v1.77.10
	//Dishonored2.exe+8B1380 - 48 83 EC 28           - sub rsp,28 { 40 }
	//Dishonored2.exe+8B1384 - 48 8B 0D 053F0F03     - mov rcx,[Dishonored2.exe+39A5290] 
	//Dishonored2.exe+8B138B - 48 85 C9              - test rcx,rcx
	//Dishonored2.exe+8B138E - 0F84 FB000000         - je Dishonored2.exe+8B148F				<<< NOP
	//Dishonored2.exe+8B1394 - 48 8B 01              - mov rax,[rcx]
	//Dishonored2.exe+8B1397 - FF 90 E8000000        - call qword ptr [rax+000000E8]
	//Dishonored2.exe+8B139D - 84 C0                 - test al,al
	//Dishonored2.exe+8B139F - 0F85 EA000000         - jne Dishonored2.exe+8B148F				<<< NOP
	//Dishonored2.exe+8B13A5 - 48 8B 0D E43E0F03     - mov rcx,[Dishonored2.exe+39A5290] 
	//Dishonored2.exe+8B13AC - 48 8B 01              - mov rax,[rcx]
	//Dishonored2.exe+8B13AF - FF 90 F8000000        - call qword ptr [rax+000000F8]
	//Dishonored2.exe+8B13B5 - 84 C0                 - test al,al
	//Dishonored2.exe+8B13B7 - 0F85 D2000000         - jne Dishonored2.exe+8B148F				<<< NOP
	//Dishonored2.exe+8B13BD - 48 8B 0D CC3E0F03     - mov rcx,[Dishonored2.exe+39A5290] 
	//Dishonored2.exe+8B13C4 - 48 8B 01              - mov rax,[rcx]
	//Dishonored2.exe+8B13C7 - FF 50 40              - call qword ptr [rax+40]
	//Dishonored2.exe+8B13CA - 84 C0                 - test al,al
	//Dishonored2.exe+8B13CC - 0F84 BD000000         - je Dishonored2.exe+8B148F				<<< NOP
	//Dishonored2.exe+8B13D2 - 48 8B 0D 97B9A401     - mov rcx,[Dishonored2.exe+22FCD70] 
	//Dishonored2.exe+8B13D9 - 48 8B 01              - mov rax,[rcx]
	//Dishonored2.exe+8B13DC - FF 90 10010000        - call qword ptr [rax+00000110]
	//Dishonored2.exe+8B13E2 - 84 C0                 - test al,al
	//Dishonored2.exe+8B13E4 - 0F85 A5000000         - jne Dishonored2.exe+8B148F				<<< NOP
	//Dishonored2.exe+8B13EA - 48 8B 0D 9F3E0F03     - mov rcx,[Dishonored2.exe+39A5290] 
	//Dishonored2.exe+8B13F1 - 48 81 C1 C86E1F00     - add rcx,001F6EC8 { 2059976 }
	//Dishonored2.exe+8B13F8 - E8 3375EDFF           - call Dishonored2.exe+788930
	//Dishonored2.exe+8B13FD - 83 F8 05              - cmp eax,05 { 5 }
	//Dishonored2.exe+8B1400 - 0F84 89000000         - je Dishonored2.exe+8B148F				<<< NOP
	//Dishonored2.exe+8B1406 - 48 8B 0D EB589D01     - mov rcx,[Dishonored2.exe+2286CF8] 
	//Dishonored2.exe+8B140D - E8 BE1E82FF           - call Dishonored2.exe+D32D0
	//Dishonored2.exe+8B1412 - 8B 05 D0AA0C03        - mov eax,[Dishonored2.exe+397BEE8] 
	//Dishonored2.exe+8B1418 - 48 8D 0D A1AA0C03     - lea rcx,[Dishonored2.exe+397BEC0] 
	//Dishonored2.exe+8B141F - 41 B0 01              - mov r8b,01 { 1 }
	//Dishonored2.exe+8B1422 - 33 D2                 - xor edx,edx
	//Dishonored2.exe+8B1424 - 89 05 7E3E0F03        - mov [Dishonored2.exe+39A52A8],eax 
	//Dishonored2.exe+8B142A - E8 A12D85FF           - call Dishonored2.exe+1041D0
	//Dishonored2.exe+8B142F - 41 B0 01              - mov r8b,01 { 1 }
	//Dishonored2.exe+8B1432 - 48 8D 0D A73FA402     - lea rcx,[Dishonored2.exe+32F53E0] 
	//Dishonored2.exe+8B1439 - 41 0FB6 D0            - movzx edx,r8b
	//Dishonored2.exe+8B143D - E8 CE2985FF           - call Dishonored2.exe+103E10
	//Dishonored2.exe+8B1442 - 83 3D 3F40A402 00     - cmp dword ptr [Dishonored2.exe+32F5488],00 
	//Dishonored2.exe+8B1449 - 41 B0 01              - mov r8b,01 { 1 }
	//Dishonored2.exe+8B144C - 48 8D 0D 0D40A402     - lea rcx,[Dishonored2.exe+32F5460]
	//Dishonored2.exe+8B1453 - 41 0FB6 D0            - movzx edx,r8b
	//Dishonored2.exe+8B1457 - 0F95 05 4E3E0F03      - setne byte ptr [Dishonored2.exe+39A52AC] { (0) }
	//Dishonored2.exe+8B145E - E8 AD2985FF           - call Dishonored2.exe+103E10
	//Dishonored2.exe+8B1463 - 48 8B 0D 76A69D01     - mov rcx,[Dishonored2.exe+228BAE0] { (7FF6B8E1BB00) }
	//Dishonored2.exe+8B146A - 48 8D 15 2F2E3E01     - lea rdx,[Dishonored2.exe+1C942A0] { ("Ansel_PlayerVisible 0") }
	//Dishonored2.exe+8B1471 - 48 8B 01              - mov rax,[rcx]
	//Dishonored2.exe+8B1474 - FF 50 48              - call qword ptr [rax+48]
	//Dishonored2.exe+8B1477 - C6 05 1E3E0F03 01     - mov byte ptr [Dishonored2.exe+39A529C],01 { (0),1 }
	//Dishonored2.exe+8B147E - C6 05 193E0F03 01     - mov byte ptr [Dishonored2.exe+39A529E],01 { (0),1 }
	//Dishonored2.exe+8B1485 - B8 01000000           - mov eax,00000001 { 1 }
	//Dishonored2.exe+8B148A - 48 83 C4 28           - add rsp,28 { 40 }
	//Dishonored2.exe+8B148E - C3                    - ret 
	//Dishonored2.exe+8B148F - 33 C0                 - xor eax,eax
	//Dishonored2.exe+8B1491 - 48 83 C4 28           - add rsp,28 { 40 }
	//Dishonored2.exe+8B1495 - C3                    - ret 
	#define STARTSESSION_JMP_OFFSET1			0xE
	#define STARTSESSION_JMP_OFFSET2			0x1F
	#define STARTSESSION_JMP_OFFSET3			0x37
	#define STARTSESSION_JMP_OFFSET4			0x4C
	#define STARTSESSION_JMP_OFFSET5			0x64
	#define STARTSESSION_JMP_OFFSET6			0x80


	// Indices in the structures read by interceptors 
	// Ansel updateCamera caller
	//Dishonored2.exe+41A668 - F3 0F10 83 703A0000   - movss xmm0,[rbx+00003A70]
	//Dishonored2.exe+41A670 - C6 05 C0662F02 00     - mov byte ptr [Dishonored2.exe+2710D37],00 { (0),0 }
	//Dishonored2.exe+41A677 - F3 0F59 05 B971C501   - mulss xmm0,[Dishonored2.exe+2071838] { (57.30) }
	//Dishonored2.exe+41A67F - F3 0F11 05 054FC303   - movss [Dishonored2.exe+404F58C],xmm0 { (48.98) }				<< FOV
	//Dishonored2.exe+41A687 - F3 0F10 83 783A0000   - movss xmm0,[rbx+00003A78]
	//Dishonored2.exe+41A68F - F3 0F11 05 D94EC303   - movss [Dishonored2.exe+404F570],xmm0 { (-55.43) }			<< X
	//Dishonored2.exe+41A697 - F3 0F10 8B 7C3A0000   - movss xmm1,[rbx+00003A7C]
	//Dishonored2.exe+41A69F - F3 0F11 0D CD4EC303   - movss [Dishonored2.exe+404F574],xmm1 { (-161.03) }			<< Y
	//Dishonored2.exe+41A6A7 - F3 0F10 83 803A0000   - movss xmm0,[rbx+00003A80]
	//Dishonored2.exe+41A6AF - F3 0F10 4D 97         - movss xmm1,[rbp-69]
	//Dishonored2.exe+41A6B4 - F3 0F11 05 BC4EC303   - movss [Dishonored2.exe+404F578],xmm0 { (68.13) }				<< Z
	//Dishonored2.exe+41A6BC - F3 0F10 45 9B         - movss xmm0,[rbp-65]
	//Dishonored2.exe+41A6C1 - F3 0F11 0D B34EC303   - movss [Dishonored2.exe+404F57C],xmm1 { (-0.01) }				<< qX
	//Dishonored2.exe+41A6C9 - F3 0F10 4D 9F         - movss xmm1,[rbp-61]
	//Dishonored2.exe+41A6CE - F3 0F11 05 AA4EC303   - movss [Dishonored2.exe+404F580],xmm0 { (0.01) }
	//Dishonored2.exe+41A6D6 - F3 0F10 45 A3         - movss xmm0,[rbp-5D]
	//Dishonored2.exe+41A6DB - F3 0F11 05 A54EC303   - movss [Dishonored2.exe+404F588],xmm0 { (0.68) }
	//Dishonored2.exe+41A6E3 - F3 0F11 0D 994EC303   - movss [Dishonored2.exe+404F584],xmm1 { (0.74) }
	//Dishonored2.exe+41A6EB - 48 8D 0D 7E4EC303     - lea rcx,[Dishonored2.exe+404F570] { (-55.43) }
	//Dishonored2.exe+41A6F2 - FF 15 40998201        - call qword ptr [Dishonored2.exe+1C44038] <<<<<< AnselSDK::UpdateCamera
	// offsets are relative to the X coordinate address (which is the first value in the struct
	#define CAMERA_QUATERNION_IN_STRUCT_OFFSET			0xC
	#define CAMERA_COORDS_IN_STRUCT_OFFSET				0x0
	#define FOV_IN_STRUCT_OFFSET						0x1C
}