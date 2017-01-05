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
#include "stdafx.h"
#include "input.h"
#include "Camera.h"
#include "InterceptorHelper.h"
#include "Gamepad.h"
#include "InputHooker.h"

using namespace std;

//--------------------------------------------------------------------------------------------------------------------------------
// data shared with asm functions. This is allocated here, 'C' style and not in some datastructure as passing that to 
// MASM is rather tedious. 
extern "C" {
	byte g_cameraEnabled = 0;
	byte g_aimFrozen = 0;
}

//--------------------------------------------------------------------------------------------------------------------------------
// Global data
bool g_inputBlocked = false;
Gamepad* g_gamePad = nullptr;
Console* g_consoleWrapper = nullptr;

//--------------------------------------------------------------------------------------------------------------------------------
// local data 
static Camera* _camera=NULL;
static LPBYTE _hostImageAddress = NULL;
static bool _cameraMovementLocked = false;
static bool _cameraStructFound = false;
static float _lookDirectionInverter = 1.0f;
static bool _timeStopped = false;

//--------------------------------------------------------------------------------------------------------------------------------
// Local function definitions
void MainLoop();
void UpdateFrame();
void HandleUserInput();
void InitCamera();
void WriteNewCameraValuesToCameraStructs();
void DisplayHelp();
void DisplayTimestopState();
void DisplayCameraState();
void DisplayYLookDirectionState();
void ToggleCameraMovementLockState(bool newValue);
void ToggleInputBlockState(bool newValue);
void ToggleFreeze47AimState(byte newValue);

//--------------------------------------------------------------------------------------------------------------------------------
// Implementations
void SystemStart(HMODULE hostBaseAddress, Console c)
{
	g_systemActive = true;
	g_consoleWrapper = &c;
	_hostImageAddress = (LPBYTE)hostBaseAddress;
	g_gamePad = new Gamepad(0);
	g_gamePad->setInvertLStickY(CONTROLLER_Y_INVERT);
	g_gamePad->setInvertRStickY(CONTROLLER_Y_INVERT);
	DisplayHelp();
	// initialization
	SetInputHooks();
	SetCameraStructInterceptorHook(_hostImageAddress);
	SetTimestopInterceptorHook(_hostImageAddress);
	InitCamera();
	SetCameraWriteInterceptorHooks(_hostImageAddress);
	DisableFoVWrite(_hostImageAddress);
	// done initializing, start main loop of camera. We won't return from this
	MainLoop();
	delete g_gamePad;
}


// Core loop of the system
void MainLoop()
{
	while (g_systemActive)
	{
		Sleep(FRAME_SLEEP);
		UpdateFrame();
	}
}


// updates the data and camera for a frame 
void UpdateFrame()
{
	HandleUserInput();
	WriteNewCameraValuesToCameraStructs();
}


