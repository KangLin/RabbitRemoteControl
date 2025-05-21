// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QThread>

#include "ChannelSSHTunnelForward.h"
#include "SSHTunnelThread.h"

static Q_LOGGING_CATEGORY(log, "Channel.SSH.Tunnel.Thread")

CSSHTunnelThread::CSSHTunnelThread(CParameterSSHTunnel *parameter, CParameterNet *remote, CConnect *pConnect)
    : QThread(nullptr),
    m_pConnect(pConnect),
    m_bExit(false),
    m_pParameter(parameter),
    m_pRemoteNet(remote)
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
    CChannelSSHTunnelForward* p = new CChannelSSHTunnelForward(
        m_pParameter, m_pRemoteNet, m_pConnect);
    if(!p)
        return;
    
    do{
        bool check = connect(p, SIGNAL(sigServer(QString, quint16)),
                             this, SIGNAL(sigServer(QString, quint16)));
        Q_ASSERT(check);
        check = connect(p, SIGNAL(sigServer(QString)),
                             this, SIGNAL(sigServer(QString)));
        Q_ASSERT(check);
        check = connect(p, SIGNAL(sigError(int,QString)), this,
                        SIGNAL(sigError(int,QString)));
        Q_ASSERT(check);

        if(!p->open(QIODevice::ReadWrite)) {
            emit m_pConnect->sigShowMessageBox(
                tr("Error"), p->errorString(), QMessageBox::Critical);
            nRet = -1;
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
