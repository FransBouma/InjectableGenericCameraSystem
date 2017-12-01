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
	#define GAME_NAME									"Assassin's Creed Origins v1.05+"
	#define CAMERA_VERSION								"1.0.5"
	#define CAMERA_CREDITS								"Otis_Inf. Thanks to DeadEndThrills for HUD render location."
	#define GAME_WINDOW_TITLE							"Assassin's Creed Origins"
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
	#define RESOLUTION_SCALE_MAX						4.0f
	#define RESOLUTION_SCALE_MIN						0.5f
	// End Mandatory constants

	// AOB Keys for interceptor's AOB scanner
	#define CAMERA_ADDRESS_INTERCEPT_KEY				"AOB_CAMERA_ADDRESS_INTERCEPT"
	#define CAMERA_WRITE1_INTERCEPT_KEY					"AOB_CAMERA_WRITE1_INTERCEPT"
	#define CAMERA_WRITE2_INTERCEPT_KEY					"AOB_CAMERA_WRITE2_INTERCEPT"
	#define CAMERA_WRITE3_INTERCEPT_KEY					"AOB_CAMERA_WRITE3_INTERCEPT"
	#define CAMERA_WRITE4_INTERCEPT_KEY					"AOB_CAMERA_WRITE4_INTERCEPT"
	#define CAMERA_WRITE5_INTERCEPT_KEY					"AOB_CAMERA_WRITE5_INTERCEPT"
	#define FOV_WRITE1_INTERCEPT_KEY					"AOB_FOV_WRITE1_INTERCEPT"
	#define FOV_WRITE2_INTERCEPT_KEY					"AOB_FOV_WRITE2_INTERCEPT"
	#define FOV_READ_INTERCEPT_KEY						"AOB_FOV_READ_INTERCEPT"
	#define PHOTOMODE_ENABLE_ALWAYS_KEY					"AOB_PHOTOMODE_ENABLE_ALWAYS"
	#define RESOLUTION_SCALE_INTERCEPT_KEY				"AOB_RESOLUTION_SCALE_INTERCEPT"
	#define RESOLUTION_SCALE_MENU_KEY					"AOB_RESOLUTION_SCALE_MENU"
	#define TOD_WRITE_INTERCEPT_KEY						"AOB_TOD_WRITE_INTERCEPT"
	#define TIMESTOP_READ_INTERCEPT_KEY					"AOB_TIMESTOP_READ_INTERCEPT"
	#define HUD_RENDER_INTERCEPT_KEY					"AOB_HUD_RENDER_INTERCEPT"

	// Indices in the structures read by interceptors 
	#define COORDS_IN_STRUCT_OFFSET						0x60
	#define COORDS2_IN_STRUCT_OFFSET					0x2E0
	#define COORDS_IN_PM_STRUCT_OFFSET					0x470				
	#define QUATERNION_IN_STRUCT_OFFSET					0x70
	#define QUATERNION_IN_PM_STRUCT_OFFSET				0x480	
	#define FOV_IN_STRUCT_OFFSET						0x264
	#define RESOLUTION_SCALE_IN_STRUCT_OFFSET			0xA8
	#define TIMESTOP_IN_STRUCT_OFFSET					0x1458			// the original timestop flag. Don't set this directly. 
	#define TIMESTOP_SOURCE_IN_STRUCT_OFFSET			0x1460			// set to 1 to stop, -1 to continue
	#define TIMESTOP_CONTINUE_IN_STRUCT_OFFSET			0x1459			// after timestop source is set to -1, this will become 1. has to be set to 0. This can take a bit, so wait several 100ms
}