void HandleUserInput()
{
	g_gamePad->update();
	bool altPressed = KeyDown(VK_LMENU) || KeyDown(VK_RMENU);
	bool controlPressed = KeyDown(VK_RCONTROL);

	if (KeyDown(IGCS_KEY_HELP) && altPressed)
	{
		DisplayHelp();
		// wait for 350ms to avoid fast keyboard hammering displaying multiple times the help!
		Sleep(350);
	}
	if (KeyDown(IGCS_KEY_INVERT_Y_LOOK))
	{
		_lookDirectionInverter = -_lookDirectionInverter;
		DisplayYLookDirectionState();
		// wait for 350ms to avoid fast keyboard hammering switching look directive multiple times
		Sleep(350);
	}
	
	if(!_cameraStructFound)
	{
		// camera not found yet, can't proceed.
		return;
	}
	if(KeyDown(IGCS_KEY_CAMERA_ENABLE))
	{
		if(g_cameraEnabled)
		{
			// it's going to be disabled, make sure things are alright when we give it back to the host
			RestoreOriginalCameraValues();
			ToggleCameraMovementLockState(false);
			ToggleFreeze47AimState(0);
		}
		else
		{
			// it's going to be enabled, so cache the original values before we enable it so we can restore it afterwards
			CacheOriginalCameraValues();
			_camera->ResetAngles();
		}
		g_cameraEnabled = g_cameraEnabled == 0 ? (byte)1 : (byte)0;
		DisplayCameraState();
		// wait for 350ms to avoid fast keyboard hammering disabling the camera right away
		Sleep(350);
	}
	if (KeyDown(IGCS_KEY_TIMESTOP))
	{
		_timeStopped = _timeStopped == 0 ? (byte)1 : (byte)0;
		DisplayTimestopState();
		SetTimeStopValue(_timeStopped);
		// wait 350 ms to avoid fast keyboard hammering unlocking/locking the timestop right away
		Sleep(350);
	}

	//////////////////////////////////////////////////// FOV
	if (KeyDown(IGCS_KEY_FOV_RESET))
	{
		ResetFoV();
	}
	if (KeyDown(IGCS_KEY_FOV_DECREASE))
	{
		ChangeFoV(-DEFAULT_FOV_SPEED);
	}
	if (KeyDown(IGCS_KEY_FOV_INCREASE))
	{
		ChangeFoV(DEFAULT_FOV_SPEED);
	}

	//////////////////////////////////////////////////// CAMERA
	if(!g_cameraEnabled)
	{
		// camera is disabled. We simply disable all input to the camera movement. 
		return;
	}
	
	if (KeyDown(IGCS_KEY_BLOCK_INPUT))
	{
		ToggleInputBlockState(!g_inputBlocked);
		// wait for 350ms to avoid fast keyboard hammering switching look directive multiple times
		Sleep(350);
	}

	if (KeyDown(IGCS_KEY_FREEZE_47))
	{
		ToggleFreeze47AimState(g_aimFrozen ? (byte)0 : (byte)1);
		// wait for 350ms to avoid fast keyboard hammering
		Sleep(350);
	}

	_camera->ResetMovement();
	float multiplier = altPressed ? FASTER_MULTIPLIER : controlPressed ? SLOWER_MULTIPLIER : 1.0f;

	if (KeyDown(IGCS_KEY_CAMERA_LOCK))
	{
		ToggleCameraMovementLockState(!_cameraMovementLocked);
		// wait 150 ms to avoid fast keyboard hammering unlocking/locking the camera movement right away
		Sleep(150);
	}
	if (_cameraMovementLocked)
	{
		// no movement allowed, simply return
		return;
	}
	if(KeyDown(IGCS_KEY_MOVE_FORWARD))
	{
		_camera->MoveForward(multiplier);
	}
	if(KeyDown(IGCS_KEY_MOVE_BACKWARD))
	{
		_camera->MoveForward(-multiplier);
	}
	if (KeyDown(IGCS_KEY_MOVE_RIGHT))
	{
		_camera->MoveRight(multiplier);
	}
	if (KeyDown(IGCS_KEY_MOVE_LEFT))
	{
		_camera->MoveRight(-multiplier);
	}
	if (KeyDown(IGCS_KEY_MOVE_UP))
	{
		_camera->MoveUp(multiplier);
	}
	if (KeyDown(IGCS_KEY_MOVE_DOWN))
	{
		_camera->MoveUp(-multiplier);
	}
	if (KeyDown(IGCS_KEY_ROTATE_DOWN))
	{
		_camera->Pitch(multiplier * _lookDirectionInverter);
	}
	if (KeyDown(IGCS_KEY_ROTATE_UP))
	{
		_camera->Pitch((-multiplier) * _lookDirectionInverter);
	}
	if (KeyDown(IGCS_KEY_ROTATE_RIGHT))
	{
		_camera->Yaw(multiplier);
	}
	if (KeyDown(IGCS_KEY_ROTATE_LEFT))
	{
		_camera->Yaw(-multiplier);
	}
	if (KeyDown(IGCS_KEY_TILT_LEFT))
	{
		_camera->Roll(multiplier);
	}
	if (KeyDown(IGCS_KEY_TILT_RIGHT))
	{
		_camera->Roll(-multiplier);
	}

	// mouse 
	long mouseDeltaX = GetMouseDeltaX();
	long mouseDeltaY = GetMouseDeltaY();
	if (mouseDeltaY != 0)
	{
		_camera->Pitch(static_cast<float>(mouseDeltaY) * MOUSE_SPEED_CORRECTION * multiplier * _lookDirectionInverter);
	}
	if (mouseDeltaX != 0)
	{
		_camera->Yaw(static_cast<float>(mouseDeltaX) * MOUSE_SPEED_CORRECTION * multiplier);
	}

	// gamepad
	if (g_gamePad->isConnected())
	{
		multiplier = g_gamePad->isButtonPressed(IGCS_BUTTON_FASTER) ? FASTER_MULTIPLIER : g_gamePad->isButtonPressed(IGCS_BUTTON_SLOWER) ? SLOWER_MULTIPLIER : multiplier;

		vec2 rightStickPosition = g_gamePad->getRStickPosition();
		_camera->Pitch(rightStickPosition.y * multiplier * _lookDirectionInverter);
		_camera->Yaw(rightStickPosition.x * multiplier);

		vec2 leftStickPosition = g_gamePad->getLStickPosition();
		_camera->MoveForward(-leftStickPosition.y * multiplier);
		_camera->MoveRight(leftStickPosition.x * multiplier);
		_camera->MoveUp((g_gamePad->getLTrigger() - g_gamePad->getRTrigger()) * multiplier);

		if (g_gamePad->isButtonPressed(IGCS_BUTTON_TILT_LEFT))
		{
			_camera->Roll(multiplier);
		}
		if (g_gamePad->isButtonPressed(IGCS_BUTTON_TILT_RIGHT))
		{
			_camera->Roll(-multiplier);
		}
		if (g_gamePad->isButtonPressed(IGCS_BUTTON_RESET_FOV))
		{
			ResetFoV();
		}
		if (g_gamePad->isButtonPressed(IGCS_BUTTON_FOV_DECREASE))
		{
			ChangeFoV(-DEFAULT_FOV_SPEED);
		}
		if (g_gamePad->isButtonPressed(IGCS_BUTTON_FOV_INCREASE))
		{
			ChangeFoV(DEFAULT_FOV_SPEED);
		}
	}
}


