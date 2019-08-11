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
	#define GAME_NAME									"Kingdom Come Deliverance (v1.9.2+)"
	#define CAMERA_VERSION								"1.0.0"
	#define CAMERA_CREDITS								"Otis_Inf"
	#define GAME_WINDOW_TITLE							"Kingdom Come Deliverance"
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
	#define DEFAULT_FOV_IN_DEGREES						65.0f
	#define DEFAULT_Z_MOVEMENT_MULTIPLIER				0.5f
	// End Mandatory constants

	// AOB Keys for interceptor's AOB scanner
	#define ANSEL_START_SESSION_KEY				"AOB_ANSEL_START_SESSION_KEY"
	#define ANSEL_STOP_SESSION_KEY				"AOB_ANSEL_STOP_SESSION_KEY"
	#define ANSEL_SETUP_UPDATECAMERA_KEY		"AOB_ANSEL_SETUP_UPDATECAMERA_KEY"
	#define ANSEL_SDK_GETCONFIGURATION_KEY		"AOB_ANSEL_SDK_GETCONFIGURATION_KEY"
	#define ANSEL_RANGELIMITER_KEY				"AOB_ANSEL_RANGELIMITER_KEY"
	#define ANSEL_CAMERADATA_WRITES_KEY			"AOB_ANSEL_CAMERADATA_WRITES_KEY"

	// StartSession jump offsets to nop, relative from start of function
	// v1.9.2
	//WHGame.DLL+1FA251C - 40 53                 - push rbx
	//WHGame.DLL+1FA251E - 48 83 EC 20           - sub rsp,20 { 32 }
	//WHGame.DLL+1FA2522 - 48 83 3D E62A8E00 00  - cmp qword ptr [WHGame.DLL+2885010],00 { (7FFB1E594720),0 }
	//WHGame.DLL+1FA252A - 48 8B D9              - mov rbx,rcx
	//WHGame.DLL+1FA252D - 75 04                 - jne WHGame.DLL+1FA2533			>> change to JMP (EB 04)
	//WHGame.DLL+1FA252F - 31 C0                 - xor eax,eax
	//WHGame.DLL+1FA2531 - EB 22                 - jmp WHGame.DLL+1FA2555
	//WHGame.DLL+1FA2533 - E8 A81AFFFF           - call WHGame.DLL+1F93FE0
	//WHGame.DLL+1FA2538 - 84 C0                 - test al,al
	//WHGame.DLL+1FA253A - 74 F3                 - je WHGame.DLL+1FA252F			>> NOP
	//WHGame.DLL+1FA253C - C6 43 04 00           - mov byte ptr [rbx+04],00 { 0 }
	//WHGame.DLL+1FA2540 - E8 1B9787FE           - call WHGame.DLL+81BC60
	//WHGame.DLL+1FA2545 - 48 8B 88 80000000     - mov rcx,[rax+00000080]
	//WHGame.DLL+1FA254C - B8 01000000           - mov eax,00000001 { 1 }
	//WHGame.DLL+1FA2551 - C6 41 0A 01           - mov byte ptr [rcx+0A],01 { 1 }
	//WHGame.DLL+1FA2555 - 48 83 C4 20           - add rsp,20 { 32 }
	//WHGame.DLL+1FA2559 - 5B                    - pop rbx
	//WHGame.DLL+1FA255A - C3                    - ret 
	#define STARTSESSION_JMP_OFFSET1			0x11
	#define STARTSESSION_JMP_OFFSET2			0x1E

	// Indices in the structures read by interceptors 
	// Ansel updateCamera caller
	//WHGame.DLL+1F8A736 - F3 0F10 0D 626E6F01   - movss xmm1,[WHGame.DLL+36815A0] { (703.49) }		>> X
	//WHGame.DLL+1F8A73E - F3 0F11 0E            - movss [rsi],xmm1
	//WHGame.DLL+1F8A742 - F3 0F10 05 5A6E6F01   - movss xmm0,[WHGame.DLL+36815A4] { (3491.64) }	>> Y
	//WHGame.DLL+1F8A74A - F3 0F11 46 04         - movss [rsi+04],xmm0
	//WHGame.DLL+1F8A74F - F3 0F10 0D 516E6F01   - movss xmm1,[WHGame.DLL+36815A8] { (124.24) }		>> Z
	//WHGame.DLL+1F8A757 - F3 0F11 4E 08         - movss [rsi+08],xmm1
	//WHGame.DLL+1F8A75C - F3 0F10 05 486E6F01   - movss xmm0,[WHGame.DLL+36815AC] { (-0.01) }		>> qX
	//WHGame.DLL+1F8A764 - F3 0F11 46 0C         - movss [rsi+0C],xmm0
	//WHGame.DLL+1F8A769 - F3 0F10 0D 3F6E6F01   - movss xmm1,[WHGame.DLL+36815B0] { (-0.01) }		>> qY
	//WHGame.DLL+1F8A771 - F3 0F11 4E 10         - movss [rsi+10],xmm1
	//WHGame.DLL+1F8A776 - F3 0F10 05 366E6F01   - movss xmm0,[WHGame.DLL+36815B4] { (0.71) }		>> qZ
	//WHGame.DLL+1F8A77E - F3 0F11 46 14         - movss [rsi+14],xmm0
	//WHGame.DLL+1F8A783 - F3 0F10 0D 2D6E6F01   - movss xmm1,[WHGame.DLL+36815B8] { (0.70) }		>> qW
	//WHGame.DLL+1F8A78B - F3 0F11 4E 18         - movss [rsi+18],xmm1
	//WHGame.DLL+1F8A790 - F3 0F10 05 246E6F01   - movss xmm0,[WHGame.DLL+36815BC] { (65.00) }		>> FoV
	// offsets are relative to the X coordinate address (which is the first value in the struct
	#define CAMERA_QUATERNION_IN_STRUCT_OFFSET			0xC
	#define CAMERA_COORDS_IN_STRUCT_OFFSET				0x0
	#define FOV_IN_STRUCT_OFFSET						0x1C
}