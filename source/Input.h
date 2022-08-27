#pragma once

#define DIRECTINPUT_VERSION 0x0800
#define NOMINMAX

#include "dinput.h"
#include "Logger.h"
#include <queue>
#include <assert.h>

class Input
{
public:
    Input();
    ~Input();
    bool Initialize(HINSTANCE, HWND);

    void ReadKeyboardEvents();
    char NextKeyEvent();
    bool HasKeyboardEvents() const;
    bool IsKeyDown(unsigned char) const;

    void ReadMouseEvents();
    char NextMouseEvent();
    bool HasMouseEvents() const;
    int GetMouseDeltaX() const;
    int GetMouseDeltaY() const;
    bool IsButtonDown(int) const;

    void GetCursorPos(int& x, int &y) const;
    void UpdateCursorPos();

    void Acquire();
    void Unacquire();

    static void ShowCursor(bool);

private:
    std::queue<DIDEVICEOBJECTDATA*> m_pKeyboardEvents;
    std::queue<DIDEVICEOBJECTDATA*> m_pMouseEvents;

    LPDIRECTINPUT8 m_pDI;
    LPDIRECTINPUTDEVICE8 m_pKeyboard;
    LPDIRECTINPUTDEVICE8 m_pMouse;

    HWND m_hWnd;
    bool m_keys[256]; // The state of the Keyboard
    unsigned int m_mouseButtons; // One bit per mouse button
    int m_mouseDx, m_mouseDy;
    int m_cursorX, m_cursorY;
};
