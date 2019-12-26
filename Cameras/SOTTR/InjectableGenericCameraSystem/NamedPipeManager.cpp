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
#include "NamedPipeManager.h"
#include "Defaults.h"
#include "Console.h"
#include <string>
#include "Utils.h"
#include "Globals.h"

namespace IGCS
{
	// Workaround for having a method as the actual thread func. See: https://stackoverflow.com/a/1372989
	static DWORD WINAPI staticListenerThread(LPVOID lpParam)
	{
		// our message handling will be very fast so we'll do this on the listener thread, no need to offload that to yet another thread yet.
		auto This = (NamedPipeManager*)lpParam;
		return This->listenerThread();
	}

	NamedPipeManager::NamedPipeManager(): _pipeHandle(nullptr), _pipeConnected(false)
	{
	}

	
	NamedPipeManager::~NamedPipeManager()
	= default;


	NamedPipeManager& NamedPipeManager::instance()
	{
		static NamedPipeManager theInstance;
		return theInstance;
	}

	
	void NamedPipeManager::connect()
	{
		if(_pipeConnected)
		{
			return;
		}
		_pipeHandle = CreateFile(TEXT(IGCS_NAMED_PIPE_NAME), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
		_pipeConnected = (_pipeHandle != INVALID_HANDLE_VALUE);
		if(!_pipeConnected)
		{
			Console::WriteError("Couldn't connect to named pipe.");
		}
	}

	
	void NamedPipeManager::startListening()
	{
		// create a thread to listen to the named pipe for messages and handle them.
		DWORD threadID;
		HANDLE threadHandle = CreateThread(nullptr, 0, staticListenerThread, (LPVOID)this, 0, &threadID);
	}

	
	void NamedPipeManager::writeMessage(std::string messageText)
	{
		if(!_pipeConnected)
		{
			return;
		}

		DWORD numberOfBytesWritten;
		WriteFile(_pipeHandle, messageText.c_str(), messageText.length() + 1, &numberOfBytesWritten, nullptr);
	}


try this? => https://developercommunity.visualstudio.com/content/problem/241290/trouble-connecting-c-client-to-c-pipeserver-using.html	
	
	DWORD NamedPipeManager::listenerThread()
	{
		if (_pipeHandle == INVALID_HANDLE_VALUE)
		{
			_pipeHandle = CreateFile(TEXT(IGCS_NAMED_PIPE_NAME), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
			DWORD lastError = GetLastError();
		}
		while(_pipeHandle!=INVALID_HANDLE_VALUE)
		{
			DWORD dwMode = PIPE_READMODE_MESSAGE;
			BOOL fSuccess = SetNamedPipeHandleState(
				_pipeHandle, // pipe handle
				&dwMode, // new pipe mode
				NULL, // don't set maximum bytes
				NULL); // don't set maximum time

			if (!fSuccess)
			{
				Console::WriteError(Utils::formatString("SetNamedPipeHandleState return: 0x%llx%", GetLastError()));
				Sleep(500);
			}
			BYTE buffer[1024];
			DWORD bytesRead;
			while(true)
			{
				auto result = ReadFile(_pipeHandle, buffer, sizeof(buffer), &bytesRead, nullptr);
				handleMessage(buffer, bytesRead);
				if(result==FALSE)
				{
					break;
				}
			}
		}
		return 0;
	}

	
	void NamedPipeManager::handleMessage(BYTE buffer[], DWORD bytesRead)
	{
		if(bytesRead<3)
		{
			Console::WriteError(Utils::formatString("Received message with length: %d", bytesRead));
			return;
		}
		switch(buffer[0])
		{
		case 1:		// Setting
			Globals::instance().handleSettingMessage(buffer, bytesRead);
#ifdef _DEBUG			
			Console::WriteLine(Utils::formatString("Setting message received with id %d. Length: %d", buffer[1], bytesRead));
#endif
			break;
		case 2:		// Keybinding
			Globals::instance().handleKeybindingMessage(buffer, bytesRead);
#ifdef _DEBUG
			Console::WriteLine(Utils::formatString("Keybinding message received with id %d. Length: %d", buffer[1], bytesRead));
#endif
			break;
		case 5:		// Action
			break;
		default:
			Console::WriteError(Utils::formatString("Received message of type: %d. Can't handle it. Ignored", buffer[0]));
			break;
		}
	}
}
