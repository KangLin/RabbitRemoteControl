// Author: Kang Lin <kl222@126.com>

#include "ConnectWakeOnLan.h"
#include <QLoggingCategory>
#ifdef HAVE_PCAPPLUSPLUS
    #include <MacAddress.h>
    #include <Logger.h>
    #include <PcapPlusPlusVersion.h>
    #include <PcapLiveDeviceList.h>
    #include <PcapLiveDevice.h>
    #include <NetworkUtils.h>
#endif

static Q_LOGGING_CATEGORY(log, "WOL")
CConnectWakeOnLan::CConnectWakeOnLan(
    CConnecterConnect *pConnecter, QObject *parent)
    : CConnect{nullptr}
    , m_pParameter(nullptr)
    , m_nRepeat(0)
    , m_tmStart(QTime::currentTime())
{
    Q_ASSERT(pConnecter);
    m_pParameter = pConnecter->GetParameter();
    Q_ASSERT(m_pParameter);
}

CConnectWakeOnLan::~CConnectWakeOnLan()
{
    qDebug(log) << "CConnectWakeOnLan::~CConnectWakeOnLan()";
}

//! [Check parameters is validity]
CConnect::OnInitReturnValue CConnectWakeOnLan::OnInit()
{
    m_tmStart = QTime::currentTime();
    return OnInitReturnValue::UseOnProcess;
}
//! [Check parameters is validity]

int CConnectWakeOnLan::OnClean()
{
    return 0;
}

/*!
 * \~chinese 插件连接的具体操作处理
 * \return
 *       \li >= 0: 继续。再次调用间隔时间，单位毫秒
 *       \li = -1: 停止
 *       \li < -1: 错误
 *
 * \~english Specific operation processing of plug-in connection
 * \return
 *       \li >= 0: continue, Interval call time (msec)
 *       \li = -1: stop
 *       \li < -1: error
 * \~
 * \see Connect() slotTimeOut()
*/
int CConnectWakeOnLan::OnProcess()
{
    if(!m_pParameter) return -1;
    auto &wol = m_pParameter->m_WakeOnLan;
    do {
        int t = m_tmStart.secsTo(QTime::currentTime());
        //*
        qDebug(log) << "Delay:" << wol.GetDelay() << "Start:" << m_tmStart
                    << "Current:" << QTime::currentTime() << t
                    << "Repeat:" << m_nRepeat;//*/

        if(t >= wol.GetDelay()) {
            qDebug(log) << "Time out";
            break;
        }
        if(wol.GetRepeat() <= m_nRepeat++) {
            int n = wol.GetDelay() - t;
            qDebug(log) << "n:" << n;
            if(n > 0)
                return n * 1000;
            break;
        }
        m_Wol.SetBroadcastAddress(wol.GetBroadcastAddress());
        if(wol.GetPassword().isEmpty())
            m_Wol.SendMagicPacket(wol.GetMac());
        else
            m_Wol.SendSecureMagicPacket(wol.GetMac(), wol.GetPassword());

        QString szMac = GetMac(
            m_pParameter->m_Net.GetHost(),
            m_pParameter->m_WakeOnLan.GetNetworkInterface(),
            wol.GetInterval());
        if(szMac.isEmpty())
            return wol.GetInterval();
        return 0;
    } while(0);
    qDebug(log) << "Stop";
    m_nRepeat = 0;
    return -1;
}

QString CConnectWakeOnLan::GetMac(const QString& szTargetIp, const QString &szSourceIp, int nTimeout)
{
#ifdef HAVE_PCAPPLUSPLUS
    pcpp::MacAddress sourceMac;
    pcpp::IPv4Address sourceIP(szSourceIp.toStdString());
    pcpp::IPv4Address targetIP(szTargetIp.toStdString());
    pcpp::PcapLiveDevice* dev = nullptr;
    dev = pcpp::PcapLiveDeviceList::getInstance()
              .getPcapLiveDeviceByIpOrName(szSourceIp.toStdString());
    if (dev == nullptr) {
        qCritical(log)
            << "Couldn't find interface by provided IP address or name" << szSourceIp;
        return QString();
    }
    double arpResponseTimeMS = 0;
    if(!pcpp::IPv4Address::isValidIPv4Address(szSourceIp.toStdString()))
        sourceIP = dev->getIPv4Address();
    sourceMac = dev->getMacAddress();
    if(!pcpp::IPv4Address::isValidIPv4Address(szTargetIp.toStdString())) {
        qCritical(log) << "Target ip is invalid:" << szTargetIp;
        return QString();
    }
    // suppressing errors to avoid cluttering stdout
    pcpp::Logger::getInstance().suppressLogs();
    pcpp::MacAddress result = pcpp::NetworkUtils::getInstance().getMacAddress(
        targetIP, dev, arpResponseTimeMS, sourceMac, sourceIP, nTimeout);
    // failed fetching MAC address
    if (result == pcpp::MacAddress::Zero)
    {
        // PcapPlusPlus logger saves the last internal error message
        qCritical(log) << pcpp::Logger::getInstance().getLastError().c_str();
        return QString();
    }
    qDebug(log) << "Mac:" << result.toString().c_str();
    // Succeeded fetching MAC address
    return result.toString().c_str();
#endif
    return QString();
}
