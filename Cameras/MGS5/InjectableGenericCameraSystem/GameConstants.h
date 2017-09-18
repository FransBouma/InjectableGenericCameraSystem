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
	#define GAME_NAME									"Metal Gear Solid V: The Phantom Pain v1.0.12+"
	#define CAMERA_VERSION								"1.0.0"
	#define CAMERA_CREDITS								"Otis_Inf and Jan Schatter. Special thanks to tinmantex and Hodgedogs"
	#define GAME_WINDOW_TITLE							"METAL GEAR SOLID V: THE PHANTOM PAIN"
	#define INITIAL_PITCH_RADIANS						0.0f	// around X axis	(right)
	#define INITIAL_YAW_RADIANS							0.0f	// around Y axis	(into the screen)
	#define INITIAL_ROLL_RADIANS						0.0f	// aruond Z axis	(up)
	#define CONTROLLER_Y_INVERT							false
	#define FASTER_MULTIPLIER							10.0f
	#define SLOWER_MULTIPLIER							0.1f
	#define MOUSE_SPEED_CORRECTION						0.2f	// to correct for the mouse-deltas related to normal rotation.
	#define DEFAULT_MOVEMENT_SPEED						0.01f
	#define DEFAULT_ROTATION_SPEED						0.008f
	#define DEFAULT_FOV_SPEED							0.12f
	#define DEFAULT_UP_MOVEMENT_MULTIPLIER				0.7f
	// End Mandatory constants

	// AOB Keys for interceptor's AOB scanner
	#define CAMERA_ADDRESS_INTERCEPT_KEY				"AOB_CAMERA_ADDRESS_INTERCEPT"
	#define CAMERA_CUTSCENE_ADDRESS_INTERCEPT_KEY		"AOB_CAMERA_CUTSCENE_ADDRESS_INTERCEPT"
	#define CAMERA_CUTSCENE_WRITE1_INTERCEPT_KEY		"AOB_CAMERA_CUTSCENE_WRITE1_INTERCEPT"
	#define FOV_WRITE_INTERCEPT_KEY						"AOB_FOV_WRITE_INTERCEPT"
	#define GAMESPEED_WRITE_INTERCEPT_KEY				"AOB_GAMESPEED_WRITE_INTERCEPT"
	#define	TIMESTOP_READ_INTERCEPT_KEY					"AOB_TIMESTOP_READ_INTERCEPT"
	#define FOV_WRITE1_CUTSCENE_INTERCEPT_KEY			"AOB_FOV_WRITE1_CUTSCENE_INTERCEPT"
	#define FOV_WRITE2_CUTSCENE_INTERCEPT_KEY			"AOB_FOV_WRITE2_CUTSCENE_INTERCEPT"
	#define DOF_ADDRESS_INTERCEPT_KEY					"AOB_DOF_ADDRESS_INTERCEPT"
	#define DOF_WRITE_INTERCEPT_KEY						"AOB_DOF_WRITE_INTERCEPT"

	// Indices in the structures read by interceptors 
	#define QUATERNION_IN_STRUCT_OFFSET					0xF0
	#define COORDS_IN_STRUCT_OFFSET						0x100
	#define QUATERNION_CUTSCENE_IN_STRUCT_OFFSET		0x120
	#define COORDS_CUTSCENE_IN_STRUCT_OFFSET			0x130
	#define FOV_IN_STRUCT_OFFSET						0xC
	#define FOV_CUTSCENE_IN_STRUCT_OFFSET				0x3C
	#define GAMESPEED_IN_STRUCT_OFFSET					0xC
	#define TIMESTOP_IN_STRUCT_OFFSET					0x38
	#define DOF_FOCUS_DISTANCE_IN_STRUCT_OFFSET			0x108
	#define DOF_FOCAL_LENGTH_IN_STRUCT_OFFSET			0x110
	#define DOF_APERTURE_IN_STRUCT_OFFSET				0x124
	#define DOF_CONTROL_IN_STRUCT_OFFSET				0x50
}
