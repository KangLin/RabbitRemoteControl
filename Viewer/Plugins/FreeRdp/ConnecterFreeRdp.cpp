//! @author: Kang Lin(kl222@126.com)

#include "ConnecterFreeRdp.h"
#include <QDebug>
#include "DlgSettings.h"

CConnecterFreeRdp::CConnecterFreeRdp(QObject *parent) : CConnecter(parent),
    m_pThread(nullptr)
{}

CConnecterFreeRdp::~CConnecterFreeRdp()
{
    qDebug() << "CConnecterFreeRdp::~CConnecterFreeRdp()";
    if(m_pThread)
    {
        m_pThread->wait();
        delete m_pThread;
    }
}

QString CConnecterFreeRdp::Name()
{
    //TODO: set server name
    QString szName;
    szName.replace(":", "_");
    return szName;
}

QString CConnecterFreeRdp::Description()
{
    return Protol() + ":";
}

QString CConnecterFreeRdp::Protol()
{
    return "RDP";
}

QDialog *CConnecterFreeRdp::GetDialogSettings(QWidget *parent)
{
    return new CDlgSettings(parent);
}

int CConnecterFreeRdp::Load(QDataStream &d)
{
    int nRet = 0;
    
    return nRet;
}

int CConnecterFreeRdp::Save(QDataStream &d)
{
    int nRet = 0;
    
    return nRet;
}

int CConnecterFreeRdp::Connect()
{
    int nRet = 0;
    if(nullptr == m_pThread)
    {
        m_pThread = new CConnectThread(GetViewer(), this);
    } else {
        m_pThread->quit();
    }

    m_pThread->start();
    return nRet;
}

int CConnecterFreeRdp::DisConnect()
{
    int nRet = 0;
    if(!m_pThread) return -1;
    
    m_pThread->m_bExit = true;
    // Actively disconnect, without waiting for the thread to exit and then disconnect
    emit sigDisconnected();
    return nRet;
}
