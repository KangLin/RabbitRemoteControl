#ifndef CFRMTERMWIDGET_H
#define CFRMTERMWIDGET_H

#include "qtermwidget.h"

class CFrmTermWidget : public QTermWidget
{
    Q_OBJECT
    
public:
    explicit CFrmTermWidget(QWidget * parent = nullptr);
    virtual ~CFrmTermWidget();
};

#endif // CFRMTERMWIDGET_H
