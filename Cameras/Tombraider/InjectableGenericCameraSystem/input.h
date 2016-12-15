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
    static Keyboard &instance();
	static void init(HINSTANCE);
	bool create();		// will kill input from this device for the game 
	void destroy();		// will resume the killed input from this device.

	bool keyDown(int virtualKeyCode);

private:
    Keyboard();
    ~Keyboard();
	static HINSTANCE g_parentModuleHandle;
	IDirectInput8 *m_pDirectInput;
	IDirectInputDevice8 *m_pDevice;
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

