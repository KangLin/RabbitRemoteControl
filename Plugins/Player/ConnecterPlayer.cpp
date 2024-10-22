// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "ConnecterPlayer.h"
#include "ConnectPlayer.h"
#include "DlgPlayer.h"
#include "PluginClient.h"
#include "RabbitCommonTools.h"

static Q_LOGGING_CATEGORY(log, "Player.Connecter")

CConneterPlayer::CConneterPlayer(CPluginClient *plugin)
    : CConnecterThread(plugin)
{
    SetParameter(&m_Parameters);
    m_Menu.clear();
    m_Menu.addAction(QIcon::fromTheme("media-playback-start"), tr("Start"),
                       this, SIGNAL(sigStart()));
    m_Menu.addAction(QIcon::fromTheme("media-playback-stop"), tr("Stop"),
                       this, SIGNAL(sigStop()));
}

CConneterPlayer::~CConneterPlayer()
{
    qDebug(log) << "CConneterPlayer::~CConneterPlayer()";
}

qint16 CConneterPlayer::Version()
{
    return 0;
}

QDialog *CConneterPlayer::OnOpenDialogSettings(QWidget *parent)
{
    QDialog* pDlg = new CDlgPlayer(&m_Parameters, parent);
    return pDlg;
}

CConnect *CConneterPlayer::InstanceConnect()
{
    CConnect* p = new CConnectPlayer(this);
    return p;
}

#if HAVE_QVideoWidget
QWidget *CConneterPlayer::GetViewer()
{
    return &m_Video;
}

QVideoSink *CConneterPlayer::GetVideoSink()
{
    return m_Video.videoSink();
}
#endif
