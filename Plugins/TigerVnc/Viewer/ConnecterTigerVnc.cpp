// Author: Kang Lin <kl222@126.com>

#include "ConnecterTigerVnc.h"
#include "rfb/encodings.h"
#include <QDebug>
#include "RabbitCommonLog.h"

CConnecterTigerVnc::CConnecterTigerVnc(CPluginViewer *parent)
    : CConnecterDesktop(parent),
      m_pConnect(nullptr)
{
    m_pParameter = &m_Para;
    m_Para.nPort = 5900;
    m_Para.bShared = true;
    m_Para.bBufferEndRefresh = false;
    m_Para.bSupportsDesktopResize = true;
    
    m_Para.bAutoSelect = true;
    m_Para.nColorLevel = CConnectTigerVnc::Full;
    m_Para.nEncoding = rfb::encodingTight;
    m_Para.bCompressLevel = true;
    m_Para.nCompressLevel = 2;
    m_Para.bNoJpeg = false;
    m_Para.nQualityLevel = 8;
    
    m_Para.bIce = false;
    m_Para.nSignalPort = 5222;
    m_Para.nStunPort = m_Para.nTurnPort = 3748;
}

CConnecterTigerVnc::~CConnecterTigerVnc()
{
    qDebug() << "CConnecterTigerVnc::~CConnecterTigerVnc()";
    if(m_pConnect)
    {
        m_pConnect->deleteLater();
        m_pConnect = nullptr;
    }
}

qint16 CConnecterTigerVnc::Version()
{
    return 0;
}

QString CConnecterTigerVnc::ServerName()
{
    if(CConnecter::ServerName().isEmpty())
    {
        if(m_Para.bIce)
        {
            if(!m_Para.szSignalUser.isEmpty())
                return m_Para.szSignalUser;
        }
        else {
            if(!m_pParameter->szHost.isEmpty())
                return m_pParameter->szHost + ":"
               + QString::number(m_pParameter->nPort);
        }
        return CConnecter::Name();
    }
    return CConnecter::ServerName();
}

QDialog *CConnecterTigerVnc::GetDialogSettings(QWidget *parent)
{
    Q_UNUSED(parent)
    CDlgSettingsTigerVnc* p = new CDlgSettingsTigerVnc(&m_Para);
    return p;
}

int CConnecterTigerVnc::OnSave(QDataStream & d)
{
    d << Version()
      << m_Para.bShared
      << m_Para.bBufferEndRefresh
      << m_Para.bSupportsDesktopResize
      << m_Para.bAutoSelect
      << static_cast<int>(m_Para.nColorLevel)
      << m_Para.nEncoding
      << m_Para.bCompressLevel
      << m_Para.nCompressLevel
      << m_Para.bNoJpeg
      << m_Para.nQualityLevel
      << m_Para.bIce
      << m_Para.szSignalServer
      << m_Para.nSignalPort
      << m_Para.szSignalUser
      << m_Para.szSignalPassword
      << m_Para.szStunServer
      << m_Para.nStunPort
      << m_Para.szTurnServer
      << m_Para.nTurnPort
      << m_Para.szTurnUser
      << m_Para.szTurnPassword
         ;
    return 0;
}

int CConnecterTigerVnc::OnLoad(QDataStream &d)
{
    int nColorLevel = CConnectTigerVnc::Full;
    qint16 version = 0;
    d >> version;
    d >> m_Para.bShared
            >> m_Para.bBufferEndRefresh
            >> m_Para.bSupportsDesktopResize
            >> m_Para.bAutoSelect
            >> nColorLevel
            >> m_Para.nEncoding
            >> m_Para.bCompressLevel
            >> m_Para.nCompressLevel
            >> m_Para.bNoJpeg
            >> m_Para.nQualityLevel
            >> m_Para.bIce
            >> m_Para.szSignalServer
            >> m_Para.nSignalPort
            >> m_Para.szSignalUser
            >> m_Para.szSignalPassword
            >> m_Para.szStunServer
            >> m_Para.nStunPort
            >> m_Para.szTurnServer
            >> m_Para.nTurnPort
            >> m_Para.szTurnUser
            >> m_Para.szTurnPassword
            ;
    //TODO: if version
    m_Para.nColorLevel = static_cast<CConnectTigerVnc::COLOR_LEVEL>(nColorLevel);
    return 0;
}

CConnect* CConnecterTigerVnc::InstanceConnect()
{
    return new CConnectTigerVnc(this);
}

int CConnecterTigerVnc::Connect()
{
    emit sigConnect(this);
    return 0;
}

int CConnecterTigerVnc::DisConnect()
{
    emit sigDisconnect(this);
    return 0;
}
