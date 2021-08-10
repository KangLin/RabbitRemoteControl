#ifndef CSCREEN_H
#define CSCREEN_H

#include <QObject>
#include "service_export.h"

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
};

#endif // CSCREEN_H
