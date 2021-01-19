#include "ConnecterLibVnc.h"
#include "DlgSettingsLibVnc.h"
#include <QDebug>

CConnecterLibVnc::CConnecterLibVnc(CPluginFactory *parent) : CConnecterPlugins(parent)
{
    m_pParamter = &m_Para;
    m_Para.nPort = 5900;
    m_Para.bShared = true;
    m_Para.bLocalCursor = true;
    m_Para.bClipboard = true;
}

CConnecterLibVnc::~CConnecterLibVnc()
{
    qDebug() << "CConnecterLibVnc::~CConnecterLibVnc()";
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
      >> m_Para.bClipboard
      ;
    //TODO: if version
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
      << m_Para.bClipboard
      ;
    return nRet;
}

CConnect* CConnecterLibVnc::InstanceConnect()
{
    return new CConnectLibVnc(this);
}
