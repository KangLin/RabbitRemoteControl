// Author: Kang Lin <kl222@126.com>

#include "ConnecterRabbitVNC.h"
#include <QDebug>
#include <QMessageBox>
#include <QRegularExpression>
#include <QLoggingCategory>

#include "DlgGetPasswordRabbitVNC.h"
#include "PluginClient.h"

Q_DECLARE_LOGGING_CATEGORY(RabbitVNC)

CConnecterRabbitVNC::CConnecterRabbitVNC(CPluginClient *parent)
    : CConnecterDesktop(parent)
{
    SetParameter(&m_Para);
}

CConnecterRabbitVNC::~CConnecterRabbitVNC()
{
    qDebug(RabbitVNC) << "CConnecterRabbitVnc::~CConnecterRabbitVnc()";
}

qint16 CConnecterRabbitVNC::Version()
{
    return 0;
}

QString CConnecterRabbitVNC::ServerName()
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
            if(!GetParameter()->GetHost().isEmpty())
                return GetParameter()->GetHost() + ":"
               + QString::number(GetParameter()->GetPort());
        }
    }
    return CConnecter::ServerName();
}

QDialog *CConnecterRabbitVNC::GetDialogSettings(QWidget *parent)
{
    CDlgSettingsRabbitVNC* p = new CDlgSettingsRabbitVNC(&m_Para, parent);
    return p;
}

CConnect* CConnecterRabbitVNC::InstanceConnect()
{
    return new CConnectRabbitVNC(this);
}

const QString CConnecterRabbitVNC::Id()
{
    if(m_Para.GetIce())
    {
        QString szId = Protocol() + "_" + m_pPluginClient->Name();
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
