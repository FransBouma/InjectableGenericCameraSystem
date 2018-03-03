////////////////////////////////////////////////////////////////////////////////////////////////////////
// Part of Injectable Generic Camera System
// Copyright(c) 2016, Frans Bouma
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

// Mandatory constants to define for a game
#define GAME_NAME									"Assassin's Creed 3 Single Player"
#define CAMERA_VERSION								"1.0.1"
#define CAMERA_CREDITS								"Otis_Inf. Thanks to Jim2Point0"
#define GAME_WINDOW_TITLE							"Assassin's Creed III"
#define INITIAL_PITCH_RADIANS						0.0f
#define INITIAL_YAW_RADIANS							0.0f
#define INITIAL_ROLL_RADIANS						0.0f
#define CONTROLLER_Y_INVERT							true
// End Mandatory constants

// Offsets for camera intercept code. Used in interceptor.
#define CAMERA_ADDRESS_INTERCEPT_START_OFFSET		0xCAF5
#define CAMERA_ADDRESS_INTERCEPT_CONTINUE_OFFSET	0xCAFD

#define	CAMERA_WRITE_INTERCEPT1_START_OFFSET		0x71EFC		
#define CAMERA_WRITE_INTERCEPT1_CONTINUE_OFFSET		0x71F08	

#define TIMESTOP_ADDRESS_INTERCEPT_START_OFFSET		0x7EF8A
#define TIMESTOP_ADDRESS_INTERCEPT_CONTINUE_OFFSET	0x7EF91		

// Indices in the structures read by interceptors 
#define LOOK_QUATERNION_IN_CAMERA_STRUCT_OFFSET		-0xC8
#define CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET		-0xD8
#define FOV_IN_CAMERA_STRUCT_OFFSET					0x8
#define TIMESTOP_IN_STRUCT_OFFSET					0x89C			

