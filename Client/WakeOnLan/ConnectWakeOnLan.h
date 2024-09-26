// Author: Kang Lin <kl222@126.com>

#ifndef CONNECTWAKEONLAN_H
#define CONNECTWAKEONLAN_H

#include <QTime>
#include "Connect.h"
#include "ParameterWakeOnLan.h"
#include "WakeOnLanQt.h"

class CConnectWakeOnLan : public CConnect
{
    Q_OBJECT
public:
    explicit CConnectWakeOnLan(CConnecter* pConnecter,
                               QObject *parent = nullptr,
                               bool bDirectConnection = true);

    // CConnect interface
public slots:
    virtual void slotClipBoardChanged() override;

protected:
    virtual int SetConnecter(CConnecter *pConnecter) override;
    virtual int SetViewer(CFrmViewer *pView, bool bDirectConnection) override;
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
