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
    return "SSH";
}

const QString CPluginLibSSH::DisplayName() const
{
    return tr("Secure Shell(SSH)");
}

const QString CPluginLibSSH::Description() const
{
    return tr("Secure Shell(SSH): is a secure protocol and the most common way of safely administering remote servers. "
              "Using a number of encryption technologies, "
              "SSH provides a mechanism for establishing a cryptographically secured connection between two parties, "
              "authenticating each side to the other, and passing commands and output back and forth."
              "You can remotely execute programs, transfer files, "
              "use a secure and transparent tunnel, "
              "manage public keys and much more ...\n"
              "It uses libssh: https://www.libssh.org .");
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
