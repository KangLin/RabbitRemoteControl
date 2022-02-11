// Author: Kang Lin <kl222@126.com>

#ifndef CDISPLAY_H
#define CDISPLAY_H

#pragma once

#include "service_export.h"

#include <QImage>

class SERVICE_EXPORT CDisplay
{
public:
    explicit CDisplay(){m_bHasCursor = true;};
    virtual ~CDisplay(){};

    static CDisplay* Instance();
    
    virtual int Width() = 0;
    virtual int Height() = 0;
    virtual int GetScreenCount() = 0;
    virtual QImage::Format GetFormat() = 0;

    virtual QImage GetDisplay() = 0;
    virtual QImage GetDisplay(int x, int y, int width, int height) = 0;
    virtual QImage GetCursor(QPoint &pos, QPoint &posHot) = 0;
    virtual QPoint GetCursorPosition() = 0;
    bool GetHasCursor() const
    {
        return m_bHasCursor;
    }
    void SetHasCursor(bool newHasCursor)
    {
        m_bHasCursor = newHasCursor;
    }
    
protected:
    virtual int Open() = 0;
    virtual int Close() = 0;

private:
    bool m_bHasCursor;
};

#endif // CDISPLAY_H
