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
	//
	// Game coordinate system: Z up, Y left, X into the screen
	//
	//
	#define GAME_NAME									"Titanfall 2 v1.0+"
	#define CAMERA_VERSION								"1.0.0"
	#define CAMERA_CREDITS								"Otis_Inf."
	#define GAME_WINDOW_TITLE							"Titanfall 2"
	#define INITIAL_PITCH_RADIANS						0.0f	// around Y axis	(Y is left, so rotate up/down).
	#define INITIAL_YAW_RADIANS							0.0f	// around Z axis	(z is up, so rotate left/right).
	#define INITIAL_ROLL_RADIANS						0.0f	// around X axis	(X is into the screen, so tilt right/left).
	#define CONTROLLER_Y_INVERT							false
	#define FASTER_MULTIPLIER							5.0f
	#define SLOWER_MULTIPLIER							0.1f
	#define MOUSE_SPEED_CORRECTION						0.2f	// to correct for the mouse-deltas related to normal rotation.
	#define DEFAULT_MOVEMENT_SPEED						2.0f
	#define DEFAULT_ROTATION_SPEED						0.01f
	#define DEFAULT_FOV_SPEED							0.005f
	#define DEFAULT_UP_MOVEMENT_MULTIPLIER				0.7f
	// End Mandatory constants

	// AOB Keys for interceptor's AOB scanner
	#define CAMERA_ADDRESS_INTERCEPT_KEY				"AOB_CAMERA_ADDRESS_INTERCEPT"
	#define CAMERA_WRITE1_INTERCEPT_KEY					"AOB_CAMERA_WRITE1_INTERCEPT"
	#define CAMERA_WRITE2_INTERCEPT_KEY					"AOB_CAMERA_WRITE2_INTERCEPT"
	#define CAMERA_WRITE3_INTERCEPT_KEY					"AOB_CAMERA_WRITE3_INTERCEPT"
	#define CAMERA_WRITE4_INTERCEPT_KEY					"AOB_CAMERA_WRITE4_INTERCEPT"
	#define CAMERA_WRITE5_INTERCEPT_KEY					"AOB_CAMERA_WRITE5_INTERCEPT"
	#define CAMERA_ANGLE_WRITE_PITCH_KEY				"AOB_CAMERA_ANGLE_WRITE_PITCH"
	#define CAMERA_ANGLE_WRITE_YAW_KEY					"AOB_CAMERA_ANGLE_WRITE_YAW"
	#define CAMERA_ANGLE_WRITE_ROLL_KEY					"AOB_CAMERA_ANGLE_WRITE_ROLL"
	#define FOV_MIN_CLAMP_LOCATION_KEY					"AOB_FOV_MIN_CLAMP_LOCATION"
	#define FOV_MAX_CLAMP_LOCATION_KEY					"AOB_FOV_MAX_CLAMP_LOCATION"
	#define FOV_ADDRESS_LOCATION_KEY					"AOB_FOV_ADDRESS_LOCATION"
	#define DRAWVIEWMODEL_LOCATION_KEY					"AOB_DRAWVIEWMODEL_LOCATION"
	#define PAUSE_UNPAUSE_ROOTOBJECT_LOCATION_KEY		"AOB_PAUSE_UNPAUSE_ROOTOBJECT"

	// Indices in the structures read by interceptors 
	#define COORDS_IN_STRUCT_OFFSET						0x0
	#define FOV_IN_STRUCT_OFFSET						0x58
}
