//------------------------------------------------------------------------------
// File: Input.cpp
//  
// Handles keyboard and mouse input
//------------------------------------------------------------------------------

#include "Input.h"
#pragma warning(disable : 4644) // Warns about the use of Offsetof inside DIMOFS_X

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
Input::Input(void)
{
    m_pMouse = NULL;
    m_pKeyboard = NULL;
    m_pDI = NULL;
    m_pKeyboard = NULL;
    for(int i = 0; i < 256; i++)
        m_keys[i] = false;
    m_mouseButtons = 0;
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
Input::~Input(void)
{
    if(m_pDI != NULL)
    {
        if(m_pKeyboard != NULL)
        {
            m_pKeyboard->Unacquire();
            m_pKeyboard->Release();
        }
        m_pDI->Release();
    }
}

//------------------------------------------------------------------------------
// Initializes the Directinput mouse and keyboard
//------------------------------------------------------------------------------
bool Input::Initialize(HINSTANCE hInstance, HWND hWnd)
{
    m_hWnd = hWnd;
    if(FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDI, NULL)))
    {
        logger.File("Input::Initialize failed at DirectInput8Create");
        return false;
    }

    if(FAILED(m_pDI->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, NULL)))
    {
        logger.File("Input::Initialize failed at CreateDevice (keyboard)"); 
        return false; 
    } 

    if(FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
    {
        logger.File("Input::Initialize failed at SetDataFormat (keyboard)");
        return false;
    }

    if(FAILED(m_pKeyboard->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
    {
        logger.File("Input::Initialize failed at SetCooperativeLevel (keyboard)");
        return false; 
    }
    DIPROPDWORD dipdw;
    dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj        = 0;
    dipdw.diph.dwHow        = DIPH_DEVICE;
    dipdw.dwData            = 32;
 
    if (FAILED(m_pKeyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
    {
        logger.File("Input::Initialize failed at SetProperty");
        return false;
    }

    m_pKeyboard->Acquire();

    if (FAILED(m_pDI->CreateDevice(GUID_SysMouse, &m_pMouse, NULL)))
    {
        logger.File("Input::Initialize failed at CreateDevice (mouse)");
        return false;
    }

    if (FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
    {
        logger.File("Input::Initialize failed at CreateDevice (mouse)");
        return false;
    }

    if (FAILED(m_pMouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
    {
        logger.File("Input::Initialize failed at CreateDevice (mouse)");
        return false;
    }

    dipdw.dwData            = 1024;

    if (FAILED(m_pMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
    {
        logger.File("Input::Initialize failed at SetProperty (mouse)");
        return false;
    }

    m_pMouse->Acquire();
    return true;
}

//------------------------------------------------------------------------------
// Reads the mouse events into the buffer
//------------------------------------------------------------------------------
void Input::ReadMouseEvents()
{
    DWORD dwElements;
    HRESULT hr;
    DIDEVICEOBJECTDATA* od;

    while(true)
    {
        dwElements = 1;
        od = new DIDEVICEOBJECTDATA;
        hr = m_pMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), od, &dwElements, 0);
        if(FAILED(hr))
        {
            m_mouseButtons = 0;
            m_pMouse->Acquire();
            hr = m_pMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), od, &dwElements, 0);
        }

        if (FAILED(hr) || dwElements == 0)
        {
            delete od;
            break;
        }
        m_pMouseEvents.push(od);
    }
}

//------------------------------------------------------------------------------
// Gives the app access to the keyboard and mouse
//------------------------------------------------------------------------------
void Input::Acquire()
{
    for(int i = 0; i < 256; i++)
    m_keys[i] = false;
    m_mouseButtons = false;
    if(m_pMouse)
        m_pMouse->Acquire();
    if(m_pKeyboard)
        m_pKeyboard->Acquire();
}

//------------------------------------------------------------------------------
// Lets other applications have access to the keyboard and mouse
//------------------------------------------------------------------------------
void Input::Unacquire()
{
    if(m_pMouse)
        m_pMouse->Unacquire();
    if(m_pKeyboard)
        m_pKeyboard->Unacquire();
}

//------------------------------------------------------------------------------
// Reads the keyboard events since the last call to this function into a buffer
//------------------------------------------------------------------------------
void Input::ReadKeyboardEvents()
{
    DWORD dwElements;
    HRESULT hr;
    DIDEVICEOBJECTDATA* od;

    while(true)
    {
        dwElements = 1;
        od = new DIDEVICEOBJECTDATA;
        hr = m_pKeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), od, &dwElements, 0);
        if(FAILED(hr))
        {
            for(int i = 0; i < 256; i++)
                m_keys[i] = false;
            m_pKeyboard->Acquire();
            hr = m_pKeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), od, &dwElements, 0);
        }

        if(FAILED(hr) || dwElements == 0)
        {
            delete od;
            break;
        }
        m_pKeyboardEvents.push(od);
    }
}

//------------------------------------------------------------------------------
// Returns true if the buffer has mouse events
//------------------------------------------------------------------------------
bool Input::HasMouseEvents() const
{
    return !m_pMouseEvents.empty();
}

//------------------------------------------------------------------------------
// Returns information on the frontmost event in the mouse event queue, and
// removes it from the queue
//------------------------------------------------------------------------------
char Input::NextMouseEvent()
{
    DIDEVICEOBJECTDATA* od;
    DWORD ofs;
    assert(!m_pMouseEvents.empty());
    od = m_pMouseEvents.front(); 
    ofs = od->dwOfs;
    switch (ofs)
    {
    case DIMOFS_X:
        m_mouseDx = od->dwData;
        break;
    case DIMOFS_Y:
        m_mouseDy = od->dwData;
        break;

    case DIMOFS_BUTTON0:
        if (od->dwData != 0)
            m_mouseButtons |= 1;
        else
            m_mouseButtons &= ~1;
        break;

    case DIMOFS_BUTTON1:
        if (od->dwData != 0)
            m_mouseButtons |= 2;
        else
            m_mouseButtons &= ~2;
        break;

    case DIMOFS_BUTTON2:
        if (od->dwData != 0)
            m_mouseButtons |= 4;
        else
            m_mouseButtons &= ~4;
        break;
    }
    m_pMouseEvents.pop();
    delete od;
    return (char)ofs;
}

//------------------------------------------------------------------------------
// Returns the amount of units moved by the mouse in the last mouse move event
//------------------------------------------------------------------------------
int Input::GetMouseDeltaX() const
{
    return m_mouseDx;
}

//------------------------------------------------------------------------------
// Returns the amount of units moved by the mouse in the last mouse move event
//------------------------------------------------------------------------------
int Input::GetMouseDeltaY() const
{
    return m_mouseDy;
}

//------------------------------------------------------------------------------
// Checks if the requested mouse button is pressed
//------------------------------------------------------------------------------
bool Input::IsButtonDown(int ofs) const
{
    switch(ofs)
    {
    case DIMOFS_BUTTON0:
        return ((m_mouseButtons & 1) == 1);
    case DIMOFS_BUTTON1:
        return ((m_mouseButtons & 2) == 2);
    case DIMOFS_BUTTON2:
        return ((m_mouseButtons & 4) == 4);
    }
    assert(false);
    return false;
}

//------------------------------------------------------------------------------
// Returns true if there have been more events since the last call to 
// ReadKeyboardEvents
//------------------------------------------------------------------------------
bool Input::HasKeyboardEvents() const
{
    return !m_pKeyboardEvents.empty();
}

//------------------------------------------------------------------------------
// Pops the frontmost item in the keyboard events queue, and returns it       
//------------------------------------------------------------------------------
char Input::NextKeyEvent()
{
    DIDEVICEOBJECTDATA* od;
    DWORD ofs;
    assert(!m_pKeyboardEvents.empty());
    od = m_pKeyboardEvents.front(); 
    ofs = od->dwOfs;
    m_keys[ofs] = ((od->dwData & 0x0080) == 0x0080);
    m_pKeyboardEvents.pop();
    delete od;
    return (char)ofs;
}

//------------------------------------------------------------------------------
// Checks if a keyboard key is down, with the DI defined key handle as argument
//------------------------------------------------------------------------------
bool Input::IsKeyDown(unsigned char c) const
{
    return m_keys[c];
}

//------------------------------------------------------------------------------
// Returns the current x and y coordinates of the mouse cursor, when in
// nonexclusive mode
//------------------------------------------------------------------------------
void Input::GetCursorPos(int& x, int &y) const
{
    x = m_cursorX;
    y = m_cursorY;
}

//------------------------------------------------------------------------------
// Lets the class know about the current position of the cursor inside the
// client area
//------------------------------------------------------------------------------
void Input::UpdateCursorPos()
{
    POINT point;
    ::GetCursorPos(&point);
    ::ScreenToClient(m_hWnd, &point);
    m_cursorX = point.x;
    m_cursorY = point.y;
}

//------------------------------------------------------------------------------
// Shows/hides the mouse cursor
//------------------------------------------------------------------------------
void Input::ShowCursor(bool bewl)
{
    ::ShowCursor(bewl);
}