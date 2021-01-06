#include "ConnecterLibVnc.h"
#include "DlgSettingsLibVnc.h"
#include <QDebug>

CConnecterLibVnc::CConnecterLibVnc(CPluginFactory *parent) : CConnecter(parent),
    m_pThread(nullptr)
{
    m_Para.nPort = 5900;
    m_Para.bShared = true;
    m_Para.bLocalCursor = false;
}

CConnecterLibVnc::~CConnecterLibVnc()
{
    qDebug() << "CConnecterLibVnc::~CConnecterLibVnc()";
    if(m_pThread)
    {
        m_pThread->wait();
        delete m_pThread;
    }
}

QString CConnecterLibVnc::ServerName()
{
    return m_Para.szHost + ":" + QString::number(m_Para.nPort);
}

QString CConnecterLibVnc::Name()
{
    return "LibVnc";
}

QString CConnecterLibVnc::Description()
{
    return Protol() + ":" + ServerName();
}

QString CConnecterLibVnc::Protol()
{
    return "RFB";
}

qint16 CConnecterLibVnc::Version()
{
    return 0;
}

QDialog *CConnecterLibVnc::GetDialogSettings(QWidget *parent)
{
    return new CDlgSettingsLibVnc(this, parent);
}

int CConnecterLibVnc::Load(QDataStream &d)
{
    int nRet = 0;
    qint16 version = 0;
    d >> version;
    d >> m_Para.szHost
      >> m_Para.nPort
      >> m_Para.szUser
      >> m_Para.szPassword
      >> m_Para.bShared
      >> m_Para.bLocalCursor
      ;
    return nRet;
}

int CConnecterLibVnc::Save(QDataStream &d)
{
    int nRet = 0;
    d << Version()
      << m_Para.szHost
      << m_Para.nPort
      << m_Para.szUser
      << m_Para.szPassword
      << m_Para.bShared
      << m_Para.bLocalCursor
      ;
    return nRet;
}

int CConnecterLibVnc::Connect()
{
    int nRet = 0;
    if(nullptr == m_pThread)
    {
        m_pThread = new CConnectThreadLibVnc(GetViewer(), this);
    }
    m_pThread->start();
    return nRet;
}

int CConnecterLibVnc::DisConnect()
{
    //qDebug() << "CConnecterLibVnc::DisConnect()";
    if(!m_pThread) return -1;
    
    m_pThread->m_bExit = true;
    // Actively disconnect, without waiting for the thread to exit and then disconnect
    emit sigDisconnected();
    return 0;
}
