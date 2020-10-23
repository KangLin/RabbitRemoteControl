#ifndef CCONNECTTHREAD_H
#define CCONNECTTHREAD_H

#include <QThread>
#include "FrmViewer.h"

class CConnectThread : public QThread
{
    Q_OBJECT
public:
    explicit CConnectThread(CFrmViewer* pView = nullptr, QObject *parent = nullptr);

protected:
    virtual void run() override;
    
private:
    CFrmViewer* m_pView;
};

#endif // CCONNECTTHREAD_H
