// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "ConnecterPlayer.h"
#include "ConnectPlayer.h"
#include "DlgPlayer.h"

static Q_LOGGING_CATEGORY(log, "Player.Connecter")

class CMyMenu: public QMenu
{
public:
    CMyMenu(const QString &title, QWidget *parent = nullptr) : QMenu(title, parent){}
    ~CMyMenu()
    {
        qDebug(log) << "CMyMenu::~CMyMenu()";
    }
};

CConneterPlayer::CConneterPlayer(CPluginClient *plugin) : CConnecterThread(plugin)
{
    SetParameter(&m_Parameters);
    m_pMenu = new CMyMenu(tr("Player"));
    m_pMenu->setIcon(QIcon::fromTheme("media-player"));
    m_pMenu->addAction(QIcon::fromTheme("media-playback-start"), tr("Start"),
                       this, SIGNAL(sigStart()));
    m_pMenu->addAction(QIcon::fromTheme("media-playback-stop"), tr("Stop"),
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

QMenu *CConneterPlayer::GetMenu(QWidget *parent)
{
    qDebug(log) << "CConneterPlayer::GetMenu()";
    return m_pMenu;
}
