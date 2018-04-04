#include "stdafx.h"
#include "OverlayControl.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"
#include "OverlayConsole.h"
#include "GameConstants.h"
#include <algorithm>
#include <mutex>
#include "Defaults.h"
#include "Globals.h"
#include "CameraManipulator.h"
#include "Input.h"
#include <atomic>

using namespace std;

namespace IGCS::OverlayControl
{
	//-----------------------------------------------
	// private structs
	struct Notification
	{
		string notificationText;
		float timeFirstDisplayed;		// if < 0, never displayed
	};

	//-----------------------------------------------
	// statics
	static vector<Notification> _activeNotifications;
	static CRITICAL_SECTION _notificationCriticalSection;
	static atomic_bool _displaySplash = true;
	static atomic<float> _timeSplashFirstShown = -1;
	static ImVec2 _splashWindowSize;
	static atomic_int _menuItemSelected = 0;
	static bool _showMainWindow = false;
	
	//-----------------------------------------------
	// forward declarations
	void renderNotifications();
	void renderAbout();
	void renderHelp();
	void renderSettings();
	void renderMainWindow();
	void renderSplash();
	void updateNotificationStore();
	void ShowHelpMarker(const char* desc);
	void ShowCommandConsole(bool* p_open);

	//-----------------------------------------------
	// code

	void init()
	{
		InitializeCriticalSectionAndSpinCount(&_notificationCriticalSection, 0x400);
	}

	void addNotification(string notificationText)
	{
		EnterCriticalSection(&_notificationCriticalSection);
			Notification toAdd;
			toAdd.notificationText = notificationText;
			toAdd.timeFirstDisplayed = -1.0f;
			_activeNotifications.push_back(toAdd);
			OverlayConsole::instance().logLine(notificationText.c_str());
		LeaveCriticalSection(&_notificationCriticalSection);
	}


