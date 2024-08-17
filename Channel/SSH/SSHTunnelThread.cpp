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
{
    qDebug(log) << "CSSHTunnelThread::CSSHTunnelThread";
    bool check = false;
    check = connect(this, SIGNAL(finished()),
                    this, SLOT(deleteLater()));
    Q_ASSERT(check);
}

CSSHTunnelThread::~CSSHTunnelThread()
{
    qDebug(log) << "CSSHTunnelThread::~CSSHTunnelThread";
}

void CSSHTunnelThread::Exit()
{
    m_bExit = true;
}

void CSSHTunnelThread::run()
{
    int nRet = 0;
    qDebug(log) << "CSSHTunnelThread run";
    CChannelSSHTunnelForward* p = new CChannelSSHTunnelForward(m_Parameter);
    if(!p)
        return;
    
    do{
        bool check = connect(p, SIGNAL(sigServer(quint16)),
                             this, SIGNAL(sigServer(quint16)));
        Q_ASSERT(check);
        check = connect(p, SIGNAL(sigServer(QString)),
                             this, SIGNAL(sigServer(QString)));
        Q_ASSERT(check);
        if(!p->open(QIODevice::ReadWrite)) {
            break;
        }
        
        while (!m_bExit && nRet >= 0) {
            nRet = p->Process();
            if(nRet) {
                qCritical(log) << "Process fail" << p->errorString();
                break;
            }
        }
        
        p->close();
    } while(0);
    
    if(nRet)
        emit sigError(nRet, p->errorString());
    
    emit sigDisconnect();
    p->deleteLater();
    qDebug(log) << "CSSHTunnelThread end";
}
