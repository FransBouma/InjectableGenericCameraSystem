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
#define GAME_NAME									"Tomb Raider v1.1.748"
#define CAMERA_VERSION								"1.0.0"
#define CAMERA_CREDITS								"Otis_Inf, Jim2Point0"
#define INITIAL_PITCH_RADIANS						(-90.0f * XM_PI) / 180.f	// World has Z up and Y out of the screen, so rotate around X (pitch) -90 degrees.
#define INITIAL_YAW_RADIANS							0.0f
#define INITIAL_ROLL_RADIANS						0.0f
#define CONTROLLER_Y_INVERT							true
// End Mandatory constants

// Offsets for camera intercept code. Used in interceptor.
#define CAMERA_ADDRESS_INTERCEPT_START_OFFSET		0x8BBA9		
#define CAMERA_ADDRESS_INTERCEPT_CONTINUE_OFFSET	0x8BBB0		

#define	CAMERA_WRITE_INTERCEPT1_START_OFFSET		0x11DE56		
#define CAMERA_WRITE_INTERCEPT1_CONTINUE_OFFSET		0x11DE77		
#define	CAMERA_WRITE_INTERCEPT2_START_OFFSET		0x5D4458		
#define CAMERA_WRITE_INTERCEPT2_CONTINUE_OFFSET		0x5D44DC		

#define GAMESPEED_ADDRESS_INTERCEPT_START_OFFSET	0x41C897B		
#define GAMESPEED_ADDRESS_INTERCEPT_CONTINUE_OFFSET 0x41C898B		

#define FOV_WRITE_INTERCEPT1_START_OFFSET			0x44E4887		
#define FOV_WRITE_INTERCEPT2_START_OFFSET			0x44E48A9		

// Indices in the structures read by interceptors 
#define LOOK_QUATERNION_IN_CAMERA_STRUCT_OFFSET		0x90			
#define CAMERA_COORDS_IN_CAMERA_STRUCT_OFFSET		-0x30			
#define FOV_IN_CAMERA_STRUCT_OFFSET					0x17C
#define GAMESPEED_IN_STRUCT_OFFSET					0x48			

