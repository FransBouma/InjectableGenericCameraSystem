////////////////////////////////////////////////////////////////////////////////////////////////////////
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
#include "stdafx.h"
#include "System.h"
#include "Globals.h"
#include "Defaults.h"
#include "GameConstants.h"
#include "Gamepad.h"
#include "CameraManipulator.h"
#include "InterceptorHelper.h"
#include "InputHooker.h"
#include "input.h"
#include "CameraManipulator.h"


namespace IGCS
{
	using namespace IGCS::GameSpecific;

	System::System()
	{
	}


	System::~System()
	{
	}


	void System::start(LPBYTE hostBaseAddress, DWORD hostImageSize)
	{
		Globals::instance().systemActive(true);
		_hostImageAddress = hostBaseAddress;
		_hostImageSize = hostImageSize;
		Globals::instance().gamePad().setInvertLStickY(CONTROLLER_Y_INVERT);
		Globals::instance().gamePad().setInvertRStickY(CONTROLLER_Y_INVERT);
		displayHelp();
		initialize();		// will block till camera is found
		mainLoop();
	}


	// Core loop of the system
	void System::mainLoop()
	{
		while (Globals::instance().systemActive())
		{
			Sleep(FRAME_SLEEP);
			updateFrame();
		}
	}


	// updates the data and camera for a frame 
	void System::updateFrame()
	{
		handleUserInput();
		writeNewCameraValuesToCameraStructs();
	}
	

	void System::writeNewCameraValuesToCameraStructs()
	{
		if (!g_cameraEnabled)
		{
			return;
		}

		// calculate new camera values
		XMVECTOR newLookQuaternion = _camera.calculateLookQuaternion();
		XMFLOAT3 currentCoords = GameSpecific::CameraManipulator::getCurrentCameraCoords();
		XMFLOAT3 newCoords = _camera.calculateNewCoords(currentCoords, newLookQuaternion);
		GameSpecific::CameraManipulator::writeNewCameraValuesToGameData(newLookQuaternion, newCoords);
	}


