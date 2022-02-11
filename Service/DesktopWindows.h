// Author: Kang Lin <kl222@126.com>

#ifndef CDESKTOPWINDOWS_H
#define CDESKTOPWINDOWS_H

#pragma once
#include "Desktop.h"

class CDesktopWindows : public CDesktop
{
    Q_OBJECT
public:
    explicit CDesktopWindows(QObject *parent = nullptr);
    virtual ~CDesktopWindows() override;
    
    virtual int Width() override;
    virtual int Height() override;
    virtual QImage GetDesktop() override;
    virtual QImage GetDesktop(int x, int y, int width, int height) override;
    virtual QImage GetCursor(QPoint &pos, QPoint &posHot) override;
    virtual QPoint GetCursorPosition() override;
    
private:
    HDC m_DC;
    HDC m_MemDC;
    HBITMAP m_Bitmap, m_DesktopBitmap;
    int m_Width, m_Height;
    QImage m_Desktop;
    
    HDC GetDesktopDC();
};

#endif // CDESKTOPWINDOWS_H
