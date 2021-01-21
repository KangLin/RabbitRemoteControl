#include "ConnecterLibVnc.h"
#include "DlgSettingsLibVnc.h"
#include <QDebug>

CConnecterLibVnc::CConnecterLibVnc(CPluginFactory *parent) : CConnecterPlugins(parent)
{
    m_pParamter = &m_Para;
    m_Para.nPort = 5900;
    m_Para.bShared = true;

    m_Para.bCompressLevel = true;
    m_Para.nCompressLevel = 3;
    m_Para.bJpeg = true;
    m_Para.nQualityLevel = 5;
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

int CConnecterLibVnc::OnLoad(QDataStream &d)
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

int CConnecterLibVnc::OnSave(QDataStream &d)
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

CConnect* CConnecterLibVnc::InstanceConnect()
{
    return new CConnectLibVnc(this);
}