	void System::handleUserInput()
	{
		Globals::instance().gamePad().update();
		bool altPressed = Input::keyDown(VK_LMENU) || Input::keyDown(VK_RMENU);
		bool controlPressed = Input::keyDown(VK_RCONTROL);

		if (Input::keyDown(IGCS_KEY_HELP) && altPressed)
		{
			displayHelp();
		}
		if (Input::keyDown(IGCS_KEY_INVERT_Y_LOOK))
		{
			toggleYLookDirectionState();
			Sleep(350);		// wait for 350ms to avoid fast keyboard hammering
		}
		if (!_cameraStructFound)
		{
			// camera not found yet, can't proceed.
			return;
		}
		if (Input::keyDown(IGCS_KEY_CAMERA_ENABLE))
		{
			if (g_cameraEnabled)
			{
				// it's going to be disabled, make sure things are alright when we give it back to the host
				CameraManipulator::restoreOriginalCameraValues();
				toggleCameraMovementLockState(false);
				toggleFreeze47AimState(0);
			}
			else
			{
				// it's going to be enabled, so cache the original values before we enable it so we can restore it afterwards
				CameraManipulator::cacheOriginalCameraValues();
				_camera.resetAngles();
			}
			g_cameraEnabled = g_cameraEnabled == 0 ? (byte)1 : (byte)0;
			displayCameraState();
			Sleep(350);				// wait for 350ms to avoid fast keyboard hammering
		}
		if (Input::keyDown(IGCS_KEY_TIMESTOP))
		{
			toggleTimestopState();
			Sleep(350);				// wait for 350ms to avoid fast keyboard hammering
		}
		if (Input::keyDown(IGCS_KEY_DECREASE_GAMESPEED))
		{
			modifyGameSpeed(true);
			Sleep(350);				// wait for 350ms to avoid fast keyboard hammering
		}
		if (Input::keyDown(IGCS_KEY_INCREASE_GAMESPEED))
		{
			modifyGameSpeed(false);
			Sleep(350);				// wait for 350ms to avoid fast keyboard hammering
		}
		if (Input::keyDown(IGCS_KEY_FOV_RESET))
		{
			CameraManipulator::resetFoV();
		}
		if (Input::keyDown(IGCS_KEY_FOV_DECREASE))
		{
			CameraManipulator::changeFoV(-DEFAULT_FOV_SPEED);
		}
		if (Input::keyDown(IGCS_KEY_FOV_INCREASE))
		{
			CameraManipulator::changeFoV(DEFAULT_FOV_SPEED);
		}
		if (Input::keyDown(IGCS_KEY_SUPERSAMPLING_DECREASE))
		{
			CameraManipulator::changeSupersampling(_hostImageAddress, -DEFAULT_SUPERSAMPLING_SPEED);
			Sleep(350);				// wait for 350ms to avoid fast keyboard hammering
		}
		if (Input::keyDown(IGCS_KEY_SUPERSAMPLING_INCREASE))
		{
			CameraManipulator::changeSupersampling(_hostImageAddress, DEFAULT_SUPERSAMPLING_SPEED);
			Sleep(350);				// wait for 350ms to avoid fast keyboard hammering
		}

		if (!g_cameraEnabled)
		{
			// camera is disabled. We simply disable all input to the camera movement, by returning now.
			return;
		}
		if (Input::keyDown(IGCS_KEY_BLOCK_INPUT))
		{
			toggleInputBlockState(!Globals::instance().inputBlocked());
			Sleep(350);				// wait for 350ms to avoid fast keyboard hammering
		}
		if (Input::keyDown(IGCS_KEY_FREEZE_47))
		{
			toggleFreeze47AimState(g_aimFrozen ? (byte)0 : (byte)1);
			Sleep(350);				// wait for 350ms to avoid fast keyboard hammering
		}

		_camera.resetMovement();
		float multiplier = altPressed ? FASTER_MULTIPLIER : controlPressed ? SLOWER_MULTIPLIER : 1.0f;
		if (Input::keyDown(IGCS_KEY_CAMERA_LOCK))
		{
			toggleCameraMovementLockState(!_cameraMovementLocked);
			Sleep(350);				// wait for 350ms to avoid fast keyboard hammering
		}
		if (_cameraMovementLocked)
		{
			// no movement allowed, simply return
			return;
		}

		handleKeyboardCameraMovement(multiplier);
		handleMouseCameraMovement(multiplier);
		handleGamePadMouseMovement(multiplier);
	}


	void System::handleGamePadMouseMovement(float multiplierBase)
	{
		auto gamePad = Globals::instance().gamePad();

		if (gamePad.isConnected())
		{
			float  multiplier = gamePad.isButtonPressed(IGCS_BUTTON_FASTER) ? FASTER_MULTIPLIER : gamePad.isButtonPressed(IGCS_BUTTON_SLOWER) ? SLOWER_MULTIPLIER : multiplierBase;

			vec2 rightStickPosition = gamePad.getRStickPosition();
			_camera.pitch(rightStickPosition.y * multiplier);
			_camera.yaw(rightStickPosition.x * multiplier);

			vec2 leftStickPosition = gamePad.getLStickPosition();
			_camera.moveForward(-leftStickPosition.y * multiplier);
			_camera.moveRight(leftStickPosition.x * multiplier);
			_camera.moveUp((gamePad.getLTrigger() - gamePad.getRTrigger()) * multiplier);

			if (gamePad.isButtonPressed(IGCS_BUTTON_TILT_LEFT))
			{
				_camera.roll(multiplier);
			}
			if (gamePad.isButtonPressed(IGCS_BUTTON_TILT_RIGHT))
			{
				_camera.roll(-multiplier);
			}
			if (gamePad.isButtonPressed(IGCS_BUTTON_RESET_FOV))
			{
				CameraManipulator::resetFoV();
			}
			if (gamePad.isButtonPressed(IGCS_BUTTON_FOV_DECREASE))
			{
				CameraManipulator::changeFoV(-DEFAULT_FOV_SPEED);
			}
			if (gamePad.isButtonPressed(IGCS_BUTTON_FOV_INCREASE))
			{
				CameraManipulator::changeFoV(DEFAULT_FOV_SPEED);
			}
		}
	}


