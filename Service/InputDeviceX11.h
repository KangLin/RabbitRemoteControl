#ifndef CINPUTDEVICEX11_H
#define CINPUTDEVICEX11_H

#include "InputDevice.h"

class CInputDeviceX11 : public CInputDevice
{
    Q_OBJECT

public:
    explicit CInputDeviceX11(QObject *parent = nullptr);
    virtual ~CInputDeviceX11();
    
    virtual int KeyEvent(quint32 keysym, quint32 keycode, bool down = true);
    virtual int MouseEvent(MouseButtons buttons, QPoint pos);
    virtual int MouseEvent(MouseButtons buttons, int x, int y);
};

#endif // CINPUTDEVICEX11_H
