#include "ConnecterLibVnc.h"
#include "DlgSettingsLibVnc.h"
#include <QDebug>

CConnecterLibVnc::CConnecterLibVnc(CPluginFactory *parent) : CConnecterPlugins(parent)
{
    m_Para.nPort = 5900;
    m_Para.bShared = true;
    m_Para.bLocalCursor = false;
}

CConnecterLibVnc::~CConnecterLibVnc()
{
    qDebug() << "CConnecterLibVnc::~CConnecterLibVnc()";
}

QString CConnecterLibVnc::GetServerName()
{
    if(m_szServerName.isEmpty())
    {
        return m_Para.szHost + ":" + QString::number(m_Para.nPort);
    }
    return m_szServerName;
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
      ;
    return nRet;
}

CConnect* CConnecterLibVnc::InstanceConnect()
{
    return new CConnectLibVnc(this);
}
