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

    virtual int KeyEvent(quint32 keysym, quint32 keycode, bool down = true);
    virtual int MouseEvent(Qt::MouseButtons button, QPoint pos);
    virtual int MouseEvent(Qt::MouseButtons button, int x, int y);
    
};

#endif // CINPUTDEVICE_H
