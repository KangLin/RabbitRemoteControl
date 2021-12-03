// Author: Kang Lin <kl222@126.com>

#ifndef CSCREENWINDOWS_H
#define CSCREENWINDOWS_H

#include "Screen.h"

class CScreenWindows : public CScreen
{
public:
    explicit CScreenWindows(QObject *parent = nullptr);
    virtual ~CScreenWindows();
    
    virtual int Width() override;
    virtual int Height() override;
    virtual int VirtualTop() override;
    virtual int VirtualLeft() override;
    virtual int VirtualWidth() override;
    virtual int VirtualHeight() override;
    virtual int VisibleMonitorCount() override;

    virtual QImage GetScreen(int index = 0) override;
    virtual int SetFormat(QImage::Format f = QImage::Format_ARGB32) override;
    virtual QImage::Format GetFormat(int index = 0) override;

private:
    int GetImage(bool bBuffer);
};

#endif // CSCREENWINDOWS_H
