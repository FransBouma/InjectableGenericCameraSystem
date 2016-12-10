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
#include "stdafx.h"
#include "input.h"

//-----------------------------------------------------------------------------
// Keyboard.
//-----------------------------------------------------------------------------

HINSTANCE Keyboard::g_parentModuleHandle = NULL;

Keyboard &Keyboard::instance()
{
    static Keyboard theInstance;
    return theInstance;
}

Keyboard::Keyboard()
{
	Keyboard::g_parentModuleHandle = NULL;
    m_pDirectInput = 0;
    m_pDevice = 0;
    m_pCurrKeyStates = m_keyStates[0];
    m_pPrevKeyStates = m_keyStates[1];
    m_lastChar = 0;
}

Keyboard::~Keyboard()
{
    destroy();
}


void Keyboard::init(HINSTANCE parentModuleHandle)
{
	Keyboard::g_parentModuleHandle = parentModuleHandle;
}


bool Keyboard::create()
{
	HINSTANCE hInstance = NULL == g_parentModuleHandle ? GetModuleHandle(0) : Keyboard::g_parentModuleHandle;
    HWND hWnd = GetForegroundWindow();

    if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION,
            IID_IDirectInput8, reinterpret_cast<void**>(&m_pDirectInput), 0)))
        return false;

    if (FAILED(m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pDevice, 0)))
        return false;

    if (FAILED(m_pDevice->SetDataFormat(&c_dfDIKeyboard)))
        return false;

    if (FAILED(m_pDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
        return false;

    if (FAILED(m_pDevice->Acquire()))
        return false;

    memset(m_keyStates, 0, sizeof(m_keyStates));
    return true;
}

void Keyboard::destroy()
{
    if (m_pDevice)
    {
        m_pDevice->Unacquire();
        m_pDevice->Release();
        m_pDevice = 0;
    }

    if (m_pDirectInput)
    {
        m_pDirectInput->Release();
        m_pDirectInput = 0;
    }
}

void Keyboard::handleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CHAR:
        m_lastChar = static_cast<char>(wParam);
        break;

    default:
        break;
    }
}

