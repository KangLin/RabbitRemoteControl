#include "ConnecterLibVNCServer.h"
#include "DlgSettingsLibVnc.h"
#include <QDebug>

CConnecterLibVNCServer::CConnecterLibVNCServer(CPluginFactory *parent) : CConnecterDesktop(parent)
{
    m_pParameter = &m_Para;
    m_Para.nPort = 5900;
    m_Para.bShared = true;
    
    m_Para.bClipboard = true;
    m_Para.bCompressLevel = true;
    m_Para.nCompressLevel = 3;
    m_Para.bJpeg = true;
    m_Para.nQualityLevel = 5;
}

CConnecterLibVNCServer::~CConnecterLibVNCServer()
{
    qDebug() << "CConnecterLibVNCServer::~CConnecterLibVNCServer()";
}

qint16 CConnecterLibVNCServer::Version()
{
    return 0;
}

QDialog *CConnecterLibVNCServer::GetDialogSettings(QWidget *parent)
{
    return new CDlgSettingsLibVnc(this, parent);
}

int CConnecterLibVNCServer::OnLoad(QDataStream &d)
{
    int nRet = 0;
    d >> m_Para.bShared
      >> m_Para.bCompressLevel
      >> m_Para.nCompressLevel
      >> m_Para.bJpeg
      >> m_Para.nQualityLevel
      ;
    //TODO: if version
    return nRet;
}

int CConnecterLibVNCServer::OnSave(QDataStream &d)
{
    int nRet = 0;
    d << m_Para.bShared
      << m_Para.bCompressLevel
      << m_Para.nCompressLevel
      << m_Para.bJpeg
      << m_Para.nQualityLevel
      ;
    return nRet;
}

CConnect* CConnecterLibVNCServer::InstanceConnect()
{
    return new CConnectLibVNCServer(this);
}
