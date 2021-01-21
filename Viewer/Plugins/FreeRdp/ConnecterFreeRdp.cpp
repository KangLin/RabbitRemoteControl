//! @author: Kang Lin(kl222@126.com)

#include "ConnecterFreeRdp.h"
#include "ConnectFreeRdp.h"
#include <QDebug>
#include "DlgSetFreeRdp.h"

CConnecterFreeRdp::CConnecterFreeRdp(CPluginFactory *parent)
    : CConnecterPlugins(parent)
{
    m_pParamter = &m_ParamterFreeRdp;
    // 在 freerdp_client_context_free 中释放
    m_ParamterFreeRdp.pSettings = freerdp_settings_new(0);
    m_pParamter->nPort = 3389;
}

CConnecterFreeRdp::~CConnecterFreeRdp()
{
    qDebug() << "CConnecterFreeRdp::~CConnecterFreeRdp()";
}

qint16 CConnecterFreeRdp::Version()
{
    return 0;
}

QDialog *CConnecterFreeRdp::GetDialogSettings(QWidget *parent)
{
    return new CDlgSetFreeRdp(&m_ParamterFreeRdp, parent);
}

int CConnecterFreeRdp::OnLoad(QDataStream &d)
{
    int nRet = 0;
    qint16 version = 0;
    d >> version;
    QString val;
    d >> val;
    freerdp_settings_set_string(m_ParamterFreeRdp.pSettings, FreeRDP_Domain, val.toStdString().c_str());
    
    quint32 width, height, colorDepth;
    d >> width >> height >> colorDepth;
    m_ParamterFreeRdp.pSettings->DesktopWidth = width;
    m_ParamterFreeRdp.pSettings->DesktopHeight = height;
    m_ParamterFreeRdp.pSettings->ColorDepth = colorDepth;
    //TODO: if version
    return nRet;
}

int CConnecterFreeRdp::OnSave(QDataStream &d)
{
    int nRet = 0;
    
    d << Version()
      << QString(freerdp_settings_get_string(m_ParamterFreeRdp.pSettings, FreeRDP_Domain))
      
      << (quint32)m_ParamterFreeRdp.pSettings->DesktopWidth
      << (quint32)m_ParamterFreeRdp.pSettings->DesktopHeight
      << (quint32)m_ParamterFreeRdp.pSettings->ColorDepth
         ;
    
    return nRet;
}

CConnect *CConnecterFreeRdp::InstanceConnect()
{
    return new CConnectFreeRdp(this);
}
