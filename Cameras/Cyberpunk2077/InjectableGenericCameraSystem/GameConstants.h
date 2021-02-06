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
	#define GAME_NAME									"Cyberpunk 2077 v1.0.11+"
	#define CAMERA_VERSION								"1.0.4"
	#define CAMERA_CREDITS								"Otis_Inf / Jim2Point0"
	#define GAME_WINDOW_TITLE							"Cyberpunk 2077"
	#define INITIAL_PITCH_RADIANS						0.0f	// around X axis	(right)
	#define INITIAL_YAW_RADIANS							0.0f	// around Y axis	(out of the screen)
	#define INITIAL_ROLL_RADIANS						0.0f	// around Z axis	(up)
	#define CONTROLLER_Y_INVERT							false
	// These will be overwritten by settings sent by the client. These defines are for initial usage. 
	#define FASTER_MULTIPLIER							5.0f
	#define SLOWER_MULTIPLIER							0.1f
	#define MOUSE_SPEED_CORRECTION						0.2f	// to correct for the mouse-deltas related to normal rotation.
	#define DEFAULT_MOVEMENT_SPEED						0.07f
	#define DEFAULT_ROTATION_SPEED						0.01f
	#define DEFAULT_FOV_SPEED							0.01f
	#define DEFAULT_FOV_DEGREES							80.0f
	#define DEFAULT_UP_MOVEMENT_MULTIPLIER				0.7f
	#define DEFAULT_TOD_CHANGE							0.01f
	// End Mandatory constants

	// AOB Keys for interceptor's AOB scanner
	#define PMSTRUCT_ADDRESS_INTERCEPT_KEY				"AOB_PMSTRUCT_ADDRESS_INTERCEPT"
	#define ACTIVECAM_ADDRESS_INTERCEPT_KEY				"AOB_ACTIVECAM_ADDRESS_INTERCEPT"
	#define ACTIVECAM_CAMERA_WRITE1_INTERCEPT_KEY		"AOB_ACTIVECAM_CAMERA_WRITE1_INTERCEPT"
	#define COORD_FACTOR_ADDRESS_KEY					"AOB_COORD_FACTOR_ADDRESS"
	#define RESOLUTION_STRUCT_ADDRESS_INTERCEPT_KEY		"AOB_RESOLUTION_STRUCT_ADDRESS_INTERCEPT"
	#define TOD_READ_INTERCEPT_KEY						"AOB_TOD_READ_INTERCEPT"
	#define PLAY_WIDGETBUCKET_READ_INTERCEPT_KEY		"AOB_PLAY_WIDGETBUCKET_READ_INTERCEPT"
	#define PM_WIDGETBUCKET_READ_INTERCEPT_KEY			"AOB_PM_WIDGETBUCKET_READ_INTERCEPT"
	#define FOV_PLAY_WRITE_INTERCEPT_KEY				"AOB_FOV_PLAY_WRITE_INTERCEPT"
	#define TIMESTOP_STRUCT_INTERCEPT_KEY				"AOB_TIMESTOP_STRUCT_INTERCEPT"
	#define WEATHER_STRUCT_INTERCEPT_KEY				"AOB_WEATHER_STRUCT_INTERCEPT"


	// Indices in the structures read by interceptors
	#define COORDS_IN_CAMSTRUCT_OFFSET					0xE0
	#define QUATERNION_IN_CAMSTRUCT_OFFSET				0xF0
	#define FOV_IN_FREECAMSTRUCT_OFFSET					0x128
	#define FOV_IN_PLAYCAMSTRUCT_OFFSET					0x37C
	#define WIDTH_IN_STRUCT_OFFSET						0x74
	#define HEIGHT_IN_STRUCT_OFFSET						0x78
	#define TOD_IN_STRUCT_OFFSET						0x34
	#define PM_ACTIVATED_BIT_IN_STRUCT_OFFSET			0x2FB
	#define HUD_TOGGLE_SWITCH_IN_BUCKETS_OFFSET			0x1E8
	#define TIMESTOP_BYTE_IN_STRUCT_OFFSET				0x1C
	#define WETNESS_FACTOR_IN_STRUCT_OFFSET				0xF0
	#define PUDDLE_SIZE_IN_STRUCT_OFFSET				0xF8
	#define MOISTURE_IN_STRUCT_OFFSET					0xD0
}