	bool popen = true;
	void renderOverlay()
	{
		// set io values FIRST, as NewFrame will reset IO values otherwise and it looks at the values for mousewheel for sizing in... new frame!
		Input::setKeyboardMouseStateInImGuiIO();
		Globals::instance().saveSettingsIfRequired(ImGui::GetIO().DeltaTime);

		ImGui_ImplDX11_NewFrame();
		ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);     // Normally user code doesn't need/want to call it because positions are saved in .ini file anyway. 
		auto& io = ImGui::GetIO();
		io.MouseDrawCursor = _showMainWindow;
		renderSplash();
		renderMainWindow();
		renderNotifications();
		ImGui::Render();
	}

	void renderMainWindow()
	{
		if (!_showMainWindow)
		{
			return;
		}
		ImGui::SetNextWindowSize(ImVec2(700, 450), ImGuiCond_FirstUseEver);
		string title = string("Injectable Camera Tools v") + CAMERA_VERSION + " for " + GAME_NAME;
		ImGui::Begin(title.c_str(), &_showMainWindow, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse);

		if (ImGui::BeginMenuBar())
		{
			auto itemSpacing = ImVec2(ImGui::GetStyle().ItemSpacing.x*2.0f, ImGui::GetStyle().ItemSpacing.y);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, itemSpacing);
			const char *const menu_items[] = { "Settings", "Console", "Help", "About" };
			for (int i = 0; i < 4; i++)
			{
				if (ImGui::Selectable(menu_items[i], _menuItemSelected == i, 0, ImVec2(ImGui::CalcTextSize(menu_items[i]).x, 0)))
				{
					_menuItemSelected = i;
				}
				ImGui::SameLine();
			}
			ImGui::PopStyleVar();
			ImGui::EndMenuBar();
		}

		ImGui::Spacing();

		switch (_menuItemSelected)
		{
		case 0: // settings
			renderSettings();
			break;
		case 1:	// console
			IGCS::OverlayConsole::instance().draw();
			break;
		case 2:	// help
			renderHelp();
			break;
		case 3:	// about
			renderAbout();
			break;
		}
		ImGui::End();
		ShowCommandConsole(&_showMainWindow);
	}


	void renderAbout()
	{
		ImGui::PushTextWrapPos();
		ImGui::Text("Injectable Camera Tools v%s for %s", CAMERA_VERSION, GAME_NAME);
		ImGui::Text("Camera credits: %s", CAMERA_CREDITS);
		ImGui::Spacing();
		ImGui::Text(R"(Powered by Injectable Generic Camera System (IGCS). 
Copyright (c) 2018 Otis_Inf (Frans Bouma). All rights reserved.
https://github.com/FransBouma/InjectableGenericCameraSystem

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.)");
		if (ImGui::CollapsingHeader("Dear ImGui"))
		{
			ImGui::TextUnformatted(R"(Copyright (c) 2014-2015 Omar Cornut and ImGui contributors

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.)");
		}
		if (ImGui::CollapsingHeader("MinHook"))
		{
			ImGui::TextUnformatted(R"(Copyright (C) 2009-2017 Tsuda Kageyu. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.)");
		}
		if (ImGui::CollapsingHeader("Miscellaneous"))
		{
			ImGui::TextUnformatted(R"(Uses CDataFile by Gary McNickle for ini file processing.

Special thanks to: 
* Crosire, for creating and opening the Reshade sourcecode. It was a great help with ImGui.
* The users of my camera's, you all are a great inspiration to make the best camera tools possible!)");
		}
		ImGui::PopTextWrapPos();
	}


	void renderHelp()
	{
		if (ImGui::CollapsingHeader("General info"))
		{
			ImGui::PushTextWrapPos();
			ImGui::TextUnformatted("* When the main window is open, all input of keyboard / mouse to the game is blocked and the camera is locked.");
			ImGui::TextUnformatted("* All changes you make to the main window (position/size) are saved to a file in the game root folder.");
			ImGui::TextUnformatted("* Most settings you change will make the settings to be saved to a file in the game root folder.");
			ImGui::TextUnformatted("* When you block input to the game, the device(s) which control the camera are blocked, all other devices are still sending input to the game.");
			ImGui::TextUnformatted("* You'll get a notification in the top left corner when you change something with the keyboard, like enable/disable the camera.");
			ImGui::TextUnformatted("* The resolution scale factor is reset to the game menu value when the camera is disabled");
			ImGui::PopTextWrapPos();
		}

		if (ImGui::CollapsingHeader("Camera tools controls"))
		{
			ImGui::TextUnformatted("Ctrl-Ins                              : Show / Hide Camera tools main window");
			ImGui::TextUnformatted("Ctrl + Mouse wheel                    : Resize font");
			ImGui::TextUnformatted("Del                                   : Toggle first/third person (not when camera is active)");
			ImGui::TextUnformatted("Ins                                   : Enable/Disable camera");
			ImGui::TextUnformatted("Home                                  : Lock/unlock camera movement");
			ImGui::TextUnformatted("ALT + rotate/move                     : Faster rotate / move");
			ImGui::TextUnformatted("Right-CTRL + rotate/move              : Slower rotate / move");
			ImGui::TextUnformatted("Controller Y-button + l/r-stick       : Faster rotate / move");
			ImGui::TextUnformatted("Controller X-button + l/r-stick       : Slower rotate / move");
			ImGui::TextUnformatted("Arrow up/down or mouse or r-stick     : Rotate camera up/down");
			ImGui::TextUnformatted("Arrow left/right or mouse or r-stick  : Rotate camera left/right");
			ImGui::TextUnformatted("Numpad 8/Numpad 5 or l-stick          : Move camera forward/backward");
			ImGui::TextUnformatted("Numpad 4/Numpad 6 or l-stick          : Move camera left / right");
			ImGui::TextUnformatted("Numpad 7/Numpad 9 or l/r-trigger      : Move camera up / down");
			ImGui::TextUnformatted("Numpad 1/Numpad 3 or d-pad left/right : Tilt camera left / right");
			ImGui::TextUnformatted("Numpad +/- or d-pad up/down           : Increase / decrease FoV");
			ImGui::TextUnformatted("Numpad * or controller B-button       : Reset FoV");
			ImGui::TextUnformatted("Numpad .                              : Block input to game for camera control device.");
			ImGui::TextUnformatted("Numpad 0                              : Toggle game pause");
		}

		if (ImGui::CollapsingHeader("Settings editor help"))
		{
			ImGui::PushTextWrapPos();
			ImGui::TextUnformatted("* Click and drag on lower right corner to resize window.");
			ImGui::TextUnformatted("* Click and drag on any empty space to move window.");
			ImGui::TextUnformatted("* Mouse Wheel to scroll.");
			ImGui::TextUnformatted("* TAB/SHIFT+TAB to cycle through keyboard editable fields.");
			ImGui::TextUnformatted("* Click and drag on a text box to change its value.");
			ImGui::TextUnformatted("* CTRL+Click on a slider or text box to input text.");
			ImGui::TextUnformatted(
				"* While editing text:\n"
				"  - Hold SHIFT or use mouse to select text\n"
				"  - CTRL+Left/Right to word jump\n"
				"  - CTRL+A or double-click to select all\n"
				"  - CTRL+X,CTRL+C,CTRL+V clipboard\n"
				"  - CTRL+Z,CTRL+Y undo/redo\n"
				"  - ESCAPE to revert\n"
				"  - You can apply arithmetic operators +,*,/ on numerical values.\n"
				"    Use +- to subtract.\n");
			ImGui::PopTextWrapPos();
		}
	}


	void renderSettings()
	{
		GameSpecific::CameraManipulator::getSettingsFromGameState();
		Settings& currentSettings = Globals::instance().settings();
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.3f);
		bool settingsChanged = false;
		if (ImGui::Button("Reset to defaults"))
		{
			currentSettings.init(true);
			settingsChanged = true;
		}

		if (ImGui::CollapsingHeader("Camera movement options", ImGuiTreeNodeFlags_DefaultOpen))
		{
			settingsChanged |= ImGui::SliderFloat("Fast movement multiplier", &currentSettings.fastMovementMultiplier, 0.1f, 100.0f, "%.3f");
			settingsChanged |= ImGui::SliderFloat("Slow movement multiplier", &currentSettings.slowMovementMultiplier, 0.001f, 1.0f, "%.3f");
			settingsChanged |= ImGui::SliderFloat("Up movement multiplier", &currentSettings.movementUpMultiplier, 0.1f, 10.0f, "%.3f");
			settingsChanged |= ImGui::SliderFloat("Movement speed", &currentSettings.movementSpeed, 0.05f, 50.0f, "%.2f");
			settingsChanged |= ImGui::Combo("Camera control device", &currentSettings.cameraControlDevice, "Keyboard & Mouse\0Gamepad\0Both\0\0");
			ImGui::SameLine(); ShowHelpMarker("The camera control device chosen will be blocked for game input.\n");
			ImGui::TextUnformatted("");  ImGui::SameLine((ImGui::GetWindowWidth() * 0.3f) - 11.0f);
			settingsChanged |= ImGui::Checkbox("Allow camera movement when this menu is up", &currentSettings.allowCameraMovementWhenMenuIsUp);
		}
		if (ImGui::CollapsingHeader("Camera rotation options", ImGuiTreeNodeFlags_DefaultOpen))
		{
			settingsChanged |= ImGui::SliderFloat("Rotation speed", &currentSettings.rotationSpeed, 0.001f, 0.5f, "%.3f");
			ImGui::TextUnformatted("");  ImGui::SameLine((ImGui::GetWindowWidth() * 0.3f)-11.0f);
			settingsChanged |= ImGui::Checkbox("Invert Y look direction", &currentSettings.invertY);
		}
		if (ImGui::CollapsingHeader("Misc. camera options", ImGuiTreeNodeFlags_DefaultOpen))
		{
			settingsChanged |= ImGui::SliderFloat("Field of View (FoV) zoom speed", &currentSettings.fovChangeSpeed, 0.0001f, 0.01f, "%.4f");
		}
		ImGui::PopItemWidth();
		if (settingsChanged)
		{
			Globals::instance().markSettingsDirty();
		}
		GameSpecific::CameraManipulator::applySettingsToGameState();
	}


	void renderNotifications()
	{
		EnterCriticalSection(&_notificationCriticalSection);
			updateNotificationStore();
			if (_activeNotifications.size() > 0)
			{
				float yCoord = 10;
				if (_timeSplashFirstShown >= 0.0f && _displaySplash)
				{
					yCoord = _splashWindowSize.y + 5.0f;	// add a tiny space between splash and the notifications.
				}
				ImGui::SetNextWindowPos(ImVec2(10, yCoord));
				if (ImGui::Begin("Notifications", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings))
				{
					for (vector<Notification>::iterator it = _activeNotifications.begin(); it < _activeNotifications.end(); it++)
					{
						ImGui::Text(it->notificationText.c_str());
					}
				}
				ImGui::End();
			}
		LeaveCriticalSection(&_notificationCriticalSection);
	}


	// will purge all outdated notifications from the set of active notifications. Be sure to call this from within a mutex lock, as notifications are added from another thread
	void updateNotificationStore()
	{
		float currentRenderTime = ImGui::GetTime();		// time overall,  in seconds

		// for now we'll use 2 seconds (2.0f). First purge everything that's first rendered more than delta T ago
		float maxTimeOnScreen = 2.0f;
		_activeNotifications.erase(remove_if(_activeNotifications.begin(),
											_activeNotifications.end(),
											[currentRenderTime, maxTimeOnScreen](Notification const & x) -> bool 
											{
												return (x.timeFirstDisplayed > 0.0f && (currentRenderTime - x.timeFirstDisplayed) > maxTimeOnScreen); 
											}), 
									_activeNotifications.end());
		// now update all notifications which have a negative time (so haven't been shown) and set them to the current time.
		for_each(begin(_activeNotifications), end(_activeNotifications), 
			[currentRenderTime](Notification & x) 
			{ 
				if (x.timeFirstDisplayed < 0.0f) 
				{
					x.timeFirstDisplayed = currentRenderTime;
				}
			});
	}


	void renderSplash()
	{
		float currentRenderTime = ImGui::GetTime();		// time overall,  in seconds
		if (_timeSplashFirstShown < 0.0f)
		{
			_timeSplashFirstShown = currentRenderTime;
		}
		_displaySplash = _displaySplash & ((currentRenderTime - _timeSplashFirstShown) < IGCS_SPLASH_DURATION);
		if (!_displaySplash)
		{
			return;
		}
		ImGui::SetNextWindowPos(ImVec2(10, 10));
		if (!ImGui::Begin("IGCS Splash", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | 
												  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings))
		{
			ImGui::End();
			return;
		}
		ImVec4 col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, col);
		ImGui::Text("Injectable Camera Tools v%s for %s", CAMERA_VERSION, GAME_NAME);
		ImGui::Text("Camera credits: %s", CAMERA_CREDITS);
		ImGui::Text("\nPress Ctrl-Ins to open the main window.\n");
		ImGui::PopStyleColor();
		ImGui::Separator();
		col = ImVec4(1.0f, 1.0f, 1.0f, 0.6f);
		ImGui::PushStyleColor(ImGuiCol_Text, col);
		ImGui::Text("Powered by Injectable Generic Camera System by Otis_Inf");
		ImGui::Text("Get your copy at: https://github.com/FransBouma/InjectableGenericCameraSystem");
		ImGui::PopStyleColor();
		_splashWindowSize = ImGui::GetWindowSize();
		ImGui::End();
	}


	void toggleOverlay()
	{
		_showMainWindow = !_showMainWindow;
	}


	bool isMainMenuVisible()
	{
		return _showMainWindow;
	}

	void ShowHelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(450.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	struct CommandConsole
	{
		char                  InputBuf[256];
		ImVector<char*>       Items;
		bool                  ScrollToBottom;
		ImVector<char*>       History;
		int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
		ImVector<const char*> Commands;

		CommandConsole()
		{
			ClearLog();
			memset(InputBuf, 0, sizeof(InputBuf));
			HistoryPos = -1;
			Commands.push_back("HELP");
			Commands.push_back("HISTORY");
			Commands.push_back("CLEAR");
			AddLog("Commands are directly passed to the game. sv_cheats is already set.");
		}
		~CommandConsole()
		{
			ClearLog();
			for (int i = 0; i < History.Size; i++)
				free(History[i]);
		}

		// Portable helpers
		static int   Stricmp(const char* str1, const char* str2) { int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
		static int   Strnicmp(const char* str1, const char* str2, int n) { int d = 0; while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; n--; } return d; }
		static char* Strdup(const char *str) { size_t len = strlen(str) + 1; void* buff = malloc(len); return (char*)memcpy(buff, (const void*)str, len); }

		void    ClearLog()
		{
			for (int i = 0; i < Items.Size; i++)
				free(Items[i]);
			Items.clear();
			ScrollToBottom = true;
		}

		void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
		{
			// FIXME-OPT
			char buf[1024];
			va_list args;
			va_start(args, fmt);
			vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
			buf[IM_ARRAYSIZE(buf) - 1] = 0;
			va_end(args);
			Items.push_back(Strdup(buf));
			ScrollToBottom = true;
		}

		void    Draw(const char* title, bool* p_open)
		{
			ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
			if (!ImGui::Begin(title, p_open))
			{
				ImGui::End();
				return;
			}

			ImGui::TextWrapped("Enter 'HELP' for help, press TAB to use text completion.");

			// TODO: display items starting from the bottom

			if (ImGui::SmallButton("Clear")) { ClearLog(); } ImGui::SameLine();
			bool copy_to_clipboard = ImGui::SmallButton("Copy"); ImGui::SameLine();
			if (ImGui::SmallButton("Scroll to bottom")) ScrollToBottom = true;

			ImGui::Separator();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			static ImGuiTextFilter filter;
			filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
			ImGui::PopStyleVar();
			ImGui::Separator();

			ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::Selectable("Clear")) ClearLog();
				ImGui::EndPopup();
			}

			// Display every line as a separate entry so we can change their color or add custom widgets. If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
			// NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping to only process visible items.
			// You can seek and display only the lines that are visible using the ImGuiListClipper helper, if your elements are evenly spaced and you have cheap random access to the elements.
			// To use the clipper we could replace the 'for (int i = 0; i < Items.Size; i++)' loop with:
			//     ImGuiListClipper clipper(Items.Size);
			//     while (clipper.Step())
			//         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
			// However take note that you can not use this code as is if a filter is active because it breaks the 'cheap random-access' property. We would need random-access on the post-filtered list.
			// A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices that passed the filtering test, recomputing this array when user changes the filter,
			// and appending newly elements as they are inserted. This is left as a task to the user until we can manage to improve this example code!
			// If your items are of variable size you may want to implement code similar to what ImGuiListClipper does. Or split your data into fixed height items to allow random-seeking into your list.
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
			if (copy_to_clipboard)
				ImGui::LogToClipboard();
			for (int i = 0; i < Items.Size; i++)
			{
				const char* item = Items[i];
				if (!filter.PassFilter(item))
					continue;
				ImVec4 col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // A better implementation may store a type per-item. For the sample let's just parse the text.
				if (strstr(item, "[error]")) col = ImColor(1.0f, 0.4f, 0.4f, 1.0f);
				else if (strncmp(item, "# ", 2) == 0) col = ImColor(1.0f, 0.78f, 0.58f, 1.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, col);
				ImGui::TextUnformatted(item);
				ImGui::PopStyleColor();
			}
			if (copy_to_clipboard)
				ImGui::LogFinish();
			if (ScrollToBottom)
				ImGui::SetScrollHere();
			ScrollToBottom = false;
			ImGui::PopStyleVar();
			ImGui::EndChild();
			ImGui::Separator();

			// Command-line
			if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, &TextEditCallbackStub, (void*)this))
			{
				char* input_end = InputBuf + strlen(InputBuf);
				while (input_end > InputBuf && input_end[-1] == ' ') { input_end--; } *input_end = 0;
				if (InputBuf[0])
					ExecCommand(InputBuf);
				strcpy(InputBuf, "");
			}

			// Demonstrate keeping auto focus on the input box
			if (ImGui::IsItemHovered() || (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
				ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

			ImGui::End();
		}

		void    ExecCommand(const char* command_line)
		{
			AddLog("# %s\n", command_line);

			// Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
			HistoryPos = -1;
			for (int i = History.Size - 1; i >= 0; i--)
				if (Stricmp(History[i], command_line) == 0)
				{
					free(History[i]);
					History.erase(History.begin() + i);
					break;
				}
			History.push_back(Strdup(command_line));

			// Process command
			if (Stricmp(command_line, "CLEAR") == 0)
			{
				ClearLog();
			}
			else if (Stricmp(command_line, "HELP") == 0)
			{
				AddLog("Commands:");
				for (int i = 0; i < Commands.Size; i++)
					AddLog("- %s", Commands[i]);
			}
			else if (Stricmp(command_line, "HISTORY") == 0)
			{
				int first = History.Size - 10;
				for (int i = first > 0 ? first : 0; i < History.Size; i++)
					AddLog("%3d: %s\n", i, History[i]);
			}
			else
			{
				IGCS::GameSpecific::CameraManipulator::runCommand(command_line);
			}
		}

		static int TextEditCallbackStub(ImGuiTextEditCallbackData* data) // In C++11 you are better off using lambdas for this sort of forwarding callbacks
		{
			CommandConsole* console = (CommandConsole*)data->UserData;
			return console->TextEditCallback(data);
		}

		int     TextEditCallback(ImGuiTextEditCallbackData* data)
		{
			//AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
			switch (data->EventFlag)
			{
			case ImGuiInputTextFlags_CallbackCompletion:
			{
				// Example of TEXT COMPLETION

				// Locate beginning of current word
				const char* word_end = data->Buf + data->CursorPos;
				const char* word_start = word_end;
				while (word_start > data->Buf)
				{
					const char c = word_start[-1];
					if (c == ' ' || c == '\t' || c == ',' || c == ';')
						break;
					word_start--;
				}

				// Build a list of candidates
				ImVector<const char*> candidates;
				for (int i = 0; i < Commands.Size; i++)
					if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
						candidates.push_back(Commands[i]);

				if (candidates.Size == 0)
				{
					// No match
					AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
				}
				else if (candidates.Size == 1)
				{
					// Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
					data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
					data->InsertChars(data->CursorPos, candidates[0]);
					data->InsertChars(data->CursorPos, " ");
				}
				else
				{
					// Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
					int match_len = (int)(word_end - word_start);
					for (;;)
					{
						int c = 0;
						bool all_candidates_matches = true;
						for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
							if (i == 0)
								c = toupper(candidates[i][match_len]);
							else if (c == 0 || c != toupper(candidates[i][match_len]))
								all_candidates_matches = false;
						if (!all_candidates_matches)
							break;
						match_len++;
					}

					if (match_len > 0)
					{
						data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
						data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
					}

					// List matches
					AddLog("Possible matches:\n");
					for (int i = 0; i < candidates.Size; i++)
						AddLog("- %s\n", candidates[i]);
				}

				break;
			}
			case ImGuiInputTextFlags_CallbackHistory:
			{
				// Example of HISTORY
				const int prev_history_pos = HistoryPos;
				if (data->EventKey == ImGuiKey_UpArrow)
				{
					if (HistoryPos == -1)
						HistoryPos = History.Size - 1;
					else if (HistoryPos > 0)
						HistoryPos--;
				}
				else if (data->EventKey == ImGuiKey_DownArrow)
				{
					if (HistoryPos != -1)
						if (++HistoryPos >= History.Size)
							HistoryPos = -1;
				}

				// A better implementation would preserve the data on the current input line along with cursor position.
				if (prev_history_pos != HistoryPos)
				{
					data->CursorPos = data->SelectionStart = data->SelectionEnd = data->BufTextLen = (int)snprintf(data->Buf, (size_t)data->BufSize, "%s", (HistoryPos >= 0) ? History[HistoryPos] : "");
					data->BufDirty = true;
				}
			}
			}
			return 0;
		}
	};


	void ShowCommandConsole(bool* p_open)
	{
		static CommandConsole console;
		console.Draw("Command Console", p_open);
	}
}
