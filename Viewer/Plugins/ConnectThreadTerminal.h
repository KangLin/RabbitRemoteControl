//! @author: Kang Lin (kl222@126.com)

#ifndef CCONNECTTHREADTERMINAL_H
#define CCONNECTTHREADTERMINAL_H

#include <QThread>
#include "ConnecterPluginsTerminal.h"

class CConnectThreadTerminal : public QThread
{
    Q_OBJECT
public:
    CConnectThreadTerminal(CConnecterPluginsTerminal* pConnecter);
protected:
    virtual void run() override;
private:
    CConnecterPluginsTerminal* m_pConnecter;
};

#endif // CCONNECTTHREADTERMINAL_H
