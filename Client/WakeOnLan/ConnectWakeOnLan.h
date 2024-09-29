// Author: Kang Lin <kl222@126.com>

#ifndef CONNECTWAKEONLAN_H
#define CONNECTWAKEONLAN_H

#include <QTime>
#include "Connect.h"
#include "ConnecterThread.h"
#include "ParameterWakeOnLan.h"
#include "WakeOnLanQt.h"

class CConnectWakeOnLan : public CConnect
{
    Q_OBJECT
public:
    explicit CConnectWakeOnLan(CConnecterThread* pConnecter,
                               QObject *parent = nullptr);

protected:
    virtual int SetConnecter(CConnecter *pConnecter) override;
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;
    virtual int OnProcess() override;

private:
    CWakeOnLanQt m_Wol;
    CParameterBase* m_pParameter;
    int m_nRepeat;
    QTime m_tmStart;
};

#endif // CONNECTWAKEONLAN_H
