#ifndef CDESKTOPXLIB_H
#define CDESKTOPXLIB_H

#pragma once
#include "Desktop.h"

class CDesktopXLib : public CDesktop
{
    Q_OBJECT
public:
    explicit CDesktopXLib(QObject *parent = nullptr);
    
    virtual int Width() override;
    virtual int Height() override;
    virtual QImage GetDesktop() override;
    virtual QImage GetDesktop(int x, int y, int width, int height) override;
    
    virtual QImage GetCursor(QPoint &pos, QPoint &posHot) override;
    virtual QPoint GetCursorPosition() override;
    virtual bool GetHasCursor() const;
    virtual void SetHasCursor(bool hasCursor);
};

#endif // CDESKTOPXLIB_H
