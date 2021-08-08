#include "InputDevice.h"

CInputDevice::CInputDevice(QObject *parent) : QObject(parent)
{
    
}

int CInputDevice::KeyEvent(quint32 keysym, quint32 keycode, bool down)
{
    return 0;
}

int CInputDevice::MouseEvent(Qt::MouseButtons button, QPoint pos)
{
    return 0;
}

int CInputDevice::MouseEvent(Qt::MouseButtons button, int x, int y)
{
    return MouseEvent(button, QPoint(x, y));
}
