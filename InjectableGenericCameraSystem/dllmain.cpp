// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

using namespace std;

DWORD WINAPI MainThread(LPVOID lpParam);
void InitConsole();
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
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}


// lpParam gets the hModule value of the DllMain process
DWORD WINAPI MainThread(LPVOID lpParam)
{
	InitConsole();

	HMODULE moduleHandle = (HMODULE)lpParam;
	HMODULE baseAddress = GetBaseAddressOfContainingProcess();
	if(NULL == baseAddress)
	{
		cout << "Not able to obtain parent process base address... exiting" << endl;
	}
	else
	{
		// start system and enter input loop
		System s;
		s.Init();
	}
	FreeConsole();
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
			wcout << "Contained in process: " << processName << endl;
			wcout << "Base address: " << toReturn << endl;
		}
		else
		{
			toReturn = NULL;
		}
	}
	CloseHandle(processHandle);
	return toReturn;
}


void InitConsole()
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());

	// Redirect the CRT standard input, output, and error handles to the console
	FILE *fp;
	freopen_s(&fp, "CONIN$", "r", stdin);
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);

	//Clear the error state for each of the C++ standard stream objects. 
	std::wcout.clear();
	std::cout.clear();
	std::wcerr.clear();
	std::cerr.clear();
	std::wcin.clear();
	std::cin.clear();
}

