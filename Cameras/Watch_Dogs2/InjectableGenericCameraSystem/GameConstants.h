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
	#define GAME_NAME									"Watch_Dogs 2 (v1.016+)"
	#define CAMERA_VERSION								"1.0.5"
	#define CAMERA_CREDITS								"Otis_Inf. Hotsample code by DeadEndThrills"
	#define GAME_WINDOW_TITLE							"Watch_Dogs 2"
	#define INITIAL_PITCH_RADIANS						0.0f	// World has X right, Z up, Y into the screen.
	#define INITIAL_YAW_RADIANS							0.0f	// around Z axis	(up)
	#define INITIAL_ROLL_RADIANS						0.0f	// around Y axis	(into the screen)
	#define CONTROLLER_Y_INVERT							false
	#define FASTER_MULTIPLIER							10.0f
	#define SLOWER_MULTIPLIER							0.1f
	#define MOUSE_SPEED_CORRECTION						0.2f	// to correct for the mouse-deltas related to normal rotation.
	#define DEFAULT_MOVEMENT_SPEED						0.015f
	#define DEFAULT_ROTATION_SPEED						0.012f
	#define DEFAULT_FOV_RADIANS							1.2217f		// 70 degrees, is the default.
	#define DEFAULT_FOV_SPEED							0.0025f
	#define DEFAULT_UP_MOVEMENT_MULTIPLIER				0.7f
	#define DEFAULT_GAME_SPEED							1.449990034f
	#define DEFAULT_MIN_GAME_SPEED						0.0000f
	#define DEFAULT_MAX_GAME_SPEED						5.0f
	#define HOTSAMPLE_FACTOR_MAX						5
	// End Mandatory constants

	// AOB Keys for interceptor's AOB scanner
	#define CAMERA_ADDRESS_INTERCEPT_KEY				"AOB_CAMERA_ADDRESS_INTERCEPT"
	#define CAMERA_WRITE_INTERCEPT_KEY					"AOB_CAMERA_WRITE_INTERCEPT"
	#define GAMESPEED_ADDRESS_INTERCEPT_KEY				"AOB_GAMESPEED_ADDRESS_INTERCEPT"
	#define TOD_ADDRESS_INTERCEPT_KEY					"AOB_TOD_ADDRESS_INTERCEPT"
	#define HOTSAMPLE_CODE_INTERCEPT_KEY				"AOB_HOTSAMPLE_CODE_INTERCEPT"

	// Indices in the structures read by interceptors 
	#define CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET		0x64
	#define LOOK_DATA_IN_CAMERA_STRUCT_OFFSET			0x70					// Rotation around X, Y and Z, in 2PI rad. 		
	#define FOV_IN_CAMERA_STRUCT_OFFSET					0x7C
	#define GAMESPEED_IN_STRUCT_OFFSET					0x84					// offset in code is 80, but it writes a double. We write a float.
	#define TOD_IN_STRUCT_OFFSET						0x0
}
