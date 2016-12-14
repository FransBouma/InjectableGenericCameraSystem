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

using namespace std;

DWORD WINAPI MainThread(LPVOID lpParam);
HMODULE GetBaseAddressOfContainingProcess();

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	DWORD threadID;

	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			CreateThread(
				NULL,
				0,
				MainThread,
				hModule,
				0,
				&threadID
			);
			break;
		case DLL_PROCESS_DETACH:
			g_systemActive = false;
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
	}
	return TRUE;
}


// lpParam gets the hModule value of the DllMain process
DWORD WINAPI MainThread(LPVOID lpParam)
{
	Console c;
	c.Init();
	c.WriteHeader();

	HMODULE baseAddress = GetBaseAddressOfContainingProcess();
	if(NULL == baseAddress)
	{
		c.WriteError("Not able to obtain parent process base address... exiting");
	}
	else
	{
		SystemStart(baseAddress, c);
	}
	c.Release();
	return 0;
}


HMODULE GetBaseAddressOfContainingProcess()
{
	TCHAR processName[MAX_PATH] = TEXT("<unknown>");
	HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
	HMODULE toReturn=NULL;

	if (NULL != processHandle)
	{
		DWORD cbNeeded;
		if (EnumProcessModulesEx(processHandle, &toReturn, sizeof(toReturn), &cbNeeded, LIST_MODULES_32BIT | LIST_MODULES_64BIT))
		{
			GetModuleBaseName(processHandle, toReturn, processName, sizeof(processName) / sizeof(TCHAR));
		}
		else
		{
			toReturn = NULL;
		}
	}
	CloseHandle(processHandle);
	return toReturn;
}

