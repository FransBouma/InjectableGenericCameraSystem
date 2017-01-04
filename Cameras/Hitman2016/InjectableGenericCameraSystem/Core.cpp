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
void RegisterRawInput();
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
	g_consoleWrapper = &c;
	_hostImageAddress = (LPBYTE)hostBaseAddress;
	g_gamePad = new Gamepad(0);
	g_gamePad->setInvertLStickY(CONTROLLER_Y_INVERT);
	g_gamePad->setInvertRStickY(CONTROLLER_Y_INVERT);
	DisplayHelp();
	// initialization
	SetInputHooks();
	RegisterRawInput();
	SetCameraStructInterceptorHook(_hostImageAddress);
	SetTimestopInterceptorHook(_hostImageAddress);
	Keyboard::init(hostBaseAddress);
	Mouse::init(hostBaseAddress);
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
	Keyboard &keyboard = Keyboard::instance();
	Mouse &mouse = Mouse::instance();
	g_gamePad->update();
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
	if (keyboard.keyDown(IGCS_KEY_INVERT_Y_LOOK))
	{
		_lookDirectionInverter = -_lookDirectionInverter;
		if (_lookDirectionInverter < 0)
		{
			g_consoleWrapper->WriteLine("Y look direction is inverted");
		}
		else
		{
			g_consoleWrapper->WriteLine("Y look direction is normal");
		}
		// wait for 350ms to avoid fast keyboard hammering switching look directive multiple times
		Sleep(350);
	}

	if (keyboard.keyDown(IGCS_KEY_BLOCK_INPUT))
	{
		g_inputBlocked = !g_inputBlocked;
		if (g_inputBlocked)
		{
			g_consoleWrapper->WriteLine("Input to game is now blocked");
		}
		else
		{
			g_consoleWrapper->WriteLine("Input to game is now enabled");
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
		if (g_cameraEnabled)
		{
			RestoreOriginalCameraValues();
			g_consoleWrapper->WriteLine("Camera disabled");
		}
		else
		{
			CacheOriginalCameraValues();
			g_consoleWrapper->WriteLine("Camera enabled");
			_camera->ResetAngles();
		}
		g_cameraEnabled = g_cameraEnabled == 0 ? (byte)1 : (byte)0;
		// wait for 350ms to avoid fast keyboard hammering disabling the camera right away
		Sleep(350);
	}
	if (keyboard.keyDown(IGCS_KEY_TIMESTOP))
	{
		if (_timeStopped)
		{
			g_consoleWrapper->WriteLine("Game unpaused");
		}
		else
		{
			g_consoleWrapper->WriteLine("Game paused");
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
	if(!g_cameraEnabled)
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
			g_consoleWrapper->WriteLine("Camera movement unlocked");
		}
		else
		{
			g_consoleWrapper->WriteLine("Camera movement locked");
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


void RegisterRawInput()
{
	// get the main window of the host.
	RAWINPUTDEVICE rid[1];
	rid[0].usUsagePage = 0x01;
	rid[0].usUsage = 0x02;
	rid[0].dwFlags = 0; 
	rid[0].hwndTarget = FindMainWindow(GetCurrentProcessId());

	if (RegisterRawInputDevices(rid, 1, sizeof(rid[0])) == FALSE)
	{
		g_consoleWrapper->WriteError("Couldn't register raw input. Error code: " + to_string(GetLastError()));
	}
	else
	{
#ifdef _DEBUG
		g_consoleWrapper->WriteLine("Raw input registered");
#endif
	}
}


bool IsCameraEnabled()
{
	return g_cameraEnabled;
}

void DisplayHelp()
{
	                          //0         1         2         3         4         5         6         7
	                          //01234567890123456789012345678901234567890123456789012345678901234567890123456789
	g_consoleWrapper->WriteLine("---[IGCS Help]---------------------------------------------------------------", CONSOLE_WHITE);
	g_consoleWrapper->WriteLine("INS                                      : Enable/Disable camera");
	g_consoleWrapper->WriteLine("HOME                                     : Lock/unlock camera movement");
	g_consoleWrapper->WriteLine("ALT+rotate/move                          : Faster rotate / move");
	g_consoleWrapper->WriteLine("SHIFT+rotate/move                        : Slower rotate / move");
	g_consoleWrapper->WriteLine("Controller A-button + left/right-stick   : Faster rotate / move");
	g_consoleWrapper->WriteLine("Controller X-button + left/right-stick   : Slower rotate / move");
	g_consoleWrapper->WriteLine("Arrow up/down or mouse or right-stick    : Rotate camera up/down");
	g_consoleWrapper->WriteLine("Arrow left/right or mouse or right-stick : Rotate camera left/right");
	g_consoleWrapper->WriteLine("Numpad 8/Numpad 5 or left-stick          : Move camera forward/backward");
	g_consoleWrapper->WriteLine("Numpad 4/Numpad 6 or left-stick          : Move camera left / right");
	g_consoleWrapper->WriteLine("Numpad 7/Numpad 9 or left/right trigger  : Move camera up / down");
	g_consoleWrapper->WriteLine("Numpad 1/Numpad 3 or d-pad left/right    : Tilt camera left / right");
	g_consoleWrapper->WriteLine("Numpad +/Numpad - or d-pad up/down       : Increase / decrease FoV");
	g_consoleWrapper->WriteLine("Numpad * or controller B-button          : Reset FoV");
	g_consoleWrapper->WriteLine("Numpad 0                                 : Pause / Continue game");
	g_consoleWrapper->WriteLine("Numpad /                                 : Toggle Y look direction");
	g_consoleWrapper->WriteLine("Numpad .                                 : Block input to game");
	g_consoleWrapper->WriteLine("ALT+H                                    : This help");
	g_consoleWrapper->WriteLine("-----------------------------------------------------------------------------", CONSOLE_WHITE);
}
