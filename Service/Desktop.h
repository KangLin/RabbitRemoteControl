// Author: Kang Lin <kl222@126.com>

#ifndef CDESKTOP_H
#define CDESKTOP_H

#pragma once

#include "service_export.h"

#include <QRect>
#include <QImage>
#include <QObject>

class SERVICE_EXPORT CDesktop : public QObject
{
    Q_OBJECT
public:
    explicit CDesktop(QObject *parent = nullptr){};
    virtual ~CDesktop(){};

    static CDesktop* Instance();

    virtual int Width() = 0;
    virtual int Height() = 0;
    virtual QImage GetDesktop() = 0;
    virtual QImage GetDesktop(int x, int y, int width, int height) = 0;
    virtual QImage GetCursor(QPoint &pos, QPoint &posHot) = 0;
    virtual QPoint GetCursorPosition() = 0;
    virtual bool GetHasCursor() const
    {
        return m_bHasCursor;
    }
    virtual void SetHasCursor(bool newHaveCursor)
    {
        m_bHasCursor = newHaveCursor;
    }

Q_SIGNALS:
    void sigUpdate(QImage desktop, QRect rect);

private:
    bool m_bHasCursor;
};

#endif // CDESKTOP_H
