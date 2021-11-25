#ifndef CDISPLAYWINDOWS_H
#define CDISPLAYWINDOWS_H

#pragma once
#include "Display.h"

class CDisplayWindows : public CDesktop
{
    Q_OBJECT
public:
    explicit CDisplayWindows(QObject *parent = nullptr);
    virtual ~CDisplayWindows() override;
    
    virtual int Width() override;
    virtual int Height() override;
    virtual QImage GetDisplay(int width, int height, int x, int y) override;
    
private:
    HDC m_DC;
    HDC m_MemDc;
    HBITMAP m_Bitmap;
    int m_Width, m_Height;
    
    HDC GetDesktopDC();
};

#endif // CDISPLAYWINDOWS_H
