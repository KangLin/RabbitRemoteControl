// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "OperatePlayer.h"
#include "BackendPlayer.h"
#include "DlgPlayer.h"

static Q_LOGGING_CATEGORY(log, "Player.Operate")

COperatePlayer::COperatePlayer(CPlugin *plugin)
    : COperateDesktop(plugin)
{
    qDebug(log) << Q_FUNC_INFO;
}

COperatePlayer::~COperatePlayer()
{
    qDebug(log) << Q_FUNC_INFO;
}

const qint16 COperatePlayer::Version() const
{
    return 0;
}

int COperatePlayer::Initial()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;

    nRet = SetParameter(&m_Parameters);
    if(nRet) return nRet;

    bool check = false;
#if HAVE_QVideoWidget
    check = connect(&m_Player, SIGNAL(sigViewerFocusIn(QWidget*)),
                    this, SIGNAL(sigViewerFocusIn(QWidget*)));
    Q_ASSERT(check);
#endif
    nRet = COperateDesktop::Initial();

    return nRet;
}

int COperatePlayer::InitialMenu()
{
    bool check = false;
    int nRet = 0;

#if HAVE_QT6_RECORD
    m_pRecord = new QAction(
        QIcon::fromTheme("media-record"), tr("Record"), &m_Menu);
    m_pRecord->setCheckable(true);
    check = connect(m_pRecord, SIGNAL(toggled(bool)),
                    this, SLOT(slotRecord(bool)));
    Q_ASSERT(check);

    m_pRecordPause = new QAction(
        QIcon::fromTheme("media-playback-pause"), tr("Record pause"), &m_Menu);
    m_pRecordPause->setCheckable(true);
    m_pRecordPause->setEnabled(false);
    check = connect(m_pRecordPause, SIGNAL(toggled(bool)),
                    this, SIGNAL(sigRecordPause(bool)));
    Q_ASSERT(check);
#endif

    m_pScreenShot = new QAction(QIcon::fromTheme("camera-photo"),
                                tr("ScreenShot"), &m_Menu);
    check = connect(m_pScreenShot, SIGNAL(triggered()),
                    this, SLOT(slotScreenShot()));
    Q_ASSERT(check);

#ifdef HAVE_QVideoWidget
    m_Menu.addAction(m_Player.m_paStart);
    check = connect(this, &COperatePlayer::sigRunning,
                    m_Player.m_paStart, &QAction::toggle);
    Q_ASSERT(check);
    check = connect(m_Player.m_paStart, SIGNAL(toggled(bool)),
                    this, SIGNAL(sigStart(bool)));
    Q_ASSERT(check);

    m_Menu.addAction(m_Player.m_paPause);
    check = connect(m_Player.m_paPause, SIGNAL(toggled(bool)),
                    this, SIGNAL(sigPause(bool)));
    Q_ASSERT(check);

#if HAVE_QT6_RECORD
    m_Menu.addAction(m_Player.m_paRecord);
    check = connect(m_Player.m_paRecord, SIGNAL(toggled(bool)),
                    m_pRecord ,SIGNAL(toggled(bool)));
    Q_ASSERT(check);

    m_Menu.addAction(m_Player.m_paRecordPause);
    check = connect(m_Player.m_paRecordPause, SIGNAL(toggled(bool)),
                    m_pRecordPause, SIGNAL(toggled(bool)));
    Q_ASSERT(check);
#endif

    m_Menu.addAction(m_Player.m_paScreenShot);
    check = connect(m_Player.m_paScreenShot, &QAction::triggered,
                    m_pScreenShot, &QAction::triggered);
    Q_ASSERT(check);

    m_Menu.addSeparator();

    m_Menu.addAction(m_Player.m_paSettings);
    check = connect(m_Player.m_paSettings, &QAction::triggered,
                    m_pActionSettings, &QAction::triggered);
    Q_ASSERT(check);

    check = connect(this, &COperatePlayer::sigRunning,
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
    check = connect(this, &CConnecterPlayer::sigRunning,
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
    m_Menu.addAction(m_pActiveSettings);
#endif
    return nRet;
}

int COperatePlayer::Clean()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    nRet = COperateDesktop::Clean();
    return nRet;
}

QDialog *COperatePlayer::OnOpenDialogSettings(QWidget *parent)
{
    QDialog* pDlg = new CDlgPlayer(&m_Parameters, parent);
    return pDlg;
}

CBackend *COperatePlayer::InstanceBackend()
{
    CBackend* p = new CBackendPlayer(this);
    return p;
}

void COperatePlayer::slotScreenShot()
{
    emit sigScreenShot();
}

#if HAVE_QVideoWidget
QWidget *COperatePlayer::GetViewer()
{
    return &m_Player;
}

QVideoSink *COperatePlayer::GetVideoSink()
{
    return m_Player.videoSink();
}

void COperatePlayer::slotPositionChanged(qint64 pos, qint64 duration)
{
    m_Player.slotPositionChanged(pos, duration);
}

void COperatePlayer::slotPlaybackError(
    QMediaPlayer::Error error, const QString &errorString)
{
    if(m_Player.m_paStart->isChecked())
        m_Player.m_paStart->setChecked(false);
}

void COperatePlayer::slotPlaybackStateChanged(QMediaPlayer::PlaybackState state)
{
    if(QMediaPlayer::StoppedState == state
        && m_Player.m_paStart->isChecked())
        m_Player.m_paStart->setChecked(false);
}

#if HAVE_QT6_RECORD
void COperatePlayer::slotRecordStateChanged(QMediaRecorder::RecorderState state)
{
    if(QMediaRecorder::StoppedState == state
        && m_Player.m_paRecord->isCheckable())
        m_Player.m_paRecord->setChecked(false);
}
#endif

#endif
