// Author: Kang Lin <kl222@126.com>

#ifndef CONNECTWAKEONLAN_H
#define CONNECTWAKEONLAN_H

#include <QTime>
#include "WakeOnLanQt.h"
#include "ConnecterWakeOnLan.h"

class CLIENT_EXPORT CConnectWakeOnLan : public CConnect
{
    Q_OBJECT
public:
    explicit CConnectWakeOnLan(CConnecterWakeOnLan* pConnecter,
                               QObject *parent = nullptr);
    virtual ~CConnectWakeOnLan();

protected:
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;
    virtual int OnProcess() override;

public:
    QString GetMac(const QString& szTargetIp, const QString& szSourceIp, int nTimeout);
    void ListInterfaces();

private:
    CWakeOnLanQt m_Wol;
    CParameterWakeOnLan* m_pParameter;
    int m_nRepeat;
    QTime m_tmStart;
};

#endif // CONNECTWAKEONLAN_H