	void System::handleMouseCameraMovement(float multiplier)
	{
		long mouseDeltaX = Input::getMouseDeltaX();
		long mouseDeltaY = Input::getMouseDeltaY();
		if (mouseDeltaY != 0)
		{
			_camera.pitch(static_cast<float>(mouseDeltaY) * MOUSE_SPEED_CORRECTION * multiplier);
		}
		if (mouseDeltaX != 0)
		{
			_camera.yaw(static_cast<float>(mouseDeltaX) * MOUSE_SPEED_CORRECTION * multiplier);
		}
	}


	void System::handleKeyboardCameraMovement(float multiplier)
	{
		if (Input::keyDown(IGCS_KEY_MOVE_FORWARD))
		{
			_camera.moveForward(multiplier);
		}
		if (Input::keyDown(IGCS_KEY_MOVE_BACKWARD))
		{
			_camera.moveForward(-multiplier);
		}
		if (Input::keyDown(IGCS_KEY_MOVE_RIGHT))
		{
			_camera.moveRight(multiplier);
		}
		if (Input::keyDown(IGCS_KEY_MOVE_LEFT))
		{
			_camera.moveRight(-multiplier);
		}
		if (Input::keyDown(IGCS_KEY_MOVE_UP))
		{
			_camera.moveUp(multiplier);
		}
		if (Input::keyDown(IGCS_KEY_MOVE_DOWN))
		{
			_camera.moveUp(-multiplier);
		}
		if (Input::keyDown(IGCS_KEY_ROTATE_DOWN))
		{
			_camera.pitch(multiplier);
		}
		if (Input::keyDown(IGCS_KEY_ROTATE_UP))
		{
			_camera.pitch(-multiplier);
		}
		if (Input::keyDown(IGCS_KEY_ROTATE_RIGHT))
		{
			_camera.yaw(multiplier);
		}
		if (Input::keyDown(IGCS_KEY_ROTATE_LEFT))
		{
			_camera.yaw(-multiplier);
		}
		if (Input::keyDown(IGCS_KEY_TILT_LEFT))
		{
			_camera.roll(multiplier);
		}
		if (Input::keyDown(IGCS_KEY_TILT_RIGHT))
		{
			_camera.roll(-multiplier);
		}
	}


	// Initializes system. Will block till camera struct is found.
	void System::initialize()
	{
		InputHooker::setInputHooks();
		GameSpecific::InterceptorHelper::initializeAOBBlocks(_hostImageAddress, _hostImageSize, _aobBlocks);
		GameSpecific::InterceptorHelper::setCameraStructInterceptorHook(_aobBlocks);
		GameSpecific::InterceptorHelper::setTimestopInterceptorHook(_aobBlocks);
		GameSpecific::CameraManipulator::waitForCameraStructAddresses();		// blocks till camera is found.
		// camera struct found, init our own camera object now and hook into game code which uses camera.
		_cameraStructFound = true;
		_camera.setPitch(INITIAL_PITCH_RADIANS);
		_camera.setRoll(INITIAL_ROLL_RADIANS);
		_camera.setYaw(INITIAL_YAW_RADIANS);
		// initialize the writes after the camera has been found and initialized, as they rely on the camera struct address.
		GameSpecific::InterceptorHelper::setCameraWriteInterceptorHooks(_aobBlocks);
	}
	

	void System::toggleInputBlockState(bool newValue)
	{
		if (Globals::instance().inputBlocked() == newValue)
		{
			// already in this state. Ignore
			return;
		}
		Globals::instance().inputBlocked(newValue);
		Console::WriteLine(newValue ? "Input to game blocked" : "Input to game enabled");
	}


