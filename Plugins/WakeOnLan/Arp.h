// Author: Kang Lin <kl222@126.com>

#ifndef __ARP_H_KL_2024_11_10__
#define __ARP_H_KL_2024_11_10__

#pragma once

#include <QObject>
#include <QTime>
#include <QTimer>
#include <QMap>

#ifdef HAVE_PCAPPLUSPLUS
    #include <PcapLiveDevice.h>
#endif

#include "WakeOnLanQt.h"
#include "ParameterWakeOnLan.h"

class CArp : public QObject
{
    Q_OBJECT
public:
    explicit CArp(QObject *parent = nullptr);
    virtual ~CArp();

    int WakeOnLan(QSharedPointer<CParameterWakeOnLan> para);

#ifdef HAVE_PCAPPLUSPLUS
    class ArpRequest
    {
    public:
        ArpRequest(QSharedPointer<CParameterWakeOnLan> p) {
            para = p;
            if(!para) return;
            nRepeat = p->GetRepeat();
            nTimeout = p->GetTimeOut();
            tmRepeat = QTime::currentTime();
            bWakeOnLan = false;
            tmStart = QTime::currentTime();
        }
        QSharedPointer<CParameterWakeOnLan> para;
        QTime tmRepeat;
        int nRepeat;
        int nTimeout;
        bool bWakeOnLan;
        QTime tmStart;
    };
    QMutex m_Mutex;
    QMap<std::string, QSharedPointer<ArpRequest> > m_Para;
private:
    void ListInterfaces();
    int StopCapture();
    int SendArpPackage(pcpp::PcapLiveDevice* device,
                       std::string szSourceIp, std::string szTargetIp);
#endif

public:
    int GetMac(QSharedPointer<CParameterWakeOnLan> para
#ifdef HAVE_PCAPPLUSPLUS
               ,QSharedPointer<ArpRequest> ar = QSharedPointer<ArpRequest>()
#endif
               );

private Q_SLOTS:
    void slotProcess();

private:
    CWakeOnLanQt m_Wol;

    QTimer m_Timer;
};

#endif // __ARP_H_KL_2024_11_10__
