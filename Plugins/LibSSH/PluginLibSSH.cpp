// Author: Kang Lin <kl222@126.com>

#include "OperateSSH.h"
#include "PluginLibSSH.h"

#include <QLoggingCategory>
static Q_LOGGING_CATEGORY(log, "Plugin.SSH")

CPluginLibSSH::CPluginLibSSH()
{
}

CPluginLibSSH::~CPluginLibSSH()
{
    qDebug(log) << Q_FUNC_INFO;
}

const QString CPluginLibSSH::Protocol() const
{
    return "SSH";
}

const QString CPluginLibSSH::Name() const
{
    return "TerminalSsh";
}

const QString CPluginLibSSH::DisplayName() const
{
    return tr("Secure Shell");
}

const QString CPluginLibSSH::Description() const
{
    return tr("Secure Shell(SSH): Used to establish secure remote connections over unsecured networks. "
              "It provides encryption and authentication mechanisms to ensure the confidentiality and integrity of the data. "
              "You can remotely execute programs, transfer files, "
              "use a secure and transparent tunnel, "
              "manage public keys and much more ...\n"
              "It is use libssh: https://www.libssh.org .");
}

const QIcon CPluginLibSSH::Icon() const
{
    return QIcon::fromTheme("ssh");
}

const CPlugin::TYPE CPluginLibSSH::Type() const
{
    return TYPE::Terminal;
}

const QString CPluginLibSSH::Version() const
{
    return 0;
}

COperate *CPluginLibSSH::OnCreateOperate(const QString &szId)
{
    if(Id() == szId)
        return new COperateSSH(this);
    return nullptr;
}
