// Author: Kang Lin <kl222@126.com>

#ifndef CDISPLAYPXLIB_H
#define CDISPLAYPXLIB_H

#pragma once

#include "Display.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>

class CDisplayXLib : public CDisplay
{
public:
    explicit CDisplayXLib();
    virtual ~CDisplayXLib() override;

    virtual int Width() override;
    virtual int Height() override;
    virtual int GetScreenCount() override;
    virtual QImage::Format GetFormat() override;
    virtual QImage GetDisplay(int x, int y, int width, int height) override;
    virtual QImage GetDisplay() override;
    virtual QImage GetCursor(QPoint &pos, QPoint &posHot) override;
    virtual QPoint GetCursorPosition() override;
    
protected:
    virtual int Open() override;
    virtual int Close() override;

private:
    Display* m_pDisplay;
    Window m_RootWindow;
    QImage::Format m_Format;
    XImage *m_pImage;
    QImage m_Desktop;
    
    QImage::Format GetFormat(XImage* pImage);
    static void DestroyImage(void *pImage);
};

#endif // CDISPLAYPXLIB_H
