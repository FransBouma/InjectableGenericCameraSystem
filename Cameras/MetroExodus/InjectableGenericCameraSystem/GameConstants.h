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
	#define GAME_NAME									"Metro Exodus (v1.0.1.6+)"
	#define CAMERA_VERSION								"1.0.0"
	#define CAMERA_CREDITS								"Otis_Inf"
	#define GAME_WINDOW_TITLE							"4A Engine"
	#define INITIAL_PITCH_RADIANS						0.0f
	#define INITIAL_YAW_RADIANS							0.0f
	#define INITIAL_ROLL_RADIANS						0.0f
	#define CONTROLLER_Y_INVERT							true
	#define FASTER_MULTIPLIER							10.0f
	#define SLOWER_MULTIPLIER							0.1f
	#define MOUSE_SPEED_CORRECTION						0.2f	// to correct for the mouse-deltas related to normal rotation.
	#define DEFAULT_MOVEMENT_SPEED						0.04f
	#define DEFAULT_ROTATION_SPEED						0.015f
	#define DEFAULT_FOV_SPEED							0.15f
	#define DEFAULT_FOV_IN_DEGREES						60.0f
	#define DEFAULT_Z_MOVEMENT_MULTIPLIER				0.5f
	// End Mandatory constants

	// AOB Keys for interceptor's AOB scanner
	#define ANSEL_START_SESSION_KEY				"AOB_ANSEL_START_SESSION_KEY"
	#define ANSEL_RANGELIMITER_KEY				"AOB_ANSEL_RANGELIMITER_KEY"
	#define ANSEL_FOV_WRITE_KEY					"AOB_ANSEL_FOV_WRITE_KEY"
	#define CAMERA_ADDRESS_KEY					"AOB_CAMERA_ADDRESS_KEY"
	#define CAMERA_WRITE1_KEY					"AOB_CAMERA_WRITE1_KEY"
	#define CAMERA_WRITE2_KEY					"AOB_CAMERA_WRITE2_KEY"
	#define CAMERA_WRITE3_KEY					"AOB_CAMERA_WRITE3_KEY"
	#define CAMERA_WRITE4_KEY					"AOB_CAMERA_WRITE4_KEY"
	// write 5&6 are relative to 4
	#define CAMERA_WRITE7_KEY					"AOB_CAMERA_WRITE7_KEY"
	#define FOV_READ_KEY						"AOB_FOV_READ_KEY"

	// StartSession jump offsets to nop, relative from start of function
	//MetroExodus.exe+7B82C2B - BB 01000000           - mov ebx,00000001 { 1 }
	//MetroExodus.exe+7B82C30 - 48 89 F9              - mov rcx,rdi
	//MetroExodus.exe+7B82C33 - E8 08FDF4F8           - call MetroExodus.exe+AD2940
	//MetroExodus.exe+7B82C38 - 85 DB                 - test ebx,ebx
	//MetroExodus.exe+7B82C3A - 74 14                 - je MetroExodus.exe+7B82C50				<<< NOP
	//MetroExodus.exe+7B82C3C - 85 C0                 - test eax,eax
	//MetroExodus.exe+7B82C3E - 74 10                 - je MetroExodus.exe+7B82C50				<<  NOP
	//MetroExodus.exe+7B82C40 - B8 01000000           - mov eax,00000001 { 1 }
	//MetroExodus.exe+7B82C45 - 48 8B 5C 24 30        - mov rbx,[rsp+30]
	//MetroExodus.exe+7B82C4A - 48 83 C4 20           - add rsp,20 { 32 }
	#define STARTSESSION_JMP_OFFSET1			0x0
	#define STARTSESSION_JMP_OFFSET2			0x4
	#define CAMERA_WRITE3_2OFFSET				0x10
	#define CAMERA_WRITE5_OFFSET				0x15		// relative to write4's AOB
	#define CAMERA_WRITE6_OFFSET				0x3A		// relative to write4's AOB
	#define CAMERA_WRITE7_2OFFSET				0xB
	#define CAMERA_WRITE7_3OFFSET				0x16
	#define CAMERA_WRITE7_4OFFSET				0x25

	#define CAMERA_COORDS_IN_STRUCT_OFFSET		0x0
	#define CAMERA_MATRIX_IN_STRUCT_OFFSET		0x10
}