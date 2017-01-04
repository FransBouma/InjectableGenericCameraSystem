#include "stdafx.h"
#include "Utils.h"
#include "MinHook.h"
#include "Gamepad.h"

using namespace std;
extern bool g_inputBlocked;
extern Gamepad* g_gamePad;
extern Console* g_consoleWrapper;

////////////// TYPEDEFS OF FUNCTIONS TO HOOK
typedef DWORD (WINAPI *XINPUTGETSTATE)(DWORD, XINPUT_STATE*);

////////////// Pointers of original functions
static XINPUTGETSTATE hookedXInputGetState = nullptr;


// wrapper for easier setting up hooks for MinHook
template <typename T>
inline MH_STATUS MH_CreateHookEx(LPVOID pTarget, LPVOID pDetour, T** ppOriginal)
{
	return MH_CreateHook(pTarget, pDetour, reinterpret_cast<LPVOID*>(ppOriginal));
}

template <typename T>
inline MH_STATUS MH_CreateHookApiEx(
	LPCWSTR pszModule, LPCSTR pszProcName, LPVOID pDetour, T** ppOriginal)
{
	return MH_CreateHookApi(
		pszModule, pszProcName, pDetour, reinterpret_cast<LPVOID*>(ppOriginal));
}


DWORD WINAPI DetourXInputGetState(
	_In_  DWORD         dwUserIndex,  // Index of the gamer associated with the device
	_Out_ XINPUT_STATE* pState        // Receives the current state
)
{
	// first call the original function
	DWORD toReturn = hookedXInputGetState(dwUserIndex, pState);
	// check if the passed in pState is equal to our gamestate. If so, always allow.
	if (pState != g_gamePad->getState())
	{
		// check if the camera is enabled. If so, zero the state
		if (g_inputBlocked)
		{
			ZeroMemory(pState, sizeof(XINPUT_STATE));
		}
	}
	return toReturn;
}


void SetInputHooks()
{
	MH_Initialize();
	if (MH_CreateHookApiEx(L"xinput9_1_0", "XInputGetState", &DetourXInputGetState, &hookedXInputGetState) != MH_OK)
	{
		g_consoleWrapper->WriteError("Hooking XInput 9_1_0 failed!");
	}
#ifdef _DEBUG
	g_consoleWrapper->WriteLine("Hook set to XInputSetState");
#endif

	// Enable all hooks
	if (MH_EnableHook(MH_ALL_HOOKS) == MH_OK)
	{
#ifdef _DEBUG
		g_consoleWrapper->WriteLine("All hooks enabled");
#endif
	}
	else
	{
		g_consoleWrapper->WriteError("Enabling hooks failed");
	}
}
