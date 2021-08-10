#ifndef CINPUTDEVICE_H
#define CINPUTDEVICE_H

#pragma once
#include "service_export.h"
#include <QPoint>
#include <QObject>

class SERVICE_EXPORT CInputDevice : public QObject
{
    Q_OBJECT

public:
    explicit CInputDevice(QObject *parent = nullptr);
    
    enum MouseButton
    {
        LeftButton = 0x01,
        RightButton = 0x02,
        MiddleButton = 0x04,
        LWheelButton = 0x08,
        RWheelButton = 0x10,
        UWheelButton = 0x20,
        DWheelButton = 0x40
    };
    Q_DECLARE_FLAGS(MouseButtons, MouseButton)
    virtual int KeyEvent(quint32 keysym, quint32 keycode, bool down = true);
    virtual int MouseEvent(MouseButtons buttons, QPoint pos);
    virtual int MouseEvent(MouseButtons buttons, int x, int y);
    
private:
    MouseButtons m_LastButtons;
    QPoint m_LastPostion;
    
};

#endif // CINPUTDEVICE_H
