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

#include "stdafx.h"
#include "input.h"
#include "Gamepad.h"

// System defaults
#define FRAME_SLEEP				8		// in milliseconds
#define FASTER_MULTIPLIER		3.0f
#define SLOWER_MULTIPLIER		0.15f
#define MOUSE_SPEED_CORRECTION	0.1f	// to correct for the mouse-deltas related to normal rotation.
#define DEFAULT_MOVEMENT_SPEED	1.0f
#define DEFAULT_ROTATION_SPEED	0.01f
#define DEFAULT_FOV_RADIANS		0.7f
#define DEFAULT_FOV_DEGREES		40.0f
#define DEFAULT_FOV_SPEED		0.005f

// Keyboard camera control
#define IGCS_KEY_CAMERA_ENABLE	Keyboard::KEY_INSERT
#define IGCS_KEY_CAMERA_LOCK	Keyboard::KEY_HOME
#define IGCS_KEY_ROTATE_RIGHT	Keyboard::KEY_RIGHT		// pitch
#define IGCS_KEY_ROTATE_LEFT	Keyboard::KEY_LEFT
#define IGCS_KEY_ROTATE_UP		Keyboard::KEY_UP		// yaw
#define IGCS_KEY_ROTATE_DOWN	Keyboard::KEY_DOWN
#define IGCS_KEY_MOVE_FORWARD	Keyboard::KEY_NUMPAD_8
#define IGCS_KEY_MOVE_BACKWARD	Keyboard::KEY_NUMPAD_5
#define IGCS_KEY_MOVE_LEFT		Keyboard::KEY_NUMPAD_4
#define IGCS_KEY_MOVE_RIGHT		Keyboard::KEY_NUMPAD_6
#define IGCS_KEY_MOVE_UP		Keyboard::KEY_NUMPAD_7
#define IGCS_KEY_MOVE_DOWN		Keyboard::KEY_NUMPAD_9
#define IGCS_KEY_TILT_LEFT		Keyboard::KEY_NUMPAD_1		// roll
#define IGCS_KEY_TILT_RIGHT		Keyboard::KEY_NUMPAD_3
#define IGCS_KEY_FOV_RESET		Keyboard::KEY_NUMPAD_MULTIPLY
#define IGCS_KEY_FOV_INCREASE	Keyboard::KEY_NUMPAD_ADD
#define IGCS_KEY_FOV_DECREASE	Keyboard::KEY_NUMPAD_MINUS
#define IGCS_KEY_HELP			Keyboard::KEY_H
#define IGCS_KEY_TIMESTOP		Keyboard::KEY_NUMPAD_0
#define IGCS_KEY_INVERT_Y_LOOK	Keyboard::KEY_UP
#define IGCS_KEY_ALT_TIMESTOP	Keyboard::KEY_NUMPAD_PERIOD

#define IGCS_BUTTON_FOV_DECREASE	Gamepad::button_t::UP
#define IGCS_BUTTON_FOV_INCREASE	Gamepad::button_t::DOWN
#define IGCS_BUTTON_RESET_FOV		Gamepad::button_t::B
#define IGCS_BUTTON_TILT_LEFT		Gamepad::button_t::LEFT
#define IGCS_BUTTON_TILT_RIGHT		Gamepad::button_t::RIGHT
#define IGCS_BUTTON_FASTER			Gamepad::button_t::A
#define IGCS_BUTTON_SLOWER			Gamepad::button_t::X

static const byte jmpFarInstructionBytes[6] = { 0xff, 0x25, 0, 0, 0, 0 };	// instruction bytes for jmp qword ptr [0000]
