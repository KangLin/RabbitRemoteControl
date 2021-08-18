#include "InputDevice.h"
#include "RabbitCommonLog.h"

CInputDevice::CInputDevice(QObject *parent) : QObject(parent)
{
}

CInputDevice::~CInputDevice()
{
    LOG_MODEL_DEBUG("InputDevcie", "CInputDevice::~CInputDevice()");
}

int CInputDevice::KeyEvent(quint32 keysym, quint32 keycode, bool down)
{
    return 0;
}

int CInputDevice::MouseEvent(MouseButtons buttons, QPoint pos)
{
    return 0;
}

int CInputDevice::MouseEvent(MouseButtons buttons, int x, int y)
{
    return MouseEvent(buttons, QPoint(x, y));
}
