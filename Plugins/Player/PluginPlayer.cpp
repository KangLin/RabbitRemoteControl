// Author: Kang Lin <kl222@126.com>

#include "PluginPlayer.h"
#include "OperatePlayer.h"

CPluginPlayer::CPluginPlayer(QObject *parent)
    : CPlugin{parent}
{}

const QString CPluginPlayer::Protocol() const
{
    return QString();
}

const CPlugin::TYPE CPluginPlayer::Type() const
{
    return TYPE::Tools;
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
    QString szText = tr("Player: play media files or camera.");
    #ifdef HAVE_QT6_RECORD
    szText += tr(" Record camera or media stream.");
    #endif
    return szText;
}

const QString CPluginPlayer::Version() const
{
    return PLayer_VERSION;
}

COperate *CPluginPlayer::OnCreateOperate(const QString &szId)
{
    COperate* p = nullptr;
    if(this->Id() == szId)
        p = new COperatePlayer(this);
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
