// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "ConnecterPlayer.h"
#include "ConnectPlayer.h"
#include "DlgPlayer.h"
#include "PluginClient.h"

static Q_LOGGING_CATEGORY(log, "Player.Connecter")

CConneterPlayer::CConneterPlayer(CPluginClient *plugin)
    : CConnecterThread(plugin)
    , m_pMenu(nullptr)
{
    SetParameter(&m_Parameters);
    m_pMenu = new QMenu(plugin->DisplayName());
    m_pMenu->setToolTip(plugin->DisplayName());
    m_pMenu->setStatusTip(plugin->DisplayName());
    m_pMenu->setIcon(Icon());
    m_pMenu->addAction(QIcon::fromTheme("media-playback-start"), tr("Start"),
                       this, SIGNAL(sigStart()));
    m_pMenu->addAction(QIcon::fromTheme("media-playback-stop"), tr("Stop"),
                       this, SIGNAL(sigStop()));
}

CConneterPlayer::~CConneterPlayer()
{
    qDebug(log) << "CConneterPlayer::~CConneterPlayer()";
    if(m_pMenu)
        delete m_pMenu;
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

QMenu *CConneterPlayer::GetMenu(QWidget *parent)
{
    qDebug(log) << "CConneterPlayer::GetMenu()";
    return m_pMenu;
}
