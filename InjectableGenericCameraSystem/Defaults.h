#pragma once

#include "stdafx.h"
#include "InputHandler.h"

// Keyboard camera control
#define IGCS_KEY_CAMERA_ENABLE	DIK_INSERT
#define IGCS_KEY_ROTATE_RIGHT	DIK_RIGHTARROW	// pitch
#define IGCS_KEY_ROTATE_LEFT	DIK_LEFTARROW
#define IGCS_KEY_ROTATE_UP		DIK_UPARROW		// yaw
#define IGCS_KEY_ROTATE_DOWN	DIK_DOWNARROW
#define IGCS_KEY_MOVE_FORWARD	DIK_NUMPAD8
#define IGCS_KEY_MOVE_BACKWARD	DIK_NUMPAD5
#define IGCS_KEY_MOVE_LEFT		DIK_NUMPAD4
#define IGCS_KEY_MOVE_RIGHT		DIK_NUMPAD6
#define IGCS_KEY_MOVE_UP		DIK_NUMPAD7
#define IGCS_KEY_MOVE_DOWN		DIK_NUMPAD9
#define IGCS_KEY_TILT_LEFT		DIK_NUMPAD1		// roll
#define IGCS_KEY_TILT_RIGHT		DIK_NUMPAD3
#define IGCS_KEY_FOV_INCREASE	DIK_NUMPADPLUS
#define IGCS_KEY_FOV_DECREASE	DIK_NUMPADMINUS

static const byte jmpFarInstructionBytes[6] = { 0xff, 0x25, 0, 0, 0, 0 };	// instruction bytes for jmp qword ptr [0000]
