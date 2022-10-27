#include "PluginLibSSH.h"
#include "RabbitCommonDir.h"
#include "ConnecterSSH.h"

#include <QLoggingCategory>
Q_LOGGING_CATEGORY(ssh, "SSH")

CPluginLibSSH::CPluginLibSSH()
{
}

CPluginLibSSH::~CPluginLibSSH()
{
    qDebug(ssh) << "CPluginFactoryLibSSH::~CPluginFactoryLibSSH()";
}

const QString CPluginLibSSH::Protocol() const
{
    return "SSH";
}

const QString CPluginLibSSH::Name() const
{
    return tr("LibSSH");
}

const QString CPluginLibSSH::Description() const
{
    return tr("SSH: you can remotely execute programs, transfer files, use a secure and transparent tunnel, manage public keys and much more ...");
}

const QIcon CPluginLibSSH::Icon() const
{
    return QIcon::fromTheme("ssh");
}

CConnecter *CPluginLibSSH::CreateConnecter(const QString &szProtocol)
{
    if(Id() == szProtocol)
        return new CConnecterSSH(this);
    return nullptr;
}
