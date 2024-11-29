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
    qDebug(log) << Q_FUNC_INFO;
}

CConnecterPlayer::~CConnecterPlayer()
{
    qDebug(log) << Q_FUNC_INFO;
}

qint16 CConnecterPlayer::Version()
{
    return 0;
}

int CConnecterPlayer::Initial()
{
    qDebug(log) << Q_FUNC_INFO;
    bool check = false;
    int nRet = 0;
    nRet = CConnecterThread::Initial();
    if(nRet) return nRet;

    nRet = SetParameter(&m_Parameters);
    if(nRet) return nRet;

    m_Menu.clear();
#ifdef HAVE_QVideoWidget
    m_Menu.addAction(m_Player.m_paStart);
    m_Menu.addAction(m_Player.m_paPause);
#if HAVE_QT6_RECORD
    m_Menu.addAction(m_Player.m_paRecord);
    m_Menu.addAction(m_Player.m_paRecordPause);
#endif
    m_Menu.addAction(m_Player.m_paScreenShot);
    m_Menu.addSeparator();
    m_Menu.addAction(m_Player.m_paSettings);
    check = connect(this, &CConnecterPlayer::sigConnected,
                    m_Player.m_paStart, &QAction::toggle);
    Q_ASSERT(check);
    check = connect(m_Player.m_paStart, SIGNAL(toggled(bool)),
                    this, SIGNAL(sigStart(bool)));
    Q_ASSERT(check);
    check = connect(m_Player.m_paPause, SIGNAL(toggled(bool)),
                    this, SIGNAL(sigPause(bool)));
    Q_ASSERT(check);
#if HAVE_QT6_RECORD
    check = connect(m_Player.m_paRecord, SIGNAL(toggled(bool)),
                    m_pRecord ,SIGNAL(toggled(bool)));
    Q_ASSERT(check);
    check = connect(m_Player.m_paRecordPause, SIGNAL(toggled(bool)),
                    m_pRecordPause, SIGNAL(toggled(bool)));
    Q_ASSERT(check);
#endif
    check = connect(m_Player.m_paScreenShot, &QAction::triggered,
                    m_pScreenShot, &QAction::triggered);
    Q_ASSERT(check);
    check = connect(m_Player.m_paSettings, &QAction::triggered,
                    m_pSettings, &QAction::trigger);
    Q_ASSERT(check);
    check = connect(this, &CConnecterPlayer::sigConnected,
                    this, [&](){
                        m_Player.SetParameter(&m_Parameters);
                    });
    Q_ASSERT(check);
    check = connect(&m_Player, SIGNAL(sigChangePosition(qint64)),
                    this, SIGNAL(sigChangePosition(qint64)));
    Q_ASSERT(check);
#else
    QAction* pStart = m_Menu.addAction(
        QIcon::fromTheme("media-playback-start"), tr("Start"));
    pStart->setCheckable(true);
    check = connect(this, &CConnecterPlayer::sigConnected,
                    pStart, &QAction::toggle);
    Q_ASSERT(check);
    check = connect(pStart, &QAction::toggled,
                    this, [&](bool checked){
                        QAction* p = qobject_cast<QAction*>(sender());
                        if(!p) return;
                        if(p->isChecked()) {
                            p->setIcon(QIcon::fromTheme("media-playback-stop"));
                            p->setText(tr("Stop"));
                            m_pPause->setEnabled(true);
                            m_pPause->setChecked(false);
                            m_pScreenShot->setEnabled(true);
#if HAVE_QT6_RECORD
                            m_pRecord->setEnabled(true);
                            m_pRecord->setChecked(false);
                            m_pRecordPause->setEnabled(true);
                            m_pRecordPause->setChecked(false);
#endif
                        } else {
                            p->setIcon(QIcon::fromTheme("media-playback-start"));
                            p->setText(tr("Start"));
                            m_pPause->setEnabled(false);
                            m_pPause->setChecked(false);
                            m_pScreenShot->setEnabled(false);
#if HAVE_QT6_RECORD
                            m_pRecord->setEnabled(false);
                            m_pRecord->setChecked(false);
                            m_pRecordPause->setEnabled(false);
                            m_pRecordPause->setChecked(false);
#endif
                        }
                        emit sigStart(checked);
                    });
    Q_ASSERT(check);
    m_pPause = m_Menu.addAction(
        QIcon::fromTheme("media-playback-pause"), tr("pause"));
    m_pPause->setCheckable(true);
    check = connect(m_pPause, SIGNAL(toggled(bool)),
                    this, SIGNAL(sigPause(bool)));
    Q_ASSERT(check);
#if HAVE_QT6_RECORD
    m_Menu.addAction(m_pRecord);
    m_Menu.addAction(m_pRecordPause);
#endif
    m_Menu.addAction(m_pScreenShot);
    m_Menu.addSeparator();
    m_Menu.addAction(m_pSettings);
#endif
    return nRet;
}

int CConnecterPlayer::Clean()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    nRet = CConnecterThread::Clean();
    return nRet;
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

void CConnecterPlayer::slotScreenShot()
{
    emit sigScreenShot();
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

void CConnecterPlayer::slotPositionChanged(qint64 pos, qint64 duration)
{
    m_Player.slotPositionChanged(pos, duration);
}

void CConnecterPlayer::slotPlaybackStateChanged(QMediaPlayer::PlaybackState state)
{
    if(QMediaPlayer::StoppedState == state
        && m_Player.m_paStart->isChecked())
        m_Player.m_paStart->setChecked(false);
}

#if HAVE_QT6_RECORD
void CConnecterPlayer::slotRecordStateChanged(QMediaRecorder::RecorderState state)
{

    if(QMediaRecorder::StoppedState == state
        && m_Player.m_paRecord->isCheckable())
        m_Player.m_paRecord->setChecked(false);
}
#endif

#endif
