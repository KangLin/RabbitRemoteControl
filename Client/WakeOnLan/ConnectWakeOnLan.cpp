// Author: Kang Lin <kl222@126.com>

#include "ConnectWakeOnLan.h"
#include <QLoggingCategory>

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
        return wol.GetInterval();
    } while(0);
    qDebug(log) << "Stop";
    return -1;
}
