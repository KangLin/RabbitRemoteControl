#ifndef CDESKTOPXLIB_H
#define CDESKTOPXLIB_H

#include "Desktop.h"

class CDesktopXLib : public CDesktop
{
    Q_OBJECT
public:
    explicit CDesktopXLib(QObject *parent = nullptr);
    
    virtual int Width() override;
    virtual int Height() override;
    virtual QImage GetDesktop(int width, int height, int x, int y) override;
};

#endif // CDESKTOPXLIB_H
