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
	#define GAME_NAME									"Dishonored 2, v1.77.9+"
	#define CAMERA_VERSION								"1.0.1"
	#define CAMERA_CREDITS								"Otis_Inf"
	#define GAME_WINDOW_TITLE							"Dishonored 2"
	#define INITIAL_PITCH_RADIANS						0.0f
	#define INITIAL_YAW_RADIANS							0.0f
	#define INITIAL_ROLL_RADIANS						0.0f
	#define CONTROLLER_Y_INVERT							true
	#define FASTER_MULTIPLIER							5.0f
	#define SLOWER_MULTIPLIER							0.1f
	#define MOUSE_SPEED_CORRECTION						0.2f	// to correct for the mouse-deltas related to normal rotation.
	#define DEFAULT_MOVEMENT_SPEED						0.1f
	#define DEFAULT_ROTATION_SPEED						0.015f
	#define DEFAULT_FOV_SPEED							0.15f
	#define DEFAULT_Z_MOVEMENT_MULTIPLIER				0.5f
	// End Mandatory constants

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
	#define CAMERA_QUATERNION_IN_IMAGE_OFFSET			0x404F57C
	#define CAMERA_COORDS_IN_IMAGE_OFFSET				0x404F570		
	#define FOV_IN_STRUCT_OFFSET						0x404F58C
}