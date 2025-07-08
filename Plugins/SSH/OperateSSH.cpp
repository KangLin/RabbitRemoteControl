// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>

#include "OperateSSH.h"
#include "DlgSettingsSSH.h"
#include "BackendSSH.h"
#include "Plugin.h"

#undef SetPort
static Q_LOGGING_CATEGORY(log, "Plugin.SSH.Operate")

COperateSSH::COperateSSH(CPlugin *parent)
    : COperateTerminal(parent)
{
    qDebug(log) << Q_FUNC_INFO;
    auto &net = m_Parameters.m_SSH.m_Net;
    net.SetPort(22);
    QList<CParameterUser::TYPE> lstType;
    lstType << CParameterUser::TYPE::UserPassword
            << CParameterUser::TYPE::PublicKey;
    net.m_User.SetType(lstType);
    bool check = connect(this, SIGNAL(sigReceiveData(const QByteArray&)),
                         this, SLOT(slotReceiveData(const QByteArray&)));
    Q_ASSERT(check);
}

COperateSSH::~COperateSSH()
{
    qDebug(log) << Q_FUNC_INFO;
}

int COperateSSH::Initial()
{
    int nRet = 0;
    nRet = COperateTerminal::Initial();
    if(nRet) return nRet;
    return SetParameter(&m_Parameters);
}

int COperateSSH::Clean()
{
    return 0;
}

QDialog *COperateSSH::OnOpenDialogSettings(QWidget *parent)
{
    return new CDlgSettingsSSH(&m_Parameters, parent);
}

CBackend *COperateSSH::InstanceBackend()
{
    return new CBackendSSH(this);
}

COperateSSH::SecurityLevel COperateSSH::GetSecurityLevel()
{
    return SecurityLevel::Secure;
}

void COperateSSH::slotReceiveData(const QByteArray &data)
{
    //qDebug(log) << Q_FUNC_INFO << data.length();
    WriteTerminal(data.data(), data.length());
}

const QString COperateSSH::Id()
{
    QString szId = COperateTerminal::Id();
        auto &sshNet = m_Parameters.m_SSH.m_Net;
        if(!sshNet.GetHost().isEmpty())
            szId += "_" + sshNet.GetHost()
                    + "_" + QString::number(sshNet.GetPort());
    
    static QRegularExpression exp("[-@:/#%!^&* \\.]");
    szId = szId.replace(exp, "_");
    return szId;
}

const QString COperateSSH::Name()
{
    QString szName;
        auto &sshNet = m_Parameters.m_SSH.m_Net;
        if(!sshNet.GetHost().isEmpty()) {
            if(m_Parameters.GetGlobalParameters()
                && m_Parameters.GetGlobalParameters()->GetShowProtocolPrefix())
                szName = Protocol() + ":";
            szName += sshNet.GetHost()
                      + ":" + QString::number(sshNet.GetPort());
        }
    if(szName.isEmpty())
        szName = COperateTerminal::Name();
    return szName;
}

const QString COperateSSH::Description()
{
    QString szDescription;
    if(!Name().isEmpty())
        szDescription = tr("Name: ") + Name() + "\n";
    
    if(!GetTypeName().isEmpty())
        szDescription += tr("Type: ") + GetTypeName() + "\n";
    
    if(!Protocol().isEmpty()) {
        szDescription += tr("Protocol: ") + Protocol();
#ifdef DEBUG
        if(!GetPlugin()->DisplayName().isEmpty())
            szDescription += " - " + GetPlugin()->DisplayName();
#endif
        szDescription += "\n";
    }

    auto &sshNet = m_Parameters.m_SSH.m_Net;
    if(!sshNet.GetHost().isEmpty())
        szDescription += tr("Server address: ") + sshNet.GetHost()
                         + ":" + QString::number(sshNet.GetPort()) + "\n";

    if(GetSecurityLevel() != SecurityLevel::No)
        szDescription += tr("Security level: ") + GetSecurityLevelString() + "\n";

    if(!GetPlugin()->Description().isEmpty())
        szDescription += tr("Description: ") + GetPlugin()->Description();
    
    return szDescription;
}

