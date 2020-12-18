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
#pragma once
#include "stdafx.h"
#include "Gamepad.h"
#include "Defaults.h"
#include <map>
#include <atomic>
#include "ActionData.h"
#include <map>
#include "Settings.h"

extern "C" uint8_t g_cameraEnabled;
extern "C" uint8_t g_wetness_OverrideParameters;
extern "C" float g_wetness_StreetWetnessFactor;
extern "C" LPBYTE g_pmStructAddress;
extern "C" LPBYTE g_activeCamStructAddress;
extern "C" LPBYTE g_resolutionStructAddress;
extern "C" LPBYTE g_todStructAddress;
extern "C" LPBYTE g_playHudWidgetAddress;
extern "C" LPBYTE g_pmHudWidgetAddress;
extern "C" LPBYTE g_timestopStructAddress;
extern "C" LPBYTE g_weatherStructAddress;

namespace IGCS
{
	class Globals
	{
	public:
		Globals();
		~Globals();

		static Globals& instance();

		bool inputBlocked() const { return _inputBlocked; }
		void inputBlocked(bool value) { _inputBlocked = value; }
		bool systemActive() const { return _systemActive; }
		void systemActive(bool value) { _systemActive = value; }
		HWND mainWindowHandle() const { return _mainWindowHandle; }
		void mainWindowHandle(HWND handle) { _mainWindowHandle = handle; }
		bool toggleHudVisible()
		{
			_hudVisible = !_hudVisible;
			return _hudVisible;
		}
		Gamepad& gamePad() { return _gamePad; }
		Settings& settings() { return _settings; }
		bool keyboardMouseControlCamera() const { return _settings.cameraControlDevice == DEVICE_ID_KEYBOARD_MOUSE || _settings.cameraControlDevice == DEVICE_ID_ALL; }
		bool controllerControlsCamera() const { return _settings.cameraControlDevice == DEVICE_ID_GAMEPAD || _settings.cameraControlDevice == DEVICE_ID_ALL; }
		ActionData* getActionData(ActionType type);
		void handleSettingMessage(uint8_t payload[], DWORD payloadLength);
		void handleKeybindingMessage(uint8_t payload[], DWORD payloadLength);

	private:
		void initializeKeyBindings();

		bool _inputBlocked = true;
		atomic_bool _systemActive = false;
		Gamepad _gamePad;
		HWND _mainWindowHandle;
		Settings _settings;
		map<ActionType, ActionData*> _keyBindingPerActionType;
		bool _hudVisible = true;
	};
}