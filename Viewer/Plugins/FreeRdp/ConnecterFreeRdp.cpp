//! @author: Kang Lin(kl222@126.com)

#include "ConnecterFreeRdp.h"
#include <QDebug>
#include "DlgSetFreeRdp.h"

CConnecterFreeRdp::CConnecterFreeRdp(QObject *parent) : CConnecter(parent),
    m_pThread(nullptr)
{
    // 在 freerdp_client_context_free 中释放
    m_pSettings = freerdp_settings_new(0);
}

CConnecterFreeRdp::~CConnecterFreeRdp()
{
    qDebug() << "CConnecterFreeRdp::~CConnecterFreeRdp()";
    if(m_pThread)
    {
        m_pThread->wait();
        delete m_pThread;
    }
}

QString CConnecterFreeRdp::ServerName()
{
    QString szName = m_pSettings->ServerHostname;
    szName += ":";
    szName += QString::number(m_pSettings->ServerPort);
    return szName;
}

QString CConnecterFreeRdp::Name()
{
    return "FreeRdp";
}

QString CConnecterFreeRdp::Description()
{
    return Protol() + ":";
}

QString CConnecterFreeRdp::Protol()
{
    return "RDP";
}

qint16 CConnecterFreeRdp::Version()
{
    return 0;
}

QDialog *CConnecterFreeRdp::GetDialogSettings(QWidget *parent)
{
    return new CDlgSetFreeRdp(m_pSettings, parent);
}

int CConnecterFreeRdp::Load(QDataStream &d)
{
    int nRet = 0;
    qint16 version = 0;
    d >> version;
    QString val;
    d >> val;
    freerdp_settings_set_string(m_pSettings, FreeRDP_Domain, val.toStdString().c_str());
    d >> val;
    freerdp_settings_set_string(m_pSettings, FreeRDP_ServerHostname, val.toStdString().c_str());
    quint32 nPort = 3389;
    d >> nPort;
    m_pSettings->ServerPort = nPort;
    d >> val;
    freerdp_settings_set_string(m_pSettings, FreeRDP_Username, val.toStdString().c_str());
    d >> val;
    freerdp_settings_set_string(m_pSettings, FreeRDP_Password, val.toStdString().c_str());
    
    quint32 width, height, colorDepth;
    d >> width >> height >> colorDepth;
    m_pSettings->DesktopWidth = width;
    m_pSettings->DesktopHeight = height;
    m_pSettings->ColorDepth = colorDepth;
    
    return nRet;
}

int CConnecterFreeRdp::Save(QDataStream &d)
{
    int nRet = 0;
    
    d << Version()
      << QString(freerdp_settings_get_string(m_pSettings, FreeRDP_Domain))
      << QString(freerdp_settings_get_string(m_pSettings, FreeRDP_ServerHostname))
      << (quint32)m_pSettings->ServerPort
      << QString(freerdp_settings_get_string(m_pSettings, FreeRDP_Username))
      << QString(freerdp_settings_get_string(m_pSettings, FreeRDP_Password))
         
      << (quint32)m_pSettings->DesktopWidth
      << (quint32)m_pSettings->DesktopHeight
      << (quint32)m_pSettings->ColorDepth
         ;
    
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
