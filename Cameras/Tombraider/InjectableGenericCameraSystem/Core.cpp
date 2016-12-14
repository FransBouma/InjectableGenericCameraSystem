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

using namespace std;

//--------------------------------------------------------------------------------------------------------------------------------
// data shared with asm functions. This is allocated here, 'C' style and not in some datastructure as passing that to 
// MASM is rather tedious. 
extern "C" {
	byte _cameraEnabled = 0;
	byte _timeStopped = 0;
}

//--------------------------------------------------------------------------------------------------------------------------------
// local data 
static Camera* _camera=NULL;
static Gamepad* _gamePad = NULL;
static Console* _consoleWrapper=NULL;
static LPBYTE _hostImageAddress = NULL;
static bool _cameraMovementLocked = false;
static bool _cameraStructFound = false;
static float _lookDirectionInverter = 1.0f;
static bool _alternativeTimeStopped = false;

//--------------------------------------------------------------------------------------------------------------------------------
// Local function definitions
void MainLoop();
void UpdateFrame();
void HandleUserInput();
void InitCamera();
void WriteNewCameraValuesToCameraStructs();
void DisplayHelp();

//--------------------------------------------------------------------------------------------------------------------------------
// Implementations
void SystemStart(HMODULE hostBaseAddress, Console c)
{
	g_systemActive = true;
	_consoleWrapper = &c;
	_hostImageAddress = (LPBYTE)hostBaseAddress;
	_gamePad = new Gamepad(0);
	_gamePad->setInvertLStickY(CONTROLLER_Y_INVERT);
	_gamePad->setInvertRStickY(CONTROLLER_Y_INVERT);
	DisplayHelp();
	// initialization
	SetCameraStructInterceptorHook(_hostImageAddress);
	SetTimestopInterceptorHook(_hostImageAddress);
	Keyboard::init(hostBaseAddress);
	Mouse::init(hostBaseAddress);
	InitCamera();
	SetCameraWriteInterceptorHooks(_hostImageAddress);
	DisableFoVWrite(_hostImageAddress);
	// done initializing, start main loop of camera. We won't return from this
	MainLoop();
	delete _gamePad;
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
	Keyboard &keyboard = Keyboard::instance();
	Mouse &mouse = Mouse::instance();
	_gamePad->update();
	keyboard.update();
	mouse.update();
	bool altPressed = keyboard.keyDown(Keyboard::KEY_LALT) || keyboard.keyDown(Keyboard::KEY_RALT);
	bool shiftPressed = keyboard.keyDown(Keyboard::KEY_LSHIFT) || keyboard.keyDown(Keyboard::KEY_RSHIFT);

	if (keyboard.keyDown(IGCS_KEY_HELP) && altPressed)
	{
		DisplayHelp();
		// wait for 350ms to avoid fast keyboard hammering displaying multiple times the help!
		Sleep(350);
	}
	if (keyboard.keyDown(IGCS_KEY_INVERT_Y_LOOK) && altPressed)
	{
		_lookDirectionInverter = -_lookDirectionInverter;
		if (_lookDirectionInverter < 0)
		{
			_consoleWrapper->WriteLine("Y look direction is inverted");
		}
		else
		{
			_consoleWrapper->WriteLine("Y look direction is normal");
		}
		// wait for 350ms to avoid fast keyboard hammering switching look directive multiple times
		Sleep(350);
	}
	if(!_cameraStructFound)
	{
		// camera not found yet, can't proceed.
		return;
	}
	if(keyboard.keyPressed(IGCS_KEY_CAMERA_ENABLE))
	{
		if (_cameraEnabled)
		{
			RestoreOriginalCameraValues();
			_consoleWrapper->WriteLine("Camera disabled");
		}
		else
		{
			CacheOriginalCameraValues();
			_consoleWrapper->WriteLine("Camera enabled");
			_camera->ResetAngles();
		}
		_cameraEnabled = _cameraEnabled == 0 ? (byte)1 : (byte)0;
		// wait for 350ms to avoid fast keyboard hammering disabling the camera right away
		Sleep(350);
	}
	if (keyboard.keyDown(IGCS_KEY_TIMESTOP))
	{
		if (_timeStopped)
		{
			_consoleWrapper->WriteLine("Game unpaused");
		}
		else
		{
			_consoleWrapper->WriteLine("Game paused");
		}
		_timeStopped = _timeStopped == 0 ? (byte)1 : (byte)0;
		SetTimeStopValue(_timeStopped);
		// wait 350 ms to avoid fast keyboard hammering unlocking/locking the timestop right away
		Sleep(350);
	}

	//////////////////////////////////////////////////// FOV
	if (keyboard.keyDown(IGCS_KEY_FOV_RESET))
	{
		ResetFoV();
	}
	if (keyboard.keyDown(IGCS_KEY_FOV_DECREASE))
	{
		ChangeFoV(-DEFAULT_FOV_SPEED);
	}
	if (keyboard.keyDown(IGCS_KEY_FOV_INCREASE))
	{
		ChangeFoV(DEFAULT_FOV_SPEED);
	}

	//////////////////////////////////////////////////// CAMERA
	if(!_cameraEnabled)
	{
		// camera is disabled. We simply disable all input to the camera movement. 
		return;
	}
	_camera->ResetMovement();
	float multiplier = altPressed ? FASTER_MULTIPLIER : shiftPressed ? SLOWER_MULTIPLIER : 1.0f;

	if (keyboard.keyDown(IGCS_KEY_CAMERA_LOCK))
	{
		if (_cameraMovementLocked)
		{
			_consoleWrapper->WriteLine("Camera movement unlocked");
		}
		else
		{
			_consoleWrapper->WriteLine("Camera movement locked");
		}
		_cameraMovementLocked = !_cameraMovementLocked;
		// wait 150 ms to avoid fast keyboard hammering unlocking/locking the camera movement right away
		Sleep(150);
	}
	if (_cameraMovementLocked)
	{
		// no movement allowed, simply return
		return;
	}
	if(keyboard.keyDown(IGCS_KEY_MOVE_FORWARD))
	{
		_camera->MoveForward(multiplier);
	}
	if(keyboard.keyDown(IGCS_KEY_MOVE_BACKWARD))
	{
		_camera->MoveForward(-multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_MOVE_RIGHT))
	{
		_camera->MoveRight(multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_MOVE_LEFT))
	{
		_camera->MoveRight(-multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_MOVE_UP))
	{
		_camera->MoveUp(multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_MOVE_DOWN))
	{
		_camera->MoveUp(-multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_ROTATE_DOWN))
	{
		_camera->Pitch(multiplier * _lookDirectionInverter);
	}
	if (keyboard.keyDown(IGCS_KEY_ROTATE_UP))
	{
		_camera->Pitch((-multiplier) * _lookDirectionInverter);
	}
	if (keyboard.keyDown(IGCS_KEY_ROTATE_RIGHT))
	{
		_camera->Yaw(multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_ROTATE_LEFT))
	{
		_camera->Yaw(-multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_TILT_LEFT))
	{
		_camera->Roll(multiplier);
	}
	if (keyboard.keyDown(IGCS_KEY_TILT_RIGHT))
	{
		_camera->Roll(-multiplier);
	}

	// mouse 
	_camera->Pitch(mouse.yPosRelative() * MOUSE_SPEED_CORRECTION * multiplier * _lookDirectionInverter);
	_camera->Yaw(mouse.xPosRelative() * MOUSE_SPEED_CORRECTION * multiplier);

	// gamepad
	if (_gamePad->isConnected())
	{
		multiplier = _gamePad->isButtonPressed(IGCS_BUTTON_FASTER) ? FASTER_MULTIPLIER : _gamePad->isButtonPressed(IGCS_BUTTON_SLOWER) ? SLOWER_MULTIPLIER : multiplier;

		vec2 rightStickPosition = _gamePad->getRStickPosition();
		_camera->Pitch(rightStickPosition.y * multiplier * _lookDirectionInverter);
		_camera->Yaw(rightStickPosition.x * multiplier);

		vec2 leftStickPosition = _gamePad->getLStickPosition();
		_camera->MoveForward(-leftStickPosition.y * multiplier);
		_camera->MoveRight(leftStickPosition.x * multiplier);
		_camera->MoveUp((_gamePad->getLTrigger() - _gamePad->getRTrigger()) * multiplier);

		if (_gamePad->isButtonPressed(IGCS_BUTTON_TILT_LEFT))
		{
			_camera->Roll(multiplier);
		}
		if (_gamePad->isButtonPressed(IGCS_BUTTON_TILT_RIGHT))
		{
			_camera->Roll(-multiplier);
		}
		if (_gamePad->isButtonPressed(IGCS_BUTTON_RESET_FOV))
		{
			ResetFoV();
		}
		if (_gamePad->isButtonPressed(IGCS_BUTTON_FOV_DECREASE))
		{
			ChangeFoV(-DEFAULT_FOV_SPEED);
		}
		if (_gamePad->isButtonPressed(IGCS_BUTTON_FOV_INCREASE))
		{
			ChangeFoV(DEFAULT_FOV_SPEED);
		}
	}
}


