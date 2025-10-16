// Author: Kang Lin <kl222@126.com>

#include "OperateSSH.h"
#include "PluginLibSSH.h"
#ifdef HAVE_LIBSSH
    #include "ChannelSSH.h"
#endif
#include <QLoggingCategory>
static Q_LOGGING_CATEGORY(log, "Plugin.SSH")

CPluginLibSSH::CPluginLibSSH(QObject *parent) : CPlugin(parent)
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
              "manage public keys and much more ...") + "\n"
              + tr("It uses libssh: https://www.libssh.org .");
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

const QString CPluginLibSSH::Details() const
{
    QString szDetails;
    szDetails = COperateTerminal::Details();
#ifdef HAVE_LIBSSH
    CChannelSSH channel(nullptr, nullptr);
    szDetails += channel.GetDetails();
#endif

    return szDetails;
}

COperate *CPluginLibSSH::OnCreateOperate(const QString &szId)
{
    if(Id() == szId)
        return new COperateSSH(this);
    return nullptr;
}
