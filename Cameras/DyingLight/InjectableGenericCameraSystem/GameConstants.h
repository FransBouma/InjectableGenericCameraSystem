//////////////////////////	//////////////////////////////////////////////////////////////////////////////
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
	#define GAME_NAME									"Dying Light v1.16+"
	#define CAMERA_VERSION								"1.0.1"
	#define CAMERA_CREDITS								"Otis_Inf. Thanks to Jim2Point0 for LoD override"
	#define GAME_WINDOW_TITLE							"Dying Light"
	#define INITIAL_PITCH_RADIANS						0.0f	// around X axis	(right)
	#define INITIAL_YAW_RADIANS							0.0f	// around Y axis	(out of the screen)
	#define INITIAL_ROLL_RADIANS						0.0f	// around Z axis	(up)
	#define CONTROLLER_Y_INVERT							false
	#define FASTER_MULTIPLIER							5.0f
	#define SLOWER_MULTIPLIER							0.1f
	#define MOUSE_SPEED_CORRECTION						0.2f	// to correct for the mouse-deltas related to normal rotation.
	#define DEFAULT_MOVEMENT_SPEED						0.07f
	#define DEFAULT_ROTATION_SPEED						0.01f
	#define DEFAULT_FOV_SPEED							0.0005f
	#define DEFAULT_UP_MOVEMENT_MULTIPLIER				0.7f
	#define MAX_LOD_VALUE								10000.0f
	#define NORMAL_LOD_VALUE							3.0f
	// End Mandatory constants

	// AOB Keys for interceptor's AOB scanner
	#define CAMERA_ADDRESS_INTERCEPT_KEY				"AOB_CAMERA_ADDRESS_INTERCEPT"
	#define CAMERA_WRITE1_INTERCEPT_KEY					"AOB_CAMERA_WRITE1_INTERCEPT"
	#define FOV_WRITE_INTERCEPT_KEY						"AOB_FOV_WRITE_INTERCEPT"
	#define LOD_READ_INTERCEPT_KEY						"AOB_LOD_READ_INTERCEPT"
	#define TOD_WRITE_INTERCEPT_KEY						"AOB_TOD_WRITE_INTERCEPT"
	#define TOD_READ_INTERCEPT_KEY						"AOB_TOD_READ_INTERCEPT"
	#define TIMESTOP_READ_INTERCEPT_KEY					"AOB_TIMESTOP_READ_INTERCEPT"
	#define HUD_RENDER_INTERCEPT_KEY					"AOB_HUD_RENDER_INTERCEPT"

	// Indices in the structures read by interceptors 
	#define MATRIX_IN_STRUCT_OFFSET						0x30			// matrix origin returned by hooked function is the inverse lighting matrix + the matrix we're intersted in. 
	#define FOV_IN_STRUCT_OFFSET						0x2C4			// The code uses 0x2D4, but it's 0x2C4 offset from the address returned by GetViewMatrix as that address is 0x10 offset from the struct root
	#define TIMESTOP_IN_STRUCT_OFFSET					0x30			
	#define LOD1_IN_STRUCT_OFFSET						0x28
	#define LOD2_IN_STRUCT_OFFSET						0x2C
}
