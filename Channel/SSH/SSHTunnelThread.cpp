// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QThread>

#include "ChannelSSHTunnelForward.h"
#include "SSHTunnelThread.h"

static Q_LOGGING_CATEGORY(log, "Channel.SSH.Tunnel.Thread")

CSSHTunnelThread::CSSHTunnelThread(QSharedPointer<CParameterChannelSSH> parameter)
    : QThread(nullptr),
    m_bExit(false),
    m_Parameter(parameter)
{}

CSSHTunnelThread::~CSSHTunnelThread()
{
    qDebug(log) << "CSSHTunnelThread::~CSSHTunnelThread";
}

void CSSHTunnelThread::run()
{
    int nRet = 0;
    qDebug(log) << "CSSHTunnelThread run";
    CChannelSSHTunnelForward* p = new CChannelSSHTunnelForward(m_Parameter);
    if(!p)
        return;

    bool check = connect(p, SIGNAL(sigServer(quint16)),
                         this, SIGNAL(sigServer(quint16)));
    Q_ASSERT(check);
    if(!p->open(QIODevice::ReadWrite)) {
        delete p;
        return;
    }

    while (!m_bExit && nRet >= 0) {
        nRet = p->Process();
        if(nRet) {
            qCritical(log) << "Process fail" << p->errorString();
            emit sigError(nRet, p->errorString());
            emit sigDisconnect();
            break;
        }
    }

    p->close();
    delete p;
    qDebug(log) << "CSSHTunnelThread end";
}
