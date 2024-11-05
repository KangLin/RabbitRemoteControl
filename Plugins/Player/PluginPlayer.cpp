// Author: Kang Lin <kl222@126.com>

#include "PluginPlayer.h"
#include "ConnecterPlayer.h"

CPluginPlayer::CPluginPlayer(QObject *parent)
    : CPluginClient{parent}
{}

const QString CPluginPlayer::Protocol() const
{
    return "Tool";
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
    return tr("Player: play media files. play or record camera, network media stream.");
}

const QString CPluginPlayer::Version() const
{
    return PLayer_VERSION;
}

CConnecter *CPluginPlayer::CreateConnecter(const QString &szId)
{
    CConnecter* p = nullptr;
    if(this->Id() == szId)
        p = new CConnecterPlayer(this);
    return p;
}

const QIcon CPluginPlayer::Icon() const
{
    return QIcon::fromTheme("multimedia-player");
}

const QString CPluginPlayer::Details() const
{
    QString szDetails;
    szDetails = "- " + tr("Qt version:") + "\n";
    szDetails += "  - " + tr("Compile version: ") + QT_VERSION_STR + "\n";
    szDetails += "  - " +  tr("Runtime version: ") + qVersion() + "\n";
#ifdef HAVE_QVideoWidget
    szDetails += "- " + tr("Use QVideoWidget");
#endif
    return szDetails;
}