void WriteNewCameraValuesToCameraStructs()
{
	if (!g_cameraEnabled)
	{
		return;
	}

	// calculate new camera values
	XMVECTOR newLookQuaternion = _camera->CalculateLookQuaternion();
	XMFLOAT3 currentCoords = GetCurrentCameraCoords();
	XMFLOAT3 newCoords = _camera->CalculateNewCoords(currentCoords, newLookQuaternion);
	WriteNewCameraValuesToGameData(newLookQuaternion, newCoords);
}


void InitCamera()
{
	g_consoleWrapper->WriteLine("Waiting for camera struct interception...");
	WaitForCameraStructAddresses();
	_cameraStructFound = true;
	g_consoleWrapper->WriteLine("Camera found.");

	_camera = new Camera();
	_camera->SetPitch(INITIAL_PITCH_RADIANS);
	_camera->SetRoll(INITIAL_ROLL_RADIANS);
	_camera->SetYaw(INITIAL_YAW_RADIANS);
}


bool IsCameraEnabled()
{
	return g_cameraEnabled;
}


void DisplayTimestopState()
{
	g_consoleWrapper->WriteLine(_timeStopped ? "Game paused" : "Game unpaused");
}


void DisplayCameraState()
{
	g_consoleWrapper->WriteLine(g_cameraEnabled ? "Camera enabled" : "Camera disabled");
}


void DisplayYLookDirectionState()
{
	g_consoleWrapper->WriteLine(_lookDirectionInverter < 0 ? "Y look direction is inverted" : "Y look direction is normal");
}


void ToggleInputBlockState(bool newValue)
{
	if (g_inputBlocked == newValue)
	{
		// already in this state. Ignore
		return;
	}
	g_inputBlocked = newValue;
	g_consoleWrapper->WriteLine(g_inputBlocked ? "Input to game blocked" : "Input to game enabled");
}


void ToggleFreeze47AimState(byte newValue)
{
	if (g_aimFrozen == newValue)
	{
		// already in this state. Ignore
		return;
	}
	g_aimFrozen = newValue;
	g_consoleWrapper->WriteLine(g_aimFrozen ? "47's aim is now frozen" : "47's aim is now normal (relative to the camera)");
}


void ToggleCameraMovementLockState(bool newValue)
{
	if (_cameraMovementLocked == newValue)
	{
		// already in this state. Ignore
		return;
	}
	_cameraMovementLocked = newValue;
	g_consoleWrapper->WriteLine(_cameraMovementLocked ? "Camera movement is locked" : "Camera movement is unlocked");
}


void DisplayHelp()
{
	                          //0         1         2         3         4         5         6         7
	                          //01234567890123456789012345678901234567890123456789012345678901234567890123456789
	g_consoleWrapper->WriteLine("---[IGCS Help]---------------------------------------------------------------", CONSOLE_WHITE);
	g_consoleWrapper->WriteLine("INS                                   : Enable/Disable camera");
	g_consoleWrapper->WriteLine("HOME                                  : Lock/unlock camera movement");
	g_consoleWrapper->WriteLine("ALT+rotate/move                       : Faster rotate / move");
	g_consoleWrapper->WriteLine("Right-CTRL+rotate/move                : Slower rotate / move");
	g_consoleWrapper->WriteLine("Controller A-button + l/r-stick       : Faster rotate / move");
	g_consoleWrapper->WriteLine("Controller X-button + l/r-stick       : Slower rotate / move");
	g_consoleWrapper->WriteLine("Arrow up/down or mouse or r-stick     : Rotate camera up/down");
	g_consoleWrapper->WriteLine("Arrow left/right or mouse or r-stick  : Rotate camera left/right");
	g_consoleWrapper->WriteLine("Numpad 8/Numpad 5 or l-stick          : Move camera forward/backward");
	g_consoleWrapper->WriteLine("Numpad 4/Numpad 6 or l-stick          : Move camera left / right");
	g_consoleWrapper->WriteLine("Numpad 7/Numpad 9 or l/r-trigger      : Move camera up / down");
	g_consoleWrapper->WriteLine("Numpad 1/Numpad 3 or d-pad left/right : Tilt camera left / right");
	g_consoleWrapper->WriteLine("Numpad +/Numpad - or d-pad up/down    : Increase / decrease FoV");
	g_consoleWrapper->WriteLine("Numpad * or controller B-button       : Reset FoV");
	g_consoleWrapper->WriteLine("Numpad 0                              : Pause / Continue game");
	g_consoleWrapper->WriteLine("Numpad /                              : Toggle Y look direction");
	g_consoleWrapper->WriteLine("Numpad .                              : Block input to game");
	g_consoleWrapper->WriteLine("END                                   : (During game pause) Freeze 47's aim");
	g_consoleWrapper->WriteLine("ALT+H                                 : This help");
	g_consoleWrapper->WriteLine("-----------------------------------------------------------------------------", CONSOLE_WHITE);
}
