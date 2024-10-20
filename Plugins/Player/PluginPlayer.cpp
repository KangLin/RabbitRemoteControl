// Author: Kang Lin <kl222@126.com>

#include "PluginPlayer.h"
#include "ConnecterPlayer.h"

CPluginPlayer::CPluginPlayer(QObject *parent)
    : CPluginClient{parent}
{}

const QString CPluginPlayer::Protocol() const
{
    return "Player";
}

const QString CPluginPlayer::Name() const
{
    return "Player";
}

const QString CPluginPlayer::DisplayName() const
{
    return tr("Player");
}

const QString CPluginPlayer::Description() const
{
    return tr("Player");
}

const QString CPluginPlayer::Version() const
{
    return PluginPLayer_VERSION;
}

CConnecter *CPluginPlayer::CreateConnecter(const QString &szId)
{
    CConnecter* p = nullptr;
    if(this->Id() == szId)
        p = new CConneterPlayer(this);
    return p;
}

const QIcon CPluginPlayer::Icon() const
{
    return QIcon::fromTheme("multimedia-player");
}

const QString CPluginPlayer::Details() const
{
    QString szDetails;

    return szDetails;
}
