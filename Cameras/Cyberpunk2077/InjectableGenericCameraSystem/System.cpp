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
#include "MinHook.h"
#include "NamedPipeManager.h"
#include "MessageHandler.h"

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
		_hostImageAddress = (LPBYTE)hostBaseAddress;
		_hostImageSize = hostImageSize;
		filesystem::path hostExeFilenameAndPath = Utils::obtainHostExeAndPath();
		_hostExeFilename = hostExeFilenameAndPath.stem();
		_hostExePath = hostExeFilenameAndPath.parent_path();
		Globals::instance().gamePad().setInvertLStickY(CONTROLLER_Y_INVERT);
		Globals::instance().gamePad().setInvertRStickY(CONTROLLER_Y_INVERT);
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
		CameraManipulator::updateCameraDataInGameData(_camera);
	}


	bool System::checkIfGameHasFocus()
	{
		HWND currentForegroundWindow = GetForegroundWindow();
		return (currentForegroundWindow == Globals::instance().mainWindowHandle());
	}


	void System::onCameraDisabled()
	{
		CameraManipulator::restoreOriginalValuesAfterCameraDisable();
		toggleCameraMovementLockState(false);
	}

	void System::onCameraEnabled()
	{
		CameraManipulator::cacheOriginalValuesBeforeCameraEnable();
		_camera.resetAngles();
	}

	void System::handleUserInput()
	{
		if (!checkIfGameHasFocus())
		{
			// our window isn't focused, exit
			return;
		}
		
		Globals::instance().gamePad().update();
		if (_applyHammerPrevention)
		{
			_applyHammerPrevention = false;
			// sleep main thread for 300ms so key repeat delay is simulated. 
			Sleep(300);
		}

		if (!_cameraStructFound)
		{
			// camera not found yet, can't proceed.
			return;
		}

		// If we've changed settings by a keyboard setting, we'll apply them here. 
		CameraManipulator::applySettingsToGameState();

#ifdef _DEBUG
		if(Utils::keyDown(VK_END))
		{
			CameraManipulator::displayDebugInfo();
			_applyHammerPrevention = true;
		}
#endif
		
		if (Input::isActionActivated(ActionType::CameraEnable))
		{
			if (g_cameraEnabled)
			{
				// it's going to be disabled, make sure things are alright when we give it back to the host
				onCameraDisabled();
			}
			else
			{
				// it's going to be enabled, so cache the original values before we enable it so we can restore it afterwards
				onCameraEnabled();
			}
			g_cameraEnabled = g_cameraEnabled == 0 ? (uint8_t)1 : (uint8_t)0;
			displayCameraState();
			_applyHammerPrevention = true;
		}

		if (Input::isActionActivated(ActionType::TimeOfDayEarlier, true))
		{
			Globals::instance().settings().changeTimeOfDay(-DEFAULT_TOD_CHANGE * (Utils::altPressed() ? 0.1f : 1.0f));
		}
		if (Input::isActionActivated(ActionType::TimeOfDayLater, true))
		{
			Globals::instance().settings().changeTimeOfDay(DEFAULT_TOD_CHANGE * (Utils::altPressed() ? 0.1f : 1.0f));
		}
		if (Input::isActionActivated(ActionType::FovReset) && Globals::instance().keyboardMouseControlCamera())
		{
			CameraManipulator::resetFoV();
		}
		if (Input::isActionActivated(ActionType::FovDecrease) && Globals::instance().keyboardMouseControlCamera())
		{
			CameraManipulator::changeFoV(-Globals::instance().settings().fovChangeSpeed);
		}
		if (Input::isActionActivated(ActionType::FovIncrease) && Globals::instance().keyboardMouseControlCamera())
		{
			CameraManipulator::changeFoV(Globals::instance().settings().fovChangeSpeed);
		}

		if (!g_cameraEnabled)
		{
			// camera is disabled. We simply disable all input to the camera movement, by returning now.
			return;
		}
		if (Input::isActionActivated(ActionType::BlockInput))
		{
			toggleInputBlockState(!Globals::instance().inputBlocked());
			_applyHammerPrevention = true;
		}
		_camera.resetMovement();
		Settings& settings = Globals::instance().settings();
		if (Input::isActionActivated(ActionType::CameraLock)) 
		{
			toggleCameraMovementLockState(!_cameraMovementLocked);
			_applyHammerPrevention = true;
		}
		if (_cameraMovementLocked)
		{
			// no movement allowed, simply return
			return;
		}

		bool altPressed = Utils::altPressed();
		bool controlPressed = Utils::ctrlPressed();
		float multiplier = altPressed ? settings.fastMovementMultiplier : controlPressed ? settings.slowMovementMultiplier : 1.0f;
		// Calculates a multiplier based on the current fov. We have a baseline of DEFAULT_FOV. If the fov is > than that, use 1.0
		// otherwise calculate a factor by using the currentfov / DEFAULT_FOV. Cap the minimum at 0.1 so some movement is still possible :)
		multiplier *= Utils::clamp(abs(CameraManipulator::getCurrentFoV()) / DEFAULT_FOV_DEGREES, 0.01f, 1.0f);
		_applyHammerPrevention |= handleKeyboardCameraMovement(multiplier);
		handleMouseCameraMovement(multiplier);
		handleGamePadMovement(multiplier);
	}


	void System::handleGamePadMovement(float multiplierBase)
	{
		if(!Globals::instance().controllerControlsCamera())
		{
			return;
		}

		auto gamePad = Globals::instance().gamePad();

		if (gamePad.isConnected())
		{
			Settings& settings = Globals::instance().settings();
			float  multiplier = gamePad.isButtonPressed(IGCS_BUTTON_FASTER) ? settings.fastMovementMultiplier 
																			: gamePad.isButtonPressed(IGCS_BUTTON_SLOWER) ? settings.slowMovementMultiplier : multiplierBase;
			vec2 rightStickPosition = gamePad.getRStickPosition();
			_camera.pitch(rightStickPosition.y * multiplier);
			_camera.yaw(rightStickPosition.x * multiplier);

			vec2 leftStickPosition = gamePad.getLStickPosition();
			_camera.moveUp((gamePad.getLTrigger() - gamePad.getRTrigger()) * multiplier);
			_camera.moveForward(leftStickPosition.y * multiplier);
			_camera.moveRight(leftStickPosition.x * multiplier);

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
				CameraManipulator::changeFoV(-Globals::instance().settings().fovChangeSpeed);
			}
			if (gamePad.isButtonPressed(IGCS_BUTTON_FOV_INCREASE))
			{
				CameraManipulator::changeFoV(Globals::instance().settings().fovChangeSpeed);
			}
		}
	}


	void System::handleMouseCameraMovement(float multiplier)
	{
		if (!Globals::instance().keyboardMouseControlCamera())
		{
			return;
		}
		long mouseDeltaX = Input::getMouseDeltaX();
		long mouseDeltaY = Input::getMouseDeltaY();
		bool leftButtonPressed = Input::isMouseButtonDown(0);
		bool rightButtonPressed = Input::isMouseButtonDown(1);
		bool noButtonPressed = !(leftButtonPressed || rightButtonPressed);
		if (abs(mouseDeltaY) > 1)
		{
			float yValue = (static_cast<float>(mouseDeltaY) * MOUSE_SPEED_CORRECTION * multiplier);
			if (noButtonPressed)
			{
				_camera.pitch(-yValue);
			}
			else
			{
				if (leftButtonPressed)
				{
					// move up / down
					_camera.moveUp(-yValue);
				}
				else
				{
					// forward/backwards
					_camera.moveForward(-yValue);
				}
			}
		}
		if (abs(mouseDeltaX) > 1)
		{
			float xValue = static_cast<float>(mouseDeltaX) * MOUSE_SPEED_CORRECTION * multiplier;
			if (noButtonPressed)
			{
				_camera.yaw(xValue);
			}
			else
			{
				// if both buttons are pressed: do tilt
				if (leftButtonPressed && rightButtonPressed)
				{
					_camera.roll(-xValue);
				}
				else
				{
					// always left/right
					_camera.moveRight(xValue);
				}
			}
		}
		short mouseWheelDelta = Input::getMouseWheelDelta();
		if (abs(mouseWheelDelta) > 0)
		{
			MessageHandler::logDebug("Changing for with mouse delta: %d", mouseWheelDelta);
			CameraManipulator::changeFoV(-(static_cast<float>(mouseWheelDelta) * Globals::instance().settings().fovChangeSpeed));
		}
		Input::resetMouseDeltas();
	}


	bool System::handleKeyboardCameraMovement(float multiplier)
	{
		if (!Globals::instance().keyboardMouseControlCamera())
		{
			return false;
		}
		bool altPressed = Utils::altPressed();
		bool applyHammerPrevention = false;
		if (Input::isActionActivated(ActionType::ResetTilt, true))
		{
			_camera.setRoll(0.0f);
		}
		if (Input::isActionActivated(ActionType::MoveForward, true))
		{
			_camera.moveForward(multiplier);
		}
		if (Input::isActionActivated(ActionType::MoveBackward, true))
		{
			_camera.moveForward(-multiplier);
		}
		if (Input::isActionActivated(ActionType::MoveRight, true))
		{
			_camera.moveRight(multiplier);
		}
		if (Input::isActionActivated(ActionType::MoveLeft, true))
		{
			_camera.moveRight(-multiplier);
		}
		if (Input::isActionActivated(ActionType::MoveUp, true))
		{
			_camera.moveUp(multiplier);
		}
		if (Input::isActionActivated(ActionType::MoveDown, true))
		{
			_camera.moveUp(-multiplier);
		}
		if (Input::isActionActivated(ActionType::RotateDown, true))
		{
			_camera.pitch(-multiplier);
		}
		if (Input::isActionActivated(ActionType::RotateUp, true))
		{
			_camera.pitch(multiplier);
		}
		if (Input::isActionActivated(ActionType::RotateRight, true))
		{
			_camera.yaw(multiplier);
		}
		if (Input::isActionActivated(ActionType::RotateLeft, true))
		{
			_camera.yaw(-multiplier);
		}
		if (Input::isActionActivated(ActionType::TiltLeft, true))
		{
			if (altPressed)
			{
				_camera.setRoll(_camera.getRoll() + (0.5 * DirectX::XM_PI));
				applyHammerPrevention = true;
			}
			else
			{
				_camera.roll(multiplier);
			}
		}
		if (Input::isActionActivated(ActionType::TiltRight, true))
		{
			if (altPressed)
			{
				_camera.setRoll(_camera.getRoll() - (0.5 * DirectX::XM_PI));
				applyHammerPrevention = true;
			}
			else
			{
				_camera.roll(-multiplier);
			}
		}
		return applyHammerPrevention;
	}


	// Initializes system. Will block till camera struct is found.
	void System::initialize()
	{
		MH_Initialize();
		// first grab the window handle
		Globals::instance().mainWindowHandle(Utils::findMainWindow(GetCurrentProcessId()));
		NamedPipeManager::instance().connectDllToClient();
		NamedPipeManager::instance().startListening();
		InputHooker::setInputHooks();
		Input::registerRawInput();

		GameSpecific::InterceptorHelper::initializeAOBBlocks(_hostImageAddress, _hostImageSize, _aobBlocks);
		GameSpecific::InterceptorHelper::setCameraStructInterceptorHook(_aobBlocks);
		waitForCameraStructAddresses();		// blocks till camera is found.
		GameSpecific::InterceptorHelper::setPostCameraStructHooks(_aobBlocks);

		// camera struct found, init our own camera object now and hook into game code which uses camera.
		_cameraStructFound = true;
		_camera.setPitch(INITIAL_PITCH_RADIANS);
		_camera.setRoll(INITIAL_ROLL_RADIANS);
		_camera.setYaw(INITIAL_YAW_RADIANS);

		CameraManipulator::applySettingsToGameState();
	}


	// Waits for the interceptor to pick up the camera struct address. Should only return if address is found 
	void System::waitForCameraStructAddresses()
	{
		MessageHandler::logLine("Waiting for camera struct interception...");
		while(!GameSpecific::CameraManipulator::isCameraFound())
		{
			handleUserInput();
			Sleep(100);
		}
		MessageHandler::addNotification("Camera found.");
		GameSpecific::CameraManipulator::displayCameraStructAddress();
	}
		

	void System::toggleCameraMovementLockState(bool newValue)
	{
		if (_cameraMovementLocked == newValue)
		{
			// already in this state. Ignore
			return;
		}
		_cameraMovementLocked = newValue;
		MessageHandler::addNotification(_cameraMovementLocked ? "Camera movement is locked" : "Camera movement is unlocked");
	}
	

	void System::toggleInputBlockState(bool newValue)
	{
		if (Globals::instance().inputBlocked() == newValue)
		{
			// already in this state. Ignore
			return;
		}
		Globals::instance().inputBlocked(newValue);
		MessageHandler::addNotification(newValue ? "Input to game blocked" : "Input to game enabled");
	}


	void System::displayCameraState()
	{
		MessageHandler::addNotification(g_cameraEnabled ? "Camera enabled" : "Camera disabled");
	}
}
