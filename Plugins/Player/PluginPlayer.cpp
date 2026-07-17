// Author: Kang Lin <kl222@126.com>

#include "OperatePlayer.h"
#include <QLoggingCategory>
#include "RabbitCommonTools.h"
#include "PluginPlayer.h"

static Q_LOGGING_CATEGORY(log, "Player.Plugin")
CPluginPlayer::CPluginPlayer(QObject *parent)
    : CPlugin{parent}
{
    qDebug(log) << Q_FUNC_INFO;
    QStringList lstPermission;
    lstPermission << "android.permission.CAMERA";
    RabbitCommon::CTools::AndroidRequestPermission(lstPermission);
}

CPluginPlayer::~CPluginPlayer()
{
    qDebug(log) << Q_FUNC_INFO;
}

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
    return tr("Player") + tr("(Experimental)");
}

const QString CPluginPlayer::Description() const
{
    QString szText = tr("Player: play media files or camera.");
    #ifdef HAVE_QT6_RECORD
    szText += tr(" Record camera or media stream.");
    #endif
    szText += tr("(Experimental)");
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
    szDetails += "- " + tr("Dependency libraries") + "\n";
    szDetails += "  - " + tr("Qt version:") + "\n";
    szDetails += "    - " + tr("Compile version: ") + QT_VERSION_STR + "\n";
    szDetails += "    - " +  tr("Runtime version: ") + qVersion() + "\n";
#ifdef HAVE_QVideoWidget
    szDetails += "  - " + tr("Use QVideoWidget");
#endif
    return szDetails;
}
