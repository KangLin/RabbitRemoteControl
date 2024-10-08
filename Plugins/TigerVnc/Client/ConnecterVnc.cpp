// Author: Kang Lin <kl222@126.com>

#include "ConnecterVnc.h"
#include "DlgSettingsVnc.h"
#include "PluginClient.h"

#include <QMessageBox>
#include <QRegularExpression>
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "VNC.Connecter")

CConnecterVnc::CConnecterVnc(CPluginClient *plugin)
    : CConnecterThread(plugin)
{
    SetParameter(&m_Para);
}

CConnecterVnc::~CConnecterVnc()
{
    qDebug(log) << "CConnecterVnc::~CConnecterVnc()";
}

const QString CConnecterVnc::Id()
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

qint16 CConnecterVnc::Version()
{
    return 0;
}

QString CConnecterVnc::ServerName()
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

QDialog *CConnecterVnc::OnOpenDialogSettings(QWidget *parent)
{
    CDlgSettingsVnc* p = new CDlgSettingsVnc(&m_Para, parent);
    return p;
}

CConnect *CConnecterVnc::InstanceConnect()
{
    CConnectVnc* p = new CConnectVnc(this);
    return p;
}
