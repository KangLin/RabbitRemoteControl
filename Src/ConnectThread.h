#ifndef CCONNECTTHREAD_H
#define CCONNECTTHREAD_H

#include <QThread>
#include <FrmViewer.h>
#include "rabbitremotecontrol_export.h"

class RABBITREMOTECONTROL_EXPORT CConnectThread : public QThread
{
    Q_OBJECT
public:
    explicit CConnectThread(CFrmViewer *pView = nullptr, QObject *parent = nullptr);
    
signals:
    
public slots:
    
    // QThread interface
protected:
    virtual void run() override;
    
private:
    CFrmViewer* m_pView;
};

#endif // CCONNECTTHREAD_H