	void System::toggleFreeze47AimState(byte newValue)
	{
		if (g_aimFrozen == newValue)
		{
			// already in this state. Ignore
			return;
		}
		g_aimFrozen = newValue;
		Console::WriteLine(g_aimFrozen ? "47's aim is now frozen" : "47's aim is now normal (relative to the camera)");
	}


	void System::toggleCameraMovementLockState(bool newValue)
	{
		if (_cameraMovementLocked == newValue)
		{
			// already in this state. Ignore
			return;
		}
		_cameraMovementLocked = newValue;
		Console::WriteLine(_cameraMovementLocked ? "Camera movement is locked" : "Camera movement is unlocked");
	}


	void System::toggleTimestopState()
	{
		_timeStopped = _timeStopped == 0 ? (byte)1 : (byte)0;
		Console::WriteLine(_timeStopped ? "Game paused" : "Game unpaused");
		CameraManipulator::setTimeStopValue(_timeStopped);
	}


	void System::displayCameraState()
	{
		Console::WriteLine(g_cameraEnabled ? "Camera enabled" : "Camera disabled");
	}


	void System::toggleYLookDirectionState()
	{
		_camera.toggleLookDirectionInverter();
		Console::WriteLine(_camera.lookDirectionInverter() < 0 ? "Y look direction is inverted" : "Y look direction is normal");
	}

	void System::modifyGameSpeed(bool decrease)
	{
		if (!_timeStopped)
		{
			return;
		}
		CameraManipulator::modifyGameSpeed(decrease);
	}

	void System::displayHelp()
	{
						  //0         1         2         3         4         5         6         7
						  //01234567890123456789012345678901234567890123456789012345678901234567890123456789
		Console::WriteLine("---[IGCS Help]-----------------------------------------------------------------", CONSOLE_WHITE);
		Console::WriteLine("INS                                   : Enable/Disable camera");
		Console::WriteLine("HOME                                  : Lock/unlock camera movement");
		Console::WriteLine("ALT+rotate/move                       : Faster rotate / move");
		Console::WriteLine("Right-CTRL+rotate/move                : Slower rotate / move");
		Console::WriteLine("Controller A-button + l/r-stick       : Faster rotate / move");
		Console::WriteLine("Controller X-button + l/r-stick       : Slower rotate / move");
		Console::WriteLine("Arrow up/down or mouse or r-stick     : Rotate camera up/down");
		Console::WriteLine("Arrow left/right or mouse or r-stick  : Rotate camera left/right");
		Console::WriteLine("Numpad 8/Numpad 5 or l-stick          : Move camera forward/backward");
		Console::WriteLine("Numpad 4/Numpad 6 or l-stick          : Move camera left / right");
		Console::WriteLine("Numpad 7/Numpad 9 or l/r-trigger      : Move camera up / down");
		Console::WriteLine("Numpad 1/Numpad 3 or d-pad left/right : Tilt camera left / right");
		Console::WriteLine("Numpad +/Numpad - or d-pad up/down    : Increase / decrease FoV (with camera)");
		Console::WriteLine("Numpad * or controller B-button       : Reset FoV (with camera)");
		Console::WriteLine("Numpad 0                              : Pause / Continue game");
		Console::WriteLine("F2                                    : Decrease Game speed (during pause)");
		Console::WriteLine("F3                                    : Increase Game speed (during pause)");
		Console::WriteLine("END                                   : (During game pause) Freeze 47's aim");
		Console::WriteLine("[                                     : Decrease Supersampling value");
		Console::WriteLine("]                                     : Increase Supersampling value (careful!)");
		Console::WriteLine("Numpad /                              : Toggle Y look direction");
		Console::WriteLine("Numpad .                              : Toggle block input to game");
		Console::WriteLine("ALT+H                                 : This help");
		Console::WriteLine("-------------------------------------------------------------------------------", CONSOLE_WHITE);
		// wait for 350ms to avoid fast keyboard hammering
		Sleep(350);
	}
}