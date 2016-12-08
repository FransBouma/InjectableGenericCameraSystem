//-----------------------------------------------------------------------------
// Copyright (c) 2007 dhpoware. All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------
#pragma once
#include "stdafx.h"

class Keyboard
{
public:
    enum Key
    {
        KEY_0               = 0x0B,
        KEY_1               = 0x02,
        KEY_2               = 0x03,
        KEY_3               = 0x04,
        KEY_4               = 0x05,
        KEY_5               = 0x06,
        KEY_6               = 0x07,
        KEY_7               = 0x08,
        KEY_8               = 0x09,
        KEY_9               = 0x0A,

        KEY_A               = 0x1E,
        KEY_B               = 0x30,
        KEY_C               = 0x2E,
        KEY_D               = 0x20,
        KEY_E               = 0x12,
        KEY_F               = 0x21,
        KEY_G               = 0x22,
        KEY_H               = 0x23,
        KEY_I               = 0x17,
        KEY_J               = 0x24,
        KEY_K               = 0x25,
        KEY_L               = 0x26,
        KEY_M               = 0x32,
        KEY_N               = 0x31,
        KEY_O               = 0x18,
        KEY_P               = 0x19,
        KEY_Q               = 0x10,
        KEY_R               = 0x13,
        KEY_S               = 0x1F,
        KEY_T               = 0x14,
        KEY_U               = 0x16,
        KEY_V               = 0x2F,
        KEY_W               = 0x11,
        KEY_X               = 0x2D,
        KEY_Y               = 0x15,
        KEY_Z               = 0x2C,
        
        KEY_F1              = 0x3B,
        KEY_F2              = 0x3C,
        KEY_F3              = 0x3D,
        KEY_F4              = 0x3E,
        KEY_F5              = 0x3F,
        KEY_F6              = 0x40,
        KEY_F7              = 0x41,
        KEY_F8              = 0x42,
        KEY_F9              = 0x43,
        KEY_F10             = 0x44,
        KEY_F11             = 0x57,
        KEY_F12             = 0x58,

        KEY_NUMPAD_0        = 0x52,
        KEY_NUMPAD_1        = 0x4F,
        KEY_NUMPAD_2        = 0x50,
        KEY_NUMPAD_3        = 0x51,
        KEY_NUMPAD_4        = 0x4B,
        KEY_NUMPAD_5        = 0x4C,
        KEY_NUMPAD_6        = 0x4D,
        KEY_NUMPAD_7        = 0x47,
        KEY_NUMPAD_8        = 0x48,
        KEY_NUMPAD_9        = 0x49,
        KEY_NUMPAD_ADD      = 0x4E,
        KEY_NUMPAD_DIVIDE   = 0xB5,
        KEY_NUMPAD_ENTER    = 0x9C,
        KEY_NUMPAD_MINUS    = 0x4A,
        KEY_NUMPAD_MULTIPLY = 0x37,
        KEY_NUMPAD_NUMLOCK  = 0x45,
        KEY_NUMPAD_PERIOD   = 0x53,

        KEY_ADD             = 0x0D,
        KEY_APOSTROPHE      = 0x28,
        KEY_APPS            = 0xDD,
        KEY_BACKSLASH       = 0x2B,
        KEY_BACKSPACE       = 0x0E,
        KEY_CAPSLOCK        = 0x3A,
        KEY_COMMA           = 0x33,
        KEY_ENTER           = 0x1C,
        KEY_ESCAPE          = 0x01,
        KEY_MINUS           = 0x0C,
        KEY_PAUSE           = 0xC5,
        KEY_PERIOD          = 0x34,
        KEY_PRINTSCREEN     = 0xB7,
        KEY_SCROLLLOCK      = 0x46,
        KEY_SEMICOLON       = 0x27,
        KEY_SLASH           = 0x35,
        KEY_SPACE           = 0x39,
        KEY_TAB             = 0x0F,
        KEY_TILDE           = 0x29,

        KEY_LALT            = 0x38,
        KEY_LBRACKET        = 0x1A,
        KEY_LCONTROL        = 0x1D,
        KEY_LSHIFT          = 0x2A,
        KEY_LWIN            = 0xDB,
        KEY_RALT            = 0xB8,
        KEY_RBRACKET        = 0x1B,
        KEY_RCONTROL        = 0x9D,
        KEY_RSHIFT          = 0x36,
        KEY_RWIN            = 0xDC,

        KEY_DOWN            = 0xD0,
        KEY_LEFT            = 0xCB,
        KEY_RIGHT           = 0xCD,
        KEY_UP              = 0xC8,

