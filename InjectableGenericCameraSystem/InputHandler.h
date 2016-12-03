////////////////////////////////////////////////////////////////////////////////
// Adapted from: http://www.rastertek.com/dx11tut13.html
////////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define DIRECTINPUT_VERSION 0x0800

/////////////
// LINKING //
/////////////
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


//////////////
// INCLUDES //
//////////////
#include <dinput.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: InputHandler
////////////////////////////////////////////////////////////////////////////////
class InputHandler
{
public:
	InputHandler();
	InputHandler(const InputHandler&);
	~InputHandler();

	bool Initialize(HINSTANCE, HWND);
	void Shutdown();
	bool Frame();

	bool IsKeyPressed(byte keyToCheck);
	void GetMouseDeltas(long&, long&);

private:
	bool ReadKeyboard();
	bool ReadMouse();

private:
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	byte m_keyboardState[256];
	DIMOUSESTATE m_mouseState;
};
