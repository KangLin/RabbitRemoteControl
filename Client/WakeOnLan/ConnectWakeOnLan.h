// Author: Kang Lin <kl222@126.com>

#ifndef CONNECTWAKEONLAN_H
#define CONNECTWAKEONLAN_H

#include <QTime>
#include "ConnecterConnect.h"
#include "WakeOnLanQt.h"

class CLIENT_EXPORT CConnectWakeOnLan : public CConnect
{
    Q_OBJECT
public:
    explicit CConnectWakeOnLan(CConnecterConnect* pConnecter,
                               QObject *parent = nullptr);
    virtual ~CConnectWakeOnLan();

protected:
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;
    virtual int OnProcess() override;

private:
    QString GetMac(const QString& szTargetIp, const QString& szSourceIp, int nTimeout);
    void ListInterfaces();

private:
    CWakeOnLanQt m_Wol;
    CParameterBase* m_pParameter;
    int m_nRepeat;
    QTime m_tmStart;
};

#endif // CONNECTWAKEONLAN_H
