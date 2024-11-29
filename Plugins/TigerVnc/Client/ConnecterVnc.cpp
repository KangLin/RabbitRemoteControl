// Author: Kang Lin <kl222@126.com>

#include "ConnecterVnc.h"
#include "DlgSettingsVnc.h"
#include "PluginClient.h"
#include "ConnectVnc.h"

#include <QMessageBox>
#include <QRegularExpression>
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "VNC.Connecter")

CConnecterVnc::CConnecterVnc(CPluginClient *plugin)
    : CConnecterThread(plugin)
{
    qDebug(log) << Q_FUNC_INFO;
}

CConnecterVnc::~CConnecterVnc()
{
    qDebug(log) << Q_FUNC_INFO;
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
                || CConnecterConnect::ServerName().isEmpty())
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
    return CConnecterConnect::ServerName();
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

int CConnecterVnc::Initial()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    nRet = CConnecterThread::Initial();
    if(nRet) return nRet;
    nRet = SetParameter(&m_Para);
    return nRet;
}

int CConnecterVnc::Clean()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    nRet = CConnecterThread::Clean();
    return nRet;
}
