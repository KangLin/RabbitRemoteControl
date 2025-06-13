// Author: Kang Lin <kl222@126.com>

#include "OperateVnc.h"
#include "DlgSettingsVnc.h"
#include "Plugin.h"
#include "BackendVnc.h"

#include <QMessageBox>
#include <QRegularExpression>
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "VNC.Connecter")

COperateVnc::COperateVnc(CPlugin *plugin)
    : COperateDesktop(plugin)
{
    qDebug(log) << Q_FUNC_INFO;
}

COperateVnc::~COperateVnc()
{
    qDebug(log) << Q_FUNC_INFO;
}

const QString COperateVnc::Id()
{
    if(m_Para.GetIce())
    {
        QString szId = Protocol() + "_" + GetPlugin()->Name();
        if(GetParameter())
        {
            if(!m_Para.GetPeerUser().isEmpty())
                szId += + "_" + m_Para.GetPeerUser();
        }
        szId = szId.replace(QRegularExpression("[@:/#%!^&*\\.]"), "_");
        return szId;
    }
    return COperate::Id();
}

const qint16 COperateVnc::Version() const
{
    return 0;
}

QString COperateVnc::ServerName()
{
    if(GetParameter())
        if(!GetParameter()->GetShowServerName()
                || COperateDesktop::ServerName().isEmpty())
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
    return COperateDesktop::ServerName();
}

QDialog *COperateVnc::OnOpenDialogSettings(QWidget *parent)
{
    CDlgSettingsVnc* p = new CDlgSettingsVnc(&m_Para, parent);
    return p;
}

CBackend *COperateVnc::InstanceBackend()
{
    CBackendVnc* p = new CBackendVnc(this);
    return p;
}

int COperateVnc::Initial()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    nRet = COperateDesktop::Initial();
    if(nRet) return nRet;
    nRet = SetParameter(&m_Para);
    return nRet;
}

int COperateVnc::Clean()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    nRet = COperateDesktop::Clean();
    return nRet;
}
