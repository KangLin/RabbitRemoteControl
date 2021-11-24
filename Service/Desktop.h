#ifndef CDESKTOP_H
#define CDESKTOP_H

#pragma once

#include "service_export.h"

#include <QRect>
#include <QImage>

class SERVICE_EXPORT CDesktop : public QObject
{
    Q_OBJECT
public:
    explicit CDesktop(QObject *parent = nullptr){};
    virtual ~CDesktop(){};

    static CDesktop* Instance();
    
    virtual int Width() = 0;
    virtual int Height() = 0;

    virtual QImage GetDesktop(int width = -1, int height = -1,
                              int x = 0, int y = 0) = 0;
    
Q_SIGNALS:
    void sigUpdate(QImage &desktop, QRect rect);

};

#endif // CDESKTOP_H
