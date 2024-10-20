// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "ConnecterPlayer.h"
#include "ConnectPlayer.h"
#include "DlgPlayer.h"

static Q_LOGGING_CATEGORY(log, "Player.Connecter")
CConneterPlayer::CConneterPlayer(CPluginClient *plugin) : CConnecterThread(plugin)
{
    SetParameter(&m_Parameters);
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
