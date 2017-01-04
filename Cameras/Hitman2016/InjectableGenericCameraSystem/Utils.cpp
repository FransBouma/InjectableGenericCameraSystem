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
#include "Utils.h"

// Sets a jmp qword ptr [address] statement at hostImageAddress + startOffset.
void SetHook(LPBYTE hostImageAddress, DWORD startOffset, DWORD continueOffset, LPBYTE* interceptionContinue, void* asmFunction)
{
	LPBYTE startOfHookAddress = hostImageAddress + startOffset;
	*interceptionContinue = hostImageAddress + continueOffset;
	// now write bytes of jmp qword ptr [address], which is jmp qword ptr 0 offset.
	memcpy(startOfHookAddress, jmpFarInstructionBytes, sizeof(jmpFarInstructionBytes));
#ifdef _WIN64
	// now write the address. Do this with a recast of the pointer to an __int64 pointer to avoid endianmess.
	__int64* interceptorAddressDestination = (__int64*)(startOfHookAddress + 6);
	interceptorAddressDestination[0] = (__int64)asmFunction;
#else	// x86
	// now write the address. Do this with a recast of the pointer to a DWORD pointer to avoid endianmess.
	LPDWORD interceptorAddressDestination = (LPDWORD)(startOfHookAddress + 6);
	interceptorAddressDestination[0] = (LPDWORD)asmFunction;
#endif
}

// Writes NOP opcodes to a range of memory.
void NopRange(LPBYTE startAddress, int length)
{
	for (int i = 0; i < length; i++)
	{
		startAddress[i] = 0x90;
	}
}


BOOL IsMainWindow(HWND handle)
{
	BOOL toReturn = GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
	if (toReturn)
	{
		// check window title as there can be more top windows. 
		int bufsize = GetWindowTextLength(handle) + 1;
		LPWSTR title = new WCHAR[bufsize];
		GetWindowText(handle, title, bufsize);
		toReturn &= (_wcsicmp(title, TEXT(GAME_WINDOW_TITLE)) == 0);
#ifdef _DEBUG
		wcout << "Window found with title: '" << title << "'" << endl;
#endif
	}
	return toReturn;
}


BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
	handle_data& data = *(handle_data*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	if (data.process_id != process_id || !IsMainWindow(handle))
	{
		return TRUE;
	}
	data.best_handle = handle;
	return FALSE;
}


HWND FindMainWindow(unsigned long process_id)
{
	handle_data data;
	data.process_id = process_id;
	data.best_handle = 0;
	EnumWindows(EnumWindowsCallback, (LPARAM)&data);
	return data.best_handle;
}

