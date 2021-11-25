// Author: Kang Lin <kl222@126.com>

#ifndef CSCREENXLIB_H
#define CSCREENXLIB_H

#include "Screen.h"
#include <X11/Xutil.h>

class CScreenXLib : public CScreen
{
public:
    explicit CScreenXLib(QObject *parent = nullptr);
    virtual ~CScreenXLib();
    
    // CScreen interface
    virtual int Width() override;
    virtual int Height() override;
    virtual int VirtualTop() override;
    virtual int VirtualLeft() override;
    virtual int VirtualWidth() override;
    virtual int VirtualHeight() override;
    virtual int VisibleMonitorCount() override;
    virtual QImage GetScreen(int index) override;

private:
    QImage::Format GetFormat(XImage* img);
    
    XImage* m_pImage;
};

#endif // CSCREENXLIB_H
