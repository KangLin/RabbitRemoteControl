#ifndef CDESKTOPWINDOWS_H
#define CDESKTOPWINDOWS_H

#include "Desktop.h"

class CDesktopWindows : public CDesktop
{
    Q_OBJECT
public:
    explicit CDesktopWindows(QObject *parent = nullptr);
    virtual ~CDesktopWindows() override;
    
    virtual int Width() override;
    virtual int Height() override;
    virtual QImage GetDesktop(int width, int height, int x, int y) override;
    
private:
    HDC m_DC;
    HDC m_MemDc;
    HBITMAP m_Bitmap;
    int m_Width, m_Height;
    
    HDC GetDesktopDC();
};

#endif // CDESKTOPWINDOWS_H
