#ifndef CCONNECTTHREADLIBVNC_H
#define CCONNECTTHREADLIBVNC_H

#include <QThread>
#include "Connecter.h"

class CConnectThreadLibVnc : public QThread
{
public:
    explicit CConnectThreadLibVnc(CFrmViewer* pView = nullptr,
                                  CConnecter* pConnecter = nullptr,
                                  QObject *parent = nullptr);
    virtual ~CConnectThreadLibVnc() override;
    
    bool m_bExit;
    
protected:
    virtual void run() override;
    
private:
    CFrmViewer* m_pView;
    CConnecter* m_pConnecter;
};

#endif // CCONNECTTHREADLIBVNC_H
