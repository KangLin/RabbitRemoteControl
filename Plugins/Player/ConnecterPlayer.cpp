// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "ConnecterPlayer.h"
#include "ConnectPlayer.h"
#include "DlgPlayer.h"
#include "PluginClient.h"

static Q_LOGGING_CATEGORY(log, "Player.Connecter")

CConnecterPlayer::CConnecterPlayer(CPluginClient *plugin)
    : CConnecterThread(plugin)
{
    bool check = false;
    SetParameter(&m_Parameters);

    m_Menu.clear();
    m_Menu.addAction(m_Player.m_paStart);
    m_Menu.addAction(m_Player.m_paPause);
    check = connect(m_Player.m_paStart, SIGNAL(toggled(bool)),
                     this, SIGNAL(sigStart(bool)));
    Q_ASSERT(check);
    check = connect(this, &CConnecterPlayer::sigPlaybackStateChanged,
                    this, [&](QMediaPlayer::PlaybackState state){
                        if(QMediaPlayer::StoppedState == state && m_Player.m_paStart->isChecked())
                            m_Player.m_paStart->setChecked(false);
                    });
    Q_ASSERT(check);
    check = connect(m_Player.m_paPause, SIGNAL(toggled(bool)),
                         this, SIGNAL(sigPause(bool)));
    Q_ASSERT(check);
    check = connect(m_Player.m_paRecord, SIGNAL(toggled(bool)),
                    this ,SIGNAL(sigRecord(bool)));
    Q_ASSERT(check);
    check = connect(this, &CConnecterPlayer::sigRecordStateChanged,
                    this, [&](QMediaRecorder::RecorderState state){
        if(QMediaRecorder::StoppedState == state && m_Player.m_paRecord->isCheckable())
            m_Player.m_paRecord->setChecked(false);
    });
    Q_ASSERT(check);
    check = connect(m_Player.m_paRecordPause, SIGNAL(toggled(bool)),
                    this, SIGNAL(sigRecordPause(bool)));
    Q_ASSERT(check);
    check = connect(this, &CConnecterPlayer::sigConnected,
                    this, [&](){
        m_Player.SetParameter(&m_Parameters);
    });
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigPositionChanged(qint64, qint64)),
                    &m_Player, SLOT(slotPositionChanged(qint64, qint64)));
    Q_ASSERT(check);
    check = connect(&m_Player, SIGNAL(sigChangePosition(qint64)),
                    this, SIGNAL(sigChangePosition(qint64)));
    Q_ASSERT(check);

}

CConnecterPlayer::~CConnecterPlayer()
{
    qDebug(log) << "CConneterPlayer::~CConneterPlayer()";
}

qint16 CConnecterPlayer::Version()
{
    return 0;
}

QDialog *CConnecterPlayer::OnOpenDialogSettings(QWidget *parent)
{
    QDialog* pDlg = new CDlgPlayer(&m_Parameters, parent);
    return pDlg;
}

CConnect *CConnecterPlayer::InstanceConnect()
{
    CConnect* p = new CConnectPlayer(this);
    return p;
}

#if HAVE_QVideoWidget
QWidget *CConnecterPlayer::GetViewer()
{
    return &m_Player;
}

QVideoSink *CConnecterPlayer::GetVideoSink()
{
    return m_Player.videoSink();
}
#endif
