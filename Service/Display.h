// Author: Kang Lin <kl222@126.com>

#ifndef CDISPLAY_H
#define CDISPLAY_H

#pragma once

#include "service_export.h"

#include <QImage>

class SERVICE_EXPORT CDisplay
{
public:
    explicit CDisplay(){};
    virtual ~CDisplay(){};

    static CDisplay* Instance();
    
    virtual int Width() = 0;
    virtual int Height() = 0;
    virtual int GetScreenCount() = 0;
    virtual QImage::Format GetFormat() = 0;

    virtual QImage GetDisplay() = 0;
    virtual QImage GetDisplay(int x, int y, int width, int height) = 0;

protected:
    virtual int Open() = 0;
    virtual int Close() = 0;

};

#endif // CDISPLAY_H
