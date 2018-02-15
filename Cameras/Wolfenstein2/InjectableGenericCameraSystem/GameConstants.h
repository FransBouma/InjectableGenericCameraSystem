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
	#define GAME_NAME									"Wolfenstein II: The New Colossus"
	#define CAMERA_VERSION								"1.0.0"
	#define CAMERA_CREDITS								"Otis_Inf"
	#define GAME_WINDOW_TITLE							"Wolfenstein II The New Colossusx64vk"
	#define INITIAL_PITCH_RADIANS						0.0f
	#define INITIAL_YAW_RADIANS							0.0f
	#define INITIAL_ROLL_RADIANS						0.0f
	#define CONTROLLER_Y_INVERT							true
	#define FASTER_MULTIPLIER							5.0f
	#define SLOWER_MULTIPLIER							0.05f
	#define MOUSE_SPEED_CORRECTION						0.2f	// to correct for the mouse-deltas related to normal rotation.
	#define DEFAULT_MOVEMENT_SPEED						0.01f
	#define DEFAULT_ROTATION_SPEED						0.01f
	#define DEFAULT_FOV_DEGREES							40.0f
	#define DEFAULT_FOV_SPEED							0.1f
	#define DEFAULT_Z_MOVEMENT_MULTIPLIER				0.5f
	#define DEFAULT_SKIP_FRAMES_COUNT					5
	// End Mandatory constants

	// AOB Keys for interceptor's AOB scanner
	#define CAMERA_ADDRESS_INTERCEPT_KEY				"AOB_CAMERA_ADDRESS_INTERCEPT"
	#define FOV_ADDRESS_INTERCEPT_KEY					"AOB_FOV_ADDRESS_INTERCEPT"
	#define SHOWHUD_CVAR_ADDRESS_INTERCEPT_KEY			"AOB_SHOWHUD_ADDRESS_INTERCEPT"
	#define STOPTIME_CVAR_ADDRESS_INTERCEPT_KEY			"AOB_STOPTIME_CVAR_ADDRESS_INTERCEPT_KEY"
	#define RUNFRAMES_CVAR_ADDRESS_INTERCEPT_KEY		"AOB_RUNFRAMES_CVAR_ADDRESS_INTERCEPT_KEY"
														
	// Indices in the structures read by interceptors 
	#define ROTATION_MATRIX_IN_CAMERA_STRUCT_OFFSET		0xF4		
	#define CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET		0xE8		
	#define FOV_IN_FOV_STRUCT_OFFSET					0x18
}