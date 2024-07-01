// Author: Kang Lin <kl222@126.com>

#include "ConnecterTigerVnc.h"
#include <QDebug>
#include <QMessageBox>
#include <QRegularExpression>
#include "PluginClient.h"

static Q_LOGGING_CATEGORY(log, "VNC.Tiger.Connecter")

CConnecterTigerVnc::CConnecterTigerVnc(CPluginClient *parent)
    : CConnecterDesktop(parent)
{
    SetParameter(&m_Para);
}

CConnecterTigerVnc::~CConnecterTigerVnc()
{
    qDebug(log) << "CConnecterTigerVnc::~CConnecterTigerVnc()";
}

qint16 CConnecterTigerVnc::Version()
{
    return 0;
}

QString CConnecterTigerVnc::ServerName()
{
    if(GetParameter())
        if(!GetParameter()->GetShowServerName()
                || CConnecter::ServerName().isEmpty())
    {
        if(m_Para.GetIce())
        {
            if(!m_Para.GetPeerUser().isEmpty())
                return m_Para.GetPeerUser();
        }
        else {
            if(!GetParameter()->m_Net.GetHost().isEmpty())
                return GetParameter()->m_Net.GetHost() + ":"
               + QString::number(GetParameter()->m_Net.GetPort());
        }
    }
    return CConnecter::ServerName();
}

QDialog *CConnecterTigerVnc::GetDialogSettings(QWidget *parent)
{
    CDlgSettingsTigerVnc* p = new CDlgSettingsTigerVnc(&m_Para, parent);
    return p;
}

CConnect* CConnecterTigerVnc::InstanceConnect()
{
    CConnectTigerVnc* p = new CConnectTigerVnc(this);
    return p;
}

const QString CConnecterTigerVnc::Id()
{
    if(m_Para.GetIce())
    {
        QString szId = Protocol() + "_" + GetPlugClient()->Name();
        if(GetParameter())
        {
            if(!m_Para.GetPeerUser().isEmpty())
                szId += + "_" + m_Para.GetPeerUser();
        }
        szId = szId.replace(QRegularExpression("[@:/#%!^&*\\.]"), "_");
        return szId;
    }
    return CConnecter::Id();
}
