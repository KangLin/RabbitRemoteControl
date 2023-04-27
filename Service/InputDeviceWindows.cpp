// Author: Kang Lin <kl222@126.com>

#include "InputDeviceWindows.h"
#include <Windows.h>
#include <QRect>
#include <QSharedPointer>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(logDW)

QSharedPointer<CInputDevice> CInputDevice::GenerateObject()
{
    return QSharedPointer<CInputDevice>(new CInputDeviceWindows());
}

CInputDeviceWindows::CInputDeviceWindows() : CInputDevice()
{
}

CInputDeviceWindows::~CInputDeviceWindows()
{
    qDebug(logDW, "CInputDeviceX11::~CInputDeviceX11");
}

int CInputDeviceWindows::KeyEvent(quint32 keysym, quint32 keycode, bool down)
{
    return 0;
}

int CInputDeviceWindows::MouseEvent(MouseButtons buttons, QPoint pos)
{
    // - We are specifying absolute coordinates
    DWORD flags = MOUSEEVENTF_ABSOLUTE;
    
    // - Has the pointer moved since the last event?
    if (m_LastPostion != pos)
        flags |= MOUSEEVENTF_MOVE;
    
    // - If the system swaps left and right mouse buttons then we must
    //   swap them here to negate the effect, so that we do the actual
    //   action we mean to do
    if (::GetSystemMetrics(SM_SWAPBUTTON)) {
        buttons = (buttons & ~(LeftButton | RightButton));
        if (buttons & LeftButton) buttons |= LeftButton;
        if (buttons & RightButton) buttons |= RightButton;
    }
    
    // Check the left button on change state
    if((m_LastButtons & LeftButton) != (LeftButton & buttons))
    {
        if(buttons & LeftButton)
            flags |= MOUSEEVENTF_LEFTDOWN;
        else
            flags |= MOUSEEVENTF_LEFTUP;
    }
    
    // Check the middle button on change state
    if((m_LastButtons & MiddleButton) != (MiddleButton & buttons))
    {
        if(buttons & MiddleButton)
            flags |= MOUSEEVENTF_MIDDLEDOWN;
        else
            flags |= MOUSEEVENTF_MIDDLEUP;
    }
    
    // Check the right button on change state
    if((m_LastButtons & RightButton) != (RightButton & buttons))
    {
        if(buttons & RightButton)
            flags |= MOUSEEVENTF_RIGHTDOWN;
        else
            flags |= MOUSEEVENTF_RIGHTUP;
    }
    
    //TODO: Check on a mouse wheel
    DWORD mouseWheelValue = 0;
    if(UWheelButton & buttons)
    {
        flags |= MOUSEEVENTF_WHEEL;
        mouseWheelValue = 120;
    }
    if(DWheelButton & buttons)
    {
        flags |= MOUSEEVENTF_WHEEL;
        mouseWheelValue = -120;
    }
    if(LWheelButton & buttons)
    {
        flags |= MOUSEEVENTF_HWHEEL;
        mouseWheelValue = 120;
    }
    if(RWheelButton & buttons)
    {
        flags |= MOUSEEVENTF_HWHEEL;
        mouseWheelValue = -120;
    }
    
    m_LastPostion = pos;
    m_LastButtons = buttons;
    
    //    // Normilize pointer position
    //    UINT16 desktopWidth = GetSystemMetrics(SM_CXSCREEN);
    //    UINT16 desktopHeight = GetSystemMetrics(SM_CYSCREEN);
    //    int fbOffsetX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    //    int fbOffsetY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    //    INT32 x = (INT32)((pos.x() + fbOffsetX) * 65535 / (desktopWidth - 1));
    //    INT32 y = (INT32)((pos.y() + fbOffsetY)* 65535 / (desktopHeight - 1));
    
    //    INPUT input;
    //    memset(&input, 0, sizeof(INPUT));
    //    input.type = INPUT_MOUSE;
    //    input.mi.dwFlags = flags;
    //    input.mi.dx = x;
    //    input.mi.dy = y;
    //    input.mi.mouseData = mouseWheelValue;
    //    if(1 != SendInput(1, &input, sizeof(INPUT)))
    //    {
    //        qCritical(logDW, "SendInput fail: %d", GetLastError());
    //        return -1;
    //    }
    
    QRect primaryDisplay(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    //    qDebug(logDW, "primary screen:%d,%d;pos:%d,%d",
    //                    primaryDisplay.width(), primaryDisplay.height(),
    //                    pos.x(), pos.y());
    if (primaryDisplay.contains(pos)) {
        qDebug(logDW, "In primary screen");
        // mouse_event wants coordinates specified as a proportion of the
        // primary display's size, scaled to the range 0 to 65535
        QPoint scaled;
        scaled.setX((pos.x() * 65535) / (primaryDisplay.width() - 1));
        scaled.setY((pos.y() * 65535) / (primaryDisplay.height() - 1));
        ::mouse_event(flags, scaled.x(), scaled.y(), mouseWheelValue, 0);
    } else {
        // The event lies outside the primary monitor.  Under Win2K, we can just use
        // SendInput, which allows us to provide coordinates scaled to the virtual desktop.
        // SendInput is available on all multi-monitor-aware platforms.
        INPUT evt;
        evt.type = INPUT_MOUSE;
        QPoint vPos(pos.x() - GetSystemMetrics(SM_XVIRTUALSCREEN),
                    pos.y() - GetSystemMetrics(SM_YVIRTUALSCREEN));
        evt.mi.dx = (vPos.x() * 65535) / (GetSystemMetrics(SM_CXVIRTUALSCREEN)-1);
        evt.mi.dy = (vPos.y() * 65535) / (GetSystemMetrics(SM_CYVIRTUALSCREEN)-1);
        evt.mi.dwFlags = flags | MOUSEEVENTF_VIRTUALDESK;
        evt.mi.dwExtraInfo = 0;
        evt.mi.mouseData = mouseWheelValue;
        evt.mi.time = 0;
        if (SendInput(1, &evt, sizeof(evt)) != 1)
        {
            qCritical(logDW, "SendInput fail: %d", GetLastError());
            return -1;
        }
    }
    return 0;
}

int CInputDeviceWindows::MouseEvent(MouseButtons buttons, int x, int y)
{
    return MouseEvent(buttons, QPoint(x, y));
}