void WriteNewCameraValuesToCameraStructs()
{
	if (!_cameraEnabled)
	{
		return;
	}

	// calculate new camera values
	XMVECTOR newLookQuaternion = _camera->CalculateLookQuaternion();
	//XMFLOAT3 currentCoords = GetCurrentCameraCoords();
	XMFLOAT3 newCoords;// = _camera->CalculateNewCoords(currentCoords, newLookQuaternion);
	WriteNewCameraValuesToGameData(newLookQuaternion, newCoords);
}


void InitCamera()
{
	_consoleWrapper->WriteLine("Waiting for camera struct interception...");
	WaitForCameraStructAddresses();
	_cameraStructFound = true;
	_consoleWrapper->WriteLine("Camera found.");

	_camera = new Camera();
	_camera->SetPitch(INITIAL_PITCH_RADIANS);
	_camera->SetRoll(INITIAL_ROLL_RADIANS);
	_camera->SetYaw(INITIAL_YAW_RADIANS);
}


void DisplayHelp()
{
	                          //0         1         2         3         4         5         6         7
	                          //01234567890123456789012345678901234567890123456789012345678901234567890123456789
	_consoleWrapper->WriteLine("---[IGCS Help]---------------------------------------------------------------", CONSOLE_WHITE);
	_consoleWrapper->WriteLine("INS                                      : Enable/Disable camera");
	_consoleWrapper->WriteLine("HOME                                     : Lock/unlock camera movement");
	_consoleWrapper->WriteLine("ALT+rotate/move                          : Faster rotate / move");
	_consoleWrapper->WriteLine("SHIFT+rotate/move                        : Slower rotate / move");
	_consoleWrapper->WriteLine("Controller A-button + left/right-stick   : Faster rotate / move");
	_consoleWrapper->WriteLine("Controller X-button + left/right-stick   : Slower rotate / move");
	_consoleWrapper->WriteLine("Arrow up/down or mouse or right-stick    : Rotate camera up/down");
	_consoleWrapper->WriteLine("Arrow left/right or mouse or right-stick : Rotate camera left/right");
	_consoleWrapper->WriteLine("Numpad 8/Numpad 5 or left-stick          : Move camera forward/backward");
	_consoleWrapper->WriteLine("Numpad 4/Numpad 6 or left-stick          : Move camera left / right");
	_consoleWrapper->WriteLine("Numpad 7/Numpad 9 or left/right trigger  : Move camera up / down");
	_consoleWrapper->WriteLine("Numpad 1/Numpad 3 or d-pad left/right    : Tilt camera left / right");
	_consoleWrapper->WriteLine("Numpad +/Numpad - or d-pad up/down       : Increase / decrease FoV");
	_consoleWrapper->WriteLine("Numpad * or controller B-button          : Reset FoV");
	_consoleWrapper->WriteLine("Numpad 0                                 : Pause / Continue game");
	_consoleWrapper->WriteLine("Numpad .                                 : Alt Pause / Continue game (no FoV)");
	_consoleWrapper->WriteLine("ALT+Arrow up                             : Toggle Y look direction");
	_consoleWrapper->WriteLine("ALT+H                                    : This help");
	_consoleWrapper->WriteLine("-----------------------------------------------------------------------------", CONSOLE_WHITE);
}
