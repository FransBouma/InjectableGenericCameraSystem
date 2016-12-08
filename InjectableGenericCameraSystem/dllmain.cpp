// dllmain.cpp : Defines the entry point for the DLL application.
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

	HMODULE moduleHandle = (HMODULE)lpParam;
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
	HMODULE toReturn;

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

