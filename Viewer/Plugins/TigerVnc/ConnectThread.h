#ifndef CCONNECTTHREAD_H
#define CCONNECTTHREAD_H

#include <QThread>
#include "FrmViewer.h"
#include "Connecter.h"

class CConnectThread : public QThread
{
    Q_OBJECT
public:
    explicit CConnectThread(CFrmViewer* pView = nullptr,
                            CConnecter* pConnecter = nullptr,
                            QObject *parent = nullptr);

protected:
    virtual void run() override;
    
private:
    CFrmViewer* m_pView;
    CConnecter* m_pConnecter;
};

#endif // CCONNECTTHREAD_H
