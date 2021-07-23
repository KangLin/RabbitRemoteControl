#ifndef CFRMTERMWIDGET_H
#define CFRMTERMWIDGET_H

#include "qtermwidget.h"

class CFrmTermWidget : public QTermWidget
{
    Q_OBJECT
    
public:
    explicit CFrmTermWidget(QWidget * parent = nullptr);
    virtual ~CFrmTermWidget();
    
Q_SIGNALS:
    void sigZoomReset();
    
private Q_SLOTS:
    void slotCustomContextMenuCall(const QPoint & pos);
    void slotActivateUrl(const QUrl& url, bool fromContextMenu);
    
};

#endif // CFRMTERMWIDGET_H
