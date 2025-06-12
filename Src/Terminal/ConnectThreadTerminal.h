// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTTHREADTERMINAL_H
#define CCONNECTTHREADTERMINAL_H

#include <QThread>
#include "ConnecterTerminal.h"

class CConnectThreadTerminal : public QThread
{
    Q_OBJECT
public:
    CConnectThreadTerminal(CConnecterTerminal* pConnecter);
protected:
    virtual void run() override;
private:
    CConnecterTerminal* m_pConnecter;
};

#endif // CCONNECTTHREADTERMINAL_H
