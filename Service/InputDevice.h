// Author: Kang Lin <kl222@126.com>

#ifndef CINPUTDEVICE_H
#define CINPUTDEVICE_H

#pragma once
#include "service_export.h"
#include <QPoint>
#include <QSharedPointer>

class SERVICE_EXPORT CInputDevice
{
public:
    static QSharedPointer<CInputDevice> GenerateObject();
    
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
    Q_ENUM(MouseButton)
    Q_DECLARE_FLAGS(MouseButtons, MouseButton)
    Q_FLAG(MouseButtons)
    virtual int KeyEvent(quint32 keysym, quint32 keycode, bool down = true) = 0;
    virtual int MouseEvent(MouseButtons buttons, QPoint pos) = 0;
    virtual int MouseEvent(MouseButtons buttons, int x, int y) = 0;
    
protected:
    MouseButtons m_LastButtons;
    QPoint m_LastPostion;

    explicit CInputDevice(){}
};

// 在类外部声明操作符（通常放在头文件末尾）
Q_DECLARE_OPERATORS_FOR_FLAGS(CInputDevice::MouseButtons)
#endif // CINPUTDEVICE_H
