#include "PluginLibSSH.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonLog.h"
#include "ConnecterSSH.h"

#include <QDebug>

CPluginLibSSH::CPluginLibSSH()
{
}

CPluginLibSSH::~CPluginLibSSH()
{
    qDebug() << "CPluginFactoryLibSSH::~CPluginFactoryLibSSH()";
}

const QString CPluginLibSSH::Protol() const
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
    return QIcon(":/image/SSH");
}

CConnecter *CPluginLibSSH::CreateConnecter(const QString &szProtol)
{
    if(Id() == szProtol)
        return new CConnecterSSH(this);
    return nullptr;
}