        KEY_DELETE          = 0xD3,
        KEY_END             = 0xCF,
        KEY_HOME            = 0xC7,
        KEY_INSERT          = 0xD2,
        KEY_PAGEDOWN        = 0xD1,
        KEY_PAGEUP          = 0xC9,

        KEY_CALCULATOR      = 0xA1,
        KEY_MAIL            = 0xEC,
        KEY_MEDIASELECT     = 0xED,
        KEY_MEDIASTOP       = 0xA4,
        KEY_MUTE            = 0xA0,
        KEY_MYCOMPUTER      = 0xEB,
        KEY_NEXTTRACK       = 0x99,
        KEY_PLAYPAUSE       = 0xA2,
        KEY_POWER           = 0xDE,
        KEY_PREVTRACK       = 0x90,
        KEY_SLEEP           = 0xDF,
        KEY_VOLUMEDOWN      = 0xAE,
        KEY_VOLUMEUP        = 0xB0,
        KEY_WAKE            = 0xE3,
        KEY_WEBBACK         = 0xEA,
        KEY_WEBFAVORITES    = 0xE6,
        KEY_WEBFORWARD      = 0xE9,
        KEY_WEBHOME         = 0xB2,
        KEY_WEBREFRESH      = 0xE7,
        KEY_WEBSEARCH       = 0xE5,
        KEY_WEBSTOP         = 0xE8
    };

    static Keyboard &instance();
	static void init(HINSTANCE);

    void handleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void update();

    char getLastChar() const
    { return m_lastChar; }

	bool keyDown(Key key) const
    { return (m_pCurrKeyStates[key] & 0x80) ? true : false; }

	bool keyUp(Key key) const
    { return (m_pCurrKeyStates[key] & 0x80) ? false : true; }

	bool keyPressed(Key key) const
    { 
        return ((m_pCurrKeyStates[key] & 0x80)
            && !(m_pPrevKeyStates[key] & 0x80)) ? true : false;
    }

private:
    Keyboard();
    ~Keyboard();

    bool create();
    void destroy();
    
	static HINSTANCE g_parentModuleHandle;

    unsigned char *m_pPrevKeyStates;
	unsigned char *m_pCurrKeyStates;
	unsigned char m_keyStates[2][256] { { 0} };

	IDirectInput8 *m_pDirectInput;
	IDirectInputDevice8 *m_pDevice;
	char m_lastChar;
};


class Mouse
{
public:
    enum MouseButton
    {
        BUTTON_LEFT   = 0x00,
        BUTTON_RIGHT  = 0x01,
        BUTTON_MIDDLE = 0x02
    };

    static Mouse &instance();
	static void init(HINSTANCE);
    
	bool buttonDown(MouseButton button) const
    { return (m_pCurrMouseState->rgbButtons[button] & 0x80) ? true : false; }

	bool buttonPressed(MouseButton button) const
    {
        return ((m_pCurrMouseState->rgbButtons[button] & 0x80)
            && !(m_pPrevMouseState->rgbButtons[button] & 0x80)) ? true : false;
    }

    bool buttonUp(MouseButton button) const
    { return (m_pCurrMouseState->rgbButtons[button] & 0x80) ? false : true; }

    bool isMouseSmoothing() const
    { return m_enableFiltering; }

	float xPosRelative() const
    { return m_deltaMouseX; }

	float yPosRelative() const
    { return m_deltaMouseY; }

    float weightModifier() const
    { return m_weightModifier; }

    float wheelPos() const
    { return m_deltaMouseWheel; }

    void setWeightModifier(float weightModifier);
    void smoothMouse(bool smooth);
	__declspec(noinline) void update();

private:
    Mouse();
    ~Mouse();

    bool create();
    void destroy();
    void performMouseSmoothing(float x, float y);
    void performMouseFiltering(float x, float y);
            
	static HINSTANCE g_parentModuleHandle;

    static const float WEIGHT_MODIFIER;
    static const int HISTORY_BUFFER_SIZE;

    IDirectInput8 *m_pDirectInput;
    IDirectInputDevice8 *m_pDevice;
    DIMOUSESTATE *m_pCurrMouseState;
    DIMOUSESTATE *m_pPrevMouseState;
    DIMOUSESTATE m_mouseStates[2];
    float m_deltaMouseX;
    float m_deltaMouseY;
    float m_deltaMouseWheel;
    float m_weightModifier;
    int m_historyBufferSize;
    int m_mouseIndex;
    bool m_enableFiltering;
    std::vector< std::pair<float,float> > m_historyBuffer;
    std::pair<float,float> m_mouseMovement[2];
};