void Keyboard::update()
{
    if (!m_pDirectInput || !m_pDevice)
    {
        if (!create())
        {
            destroy();
            return;
        }
    }

    HRESULT hr = 0;
    unsigned char *pTemp = m_pPrevKeyStates;

    m_pPrevKeyStates = m_pCurrKeyStates;
    m_pCurrKeyStates = pTemp;
    
    while (true)
    {
        hr = m_pDevice->GetDeviceState(256, m_pCurrKeyStates);
        
        if (FAILED(hr))
        {
            if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
            {
                if (FAILED(m_pDevice->Acquire()))
                    return;
            }
        }
        else
        {
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Mouse.
//-----------------------------------------------------------------------------

const float Mouse::WEIGHT_MODIFIER = 0.2f;
const int Mouse::HISTORY_BUFFER_SIZE = 10;
HINSTANCE Mouse::g_parentModuleHandle = NULL;

Mouse &Mouse::instance()
{
    static Mouse theInstance;
    return theInstance;
}

Mouse::Mouse()
{
	Mouse::g_parentModuleHandle = NULL;
    m_pDevice = 0;
    m_pCurrMouseState = &m_mouseStates[0];
    m_pPrevMouseState = &m_mouseStates[1];
    
    m_deltaMouseX = 0.0f;
    m_deltaMouseY = 0.0f;
    m_deltaMouseWheel = 0.0f;
    
    m_weightModifier = WEIGHT_MODIFIER;
    m_enableFiltering = true;

    m_historyBufferSize = HISTORY_BUFFER_SIZE;
    m_historyBuffer.resize(m_historyBufferSize);
}

Mouse::~Mouse()
{
    destroy();
}


void Mouse::init(HINSTANCE parentModuleHandle)
{
	Mouse::g_parentModuleHandle = parentModuleHandle;
}

bool Mouse::create()
{
	HINSTANCE hInstance = NULL == g_parentModuleHandle ? GetModuleHandle(0) : Mouse::g_parentModuleHandle;
    HWND hWnd = GetForegroundWindow();

    if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION,
            IID_IDirectInput8, reinterpret_cast<void**>(&m_pDirectInput), 0)))
        return false;

    if (FAILED(m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pDevice, 0)))
        return false;

    if (FAILED(m_pDevice->SetDataFormat(&c_dfDIMouse)))
        return false;

    if (FAILED(m_pDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
        return false;

    if (FAILED(m_pDevice->Acquire()))
        return false;

    memset(m_mouseStates, 0, sizeof(m_mouseStates));
    memset(&m_historyBuffer[0], 0, m_historyBuffer.size());

    m_mouseIndex = 0;
    m_mouseMovement[0].first = m_mouseMovement[0].second = 0.0f;
    m_mouseMovement[1].first = m_mouseMovement[2].second = 0.0f;

    return true;
}

void Mouse::destroy()
{
    if (m_pDevice)
    {
        m_pDevice->Unacquire();
        m_pDevice->Release();
        m_pDevice = 0;
    }

    if (m_pDirectInput)
    {
        m_pDirectInput->Release();
        m_pDirectInput = 0;
    }
}

void Mouse::update()
{
    if (!m_pDirectInput || !m_pDevice)
    {
        if (!create())
        {
            destroy();
            return;
        }
    }

    HRESULT hr = 0;
    DIMOUSESTATE *pTemp = m_pPrevMouseState;

    m_pPrevMouseState = m_pCurrMouseState;
    m_pCurrMouseState = pTemp;

    while (true)
    {
        hr = m_pDevice->GetDeviceState(sizeof(DIMOUSESTATE), m_pCurrMouseState);

        if (FAILED(hr))
        {
            if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
            {
                if (FAILED(m_pDevice->Acquire()))
                    return;
            }
        }
        else
        {
            break;
        }
    }

    if (m_enableFiltering)
    {
        performMouseFiltering(
            static_cast<float>(m_pCurrMouseState->lX),
            static_cast<float>(m_pCurrMouseState->lY));

        performMouseSmoothing(m_deltaMouseX, m_deltaMouseY);
    }
    else
    {
        m_deltaMouseX = static_cast<float>(m_pCurrMouseState->lX);
        m_deltaMouseY = static_cast<float>(m_pCurrMouseState->lY);
    }

    if (m_pCurrMouseState->lZ > 0)
        m_deltaMouseWheel = 1.0f;           // wheel rolled forwards
    else if (m_pCurrMouseState->lZ < 0)
        m_deltaMouseWheel = -1.0f;          // wheel rolled backwards
    else
        m_deltaMouseWheel = 0.0f;           // wheel hasn't moved
}

void Mouse::performMouseSmoothing(float x, float y)
{
    // Average the mouse movement across a couple of frames to smooth
    // out mouse movement.

    m_mouseMovement[m_mouseIndex].first = x;
    m_mouseMovement[m_mouseIndex].second = y;

    m_deltaMouseX = (m_mouseMovement[0].first + m_mouseMovement[1].first) * 0.5f;
    m_deltaMouseY = (m_mouseMovement[0].second + m_mouseMovement[1].second) * 0.5f;

    m_mouseIndex ^= 1;
    m_mouseMovement[m_mouseIndex].first = 0.0f;
    m_mouseMovement[m_mouseIndex].second = 0.0f;
}

void Mouse::performMouseFiltering(float x, float y)
{
    // Filter the relative mouse movement based on a weighted sum of the mouse
    // movement from previous frames to ensure that the mouse movement this
    // frame is smooth.
    //
    // For further details see:
    //  Nettle, Paul "Smooth Mouse Filtering", flipCode's Ask Midnight column.
    //  http://www.flipcode.com/cgi-bin/fcarticles.cgi?show=64462

    for (int i = m_historyBufferSize - 1; i > 0; --i)
    {
        m_historyBuffer[i].first = m_historyBuffer[i - 1].first;
        m_historyBuffer[i].second = m_historyBuffer[i - 1].second;
    }

    m_historyBuffer[0].first = x;
    m_historyBuffer[0].second = y;

    float averageX = 0.0f;
    float averageY = 0.0f;
    float averageTotal = 0.0f;
    float currentWeight = 1.0f;

    for (int i = 0; i < m_historyBufferSize; ++i)
    {
        averageX += m_historyBuffer[i].first * currentWeight;
        averageY += m_historyBuffer[i].second * currentWeight;
        averageTotal += 1.0f * currentWeight;
        currentWeight *= m_weightModifier;
    }

    m_deltaMouseX = averageX / averageTotal;
    m_deltaMouseY = averageY / averageTotal;
}

void Mouse::setWeightModifier(float weightModifier)
{
    m_weightModifier = weightModifier;
}

void Mouse::smoothMouse(bool smooth)
{
    m_enableFiltering = smooth;
}