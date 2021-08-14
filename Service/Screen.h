#ifndef CSCREEN_H
#define CSCREEN_H

#include <QObject>
#include "service_export.h"
#include <QImage>

class SERVICE_EXPORT CScreen : public QObject
{
    Q_OBJECT
public:
    CScreen(QObject* parent = nullptr);
    
    int Width();
    int Height();
    
    int VirtualTop();
    int VirtualLeft();
    int VirtualWidth();
    int VirtualHeight();
    
    // Windows contain both visible and invisible pseudo-monitors
    // that are associated with mirroring drivers.
    // The function returns only visible monitor count.
    int VisibleMonitorCount();
    
    virtual QImage GetDesktop();
    
Q_SIGNALS:
    void sigUpdate(QImage screen);
};

#endif // CSCREEN_H
