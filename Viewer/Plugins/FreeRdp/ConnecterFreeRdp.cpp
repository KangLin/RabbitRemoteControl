//! @author: Kang Lin(kl222@126.com)

#include "ConnecterFreeRdp.h"
#include "ConnectFreeRdp.h"
#include <QDebug>
#include "DlgSetFreeRdp.h"

CConnecterFreeRdp::CConnecterFreeRdp(CPluginFactory *parent)
    : CConnecterPlugins(parent)
{
    // 在 freerdp_client_context_free 中释放
    m_pSettings = freerdp_settings_new(0);
}

CConnecterFreeRdp::~CConnecterFreeRdp()
{
    qDebug() << "CConnecterFreeRdp::~CConnecterFreeRdp()";
}

QString CConnecterFreeRdp::GetServerName()
{
    if(m_szServerName.isEmpty())
    {
        return m_pSettings->ServerHostname + QString(":")
                + QString::number(m_pSettings->ServerPort);
    }
    return m_szServerName;
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
    //TODO: if version
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

CConnect *CConnecterFreeRdp::InstanceConnect()
{
    return new CConnectFreeRdp(this);
}
