#pragma once

#include "stdafx.h"
#include "input.h"

// Keyboard camera control
#define IGCS_KEY_CAMERA_ENABLE	Keyboard::KEY_INSERT
#define IGCS_KEY_ROTATE_RIGHT	Keyboard::KEY_RIGHT	// pitch
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
#define IGCS_KEY_FOV_INCREASE	Keyboard::KEY_NUMPAD_ADD
#define IGCS_KEY_FOV_DECREASE	Keyboard::KEY_NUMPAD_MINUS

static const byte jmpFarInstructionBytes[6] = { 0xff, 0x25, 0, 0, 0, 0 };	// instruction bytes for jmp qword ptr [0000]
