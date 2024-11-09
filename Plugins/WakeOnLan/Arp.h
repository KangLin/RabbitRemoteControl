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

#ifdef HAVE_PCAPPLUSPLUS
    class ArpRequest {
    public:
        ArpRequest() {
            bWakeOnLan = false;
            device = nullptr;
            bOpen = false;
            tmStart = QTime::currentTime();
            nTimeout = 0;
            tmRepeat = QTime::currentTime();
            nRepeat = 0;
        }
        ~ArpRequest() {
            qDebug() << __FUNCTION__;
        }
        QSharedPointer<CParameterWakeOnLan> para;
        bool bWakeOnLan;
        pcpp::PcapLiveDevice* device;
        bool bOpen;
        QTime tmStart;
        int nTimeout;
        QTime tmRepeat;
        int nRepeat;
    };
    QMutex m_Mutex;
    QMap<QString, QSharedPointer<ArpRequest> > m_ArpRequest;
    int StopCapture(pcpp::PcapLiveDevice* device, bool close);
#endif

    int WakeOnLan(QSharedPointer<CParameterWakeOnLan> para);
    int GetMac(QSharedPointer<CParameterWakeOnLan> para,
               QSharedPointer<ArpRequest> aq = QSharedPointer<ArpRequest>());

private:
    void ListInterfaces();

private Q_SLOTS:
    void slotProcess();

private:
    CWakeOnLanQt m_Wol;

    QTimer m_Timer;
};

#endif // __ARP_H_KL_2024_11_10__
