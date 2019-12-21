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

namespace IGCS
{
	NamedPipeManager::NamedPipeManager(): _pipeHandle(nullptr), _pipeConnected(false)
	{
	}

	
	NamedPipeManager::~NamedPipeManager()
	{
	}

	
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

	
	void NamedPipeManager::writeMessage(std::string messageText)
	{
		if(!_pipeConnected)
		{
			return;
		}

		DWORD numberOfBytesWritten;
		WriteFile(_pipeHandle, messageText.c_str(), messageText.length() + 1, &numberOfBytesWritten, nullptr);
	}
}
