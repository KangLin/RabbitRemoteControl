// Author: Kang Lin <kl222@126.com>

#ifndef CSCREEN_H
#define CSCREEN_H

#include <QObject>
#include "service_export.h"
#include <QImage>

/*!
 * \brief The CScreen class. The descript a screen.
 */
class SERVICE_EXPORT CScreen : public QObject
{
    Q_OBJECT
public:
    CScreen(QObject* parent = nullptr) : QObject(parent),
        m_Format(QImage::Format_Invalid),
        m_bCursor(true),
        m_nNumber(-1)
    {}
    
    static CScreen* Instance();

    virtual int Width() = 0;
    virtual int Height() = 0;
    
    virtual int VirtualTop() = 0;
    virtual int VirtualLeft() = 0;
    virtual int VirtualWidth() = 0;
    virtual int VirtualHeight() = 0;
    
    // Windows contain both visible and invisible pseudo-monitors
    // that are associated with mirroring drivers.
    // The function returns only visible monitor count.
    virtual int VisibleMonitorCount() = 0;
    
    virtual QImage GetScreen(int index = 0) = 0;
    virtual QImage::Format GetFormat(int index = 0)
    {
        return m_Format;
    }
    virtual int SetFormat(QImage::Format f = QImage::Format_ARGB32)
    {
        m_Format = f;
        return 0;
    }
    
    bool HasCursor(){return m_bCursor;}
    void SetHasCursor(bool bHas){m_bCursor = bHas;}

Q_SIGNALS:
    void sigUpdate(QImage screen);
    
protected:
    QImage m_Screen;
    QImage::Format m_Format;
    bool m_bCursor;
    int m_nNumber;
};

#endif // CSCREEN_H
