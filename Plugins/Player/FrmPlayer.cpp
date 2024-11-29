// Author: Kang Lin <kl222@126.com>

#include <QKeyEvent>
#include <QMouseEvent>
#include <QLoggingCategory>
#include <QToolButton>
#include <QTime>

#include "FrmPlayer.h"

static Q_LOGGING_CATEGORY(log, "FrmPlayer")

CFrmPlayer::CFrmPlayer(QWidget *parent) : QWidget(parent)
    , m_paStart(nullptr)
    , m_paPause(nullptr)
#if HAVE_QT6_RECORD
    , m_paRecord(nullptr)
    , m_paRecordPause(nullptr)
#endif
    , m_paMuted(nullptr)
    , m_paVolume(nullptr)
    , m_VideoWidget(this)
    , m_ToolBar(this)
    , m_pbVideo(Qt::Horizontal, this)
    , m_pbVolume(Qt::Horizontal, this)
    , m_bMoveVideo(false)
    , m_pParameter(nullptr)
    , m_pLabel(nullptr)
{
    bool check = false;

    qDebug(log) << Q_FUNC_INFO;

    m_VideoWidget.installEventFilter(this);

    m_paStart = m_ToolBar.addAction(
        QIcon::fromTheme("media-playback-start"), tr("Start"));
    m_paStart->setCheckable(true);
    check = connect(m_paStart, SIGNAL(toggled(bool)),
                    this, SLOT(slotStart(bool)));
    Q_ASSERT(check);

    m_paPause = m_ToolBar.addAction(
        QIcon::fromTheme("media-playback-pause"), tr("pause"));
    m_paPause->setCheckable(true);
    m_paPause->setEnabled(false);

    m_ToolBar.addSeparator();
    m_ToolBar.addAction(QIcon::fromTheme("media-seek-backward"), tr("Backward"),
                        this, [&](){
                            qDebug(log) << "Backward action";
                            emit sigChangePosition(m_pbVideo.value() - 1000);
                        });

    m_pbVideo.setRange(0, 0);
    m_pbVideo.setValue(0);
    m_pbVideo.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    check = connect(&m_pbVideo, &QSlider::sliderPressed,
                    this, [&](){
                        m_bMoveVideo = true;
                    });
    Q_ASSERT(check);
    check = connect(&m_pbVideo, &QSlider::sliderReleased,
                    this, [&](){
                        m_bMoveVideo = false;
                        emit sigChangePosition(m_pbVideo.value());
                    });
    Q_ASSERT(check);
    m_ToolBar.addWidget(&m_pbVideo);

    m_ToolBar.addAction(QIcon::fromTheme("media-seek-forward"), tr("Forward"),
                        this, [&](){
                            qDebug(log) << "Forward action";
                            emit sigChangePosition(m_pbVideo.value() + 1000);
                        });

    m_ToolBar.addSeparator();
    m_pLabel = new QLabel(&m_ToolBar);
    m_pLabel->setText("00:00:00 / 00:00:00");
    m_ToolBar.addWidget(m_pLabel);
    m_ToolBar.addSeparator();

    m_paScreenShot = m_ToolBar.addAction(
        QIcon::fromTheme("camera-photo"), tr("ScreenShot"));
    m_paScreenShot->setEnabled(false);

#if HAVE_QT6_RECORD
    m_paRecord = m_ToolBar.addAction(
        QIcon::fromTheme("media-record"), tr("Record"));
    m_paRecord->setCheckable(true);
    m_paRecord->setEnabled(false);
    check = connect(m_paRecord, &QAction::toggled, this, [&](bool checked){
        m_paRecordPause->setEnabled(checked);
        if(checked)
            m_paRecordPause->setChecked(false);
    });
    Q_ASSERT(check);
    m_paRecordPause = m_ToolBar.addAction(
        QIcon::fromTheme("media-playback-pause"), tr("Record pause"));
    m_paRecordPause->setCheckable(true);
    m_paRecordPause->setEnabled(false);
#endif

    m_paSettings = m_ToolBar.addAction(
        QIcon::fromTheme("system-settings"), tr("Settings"));

    m_paMuted = m_ToolBar.addAction(
        QIcon::fromTheme("audio-volume-medium"), tr("Audio"));
    m_paMuted->setCheckable(true);
    check = connect(m_paMuted, SIGNAL(toggled(bool)),
                    this, SLOT(slotAudioMuted(bool)));
    Q_ASSERT(check);

    m_pbVolume.setRange(0, 100);
    m_pbVolume.setValue(0);
    m_pbVolume.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_paVolume = m_ToolBar.addWidget(&m_pbVolume);
    check = connect(&m_pbVolume, SIGNAL(sliderMoved(int)),
                    this, SLOT(slotAduioVolume(int)));
    Q_ASSERT(check);
}

CFrmPlayer::~CFrmPlayer()
{
    qDebug(log) << Q_FUNC_INFO;
}

QVideoSink *CFrmPlayer::videoSink()
{
    return m_VideoWidget.videoSink();
}

int CFrmPlayer::SetParameter(CParameterPlayer* pParameter)
{
    if(!pParameter || m_pParameter == pParameter)
        return -1;

    m_pParameter = pParameter;

    m_paMuted->setChecked(m_pParameter->GetAudioOutputMuted());
    m_pbVolume.setValue(m_pParameter->GetAudioOutputVolume());
    return 0;
}

void CFrmPlayer::slotAudioMuted(bool bMuted)
{
    if(!m_pParameter)
        return;

    if(bMuted) {
        m_paMuted->setIcon(QIcon::fromTheme("audio-volume-muted"));
        m_paMuted->setText(tr("Muted"));
    } else {
        m_paMuted->setIcon(QIcon::fromTheme("audio-volume-medium"));
        m_paMuted->setText(tr("Audio"));
    }
    m_pParameter->SetAudioOutputMuted(bMuted);
    m_paVolume->setEnabled(!bMuted);
}

void CFrmPlayer::slotAduioVolume(int volume)
{
    qDebug(log) << "Volume:" << volume;
    if(!m_pParameter)
        return;
    m_pParameter->SetAudioOutputVolume(volume);
}

void CFrmPlayer::slotPositionChanged(qint64 pos, qint64 duration)
{
    qint64 currentInfo = pos / 1000;
    qint64 dur = duration / 1000;

    QString szStr;
    if (currentInfo || dur) {
        QTime currentTime((currentInfo / 3600) % 60,
                          (currentInfo / 60) % 60,
                          currentInfo % 60,
                          (currentInfo * 1000) % 1000);
        QTime totalTime((dur / 3600) % 60,
                        (dur / 60) % 60,
                        dur % 60,
                        (dur * 1000) % 1000);
        QString format = "mm:ss";
        if (dur > 3600)
            format = "hh:mm:ss";
        szStr = currentTime.toString(format)
                + " / " + totalTime.toString(format);
        m_pLabel->setText(szStr);
        m_pbVideo.setRange(0, duration);
        if(!m_bMoveVideo)
            m_pbVideo.setValue(pos);
    }
}

void CFrmPlayer::resizeEvent(QResizeEvent *event)
{
    qDebug(log) << "CFrmPlayer::resizeEvent()" << event;
    QSize s = event->size();
    AdjustCompone(s);
    QWidget::resizeEvent(event);
}

int CFrmPlayer::AdjustCompone(const QSize &s)
{
    m_VideoWidget.move(0, 0);
    QRect rect(0, 0, s.width(), s.height() - m_ToolBar.frameGeometry().height());
    m_VideoWidget.setGeometry(rect);
    int left = 0;
    int top = s.height() - m_ToolBar.frameGeometry().height();
    m_ToolBar.move(left, top);
    m_ToolBar.resize(s.width(), m_ToolBar.height());
    return 0;
}

void CFrmPlayer::slotStart(bool bStart)
{
    QAction* p = qobject_cast<QAction*>(sender());
    if(!p) return;
    if(p->isChecked()) {
        p->setIcon(QIcon::fromTheme("media-playback-stop"));
        p->setText(tr("Stop"));
        m_paPause->setEnabled(true);
        m_paPause->setChecked(false);
        m_paScreenShot->setEnabled(true);
#if HAVE_QT6_RECORD
        m_paRecord->setEnabled(true);
        m_paRecord->setChecked(false);
        m_paRecordPause->setEnabled(true);
        m_paRecordPause->setChecked(false);
#endif
    } else {
        p->setIcon(QIcon::fromTheme("media-playback-start"));
        p->setText(tr("Start"));
        m_paPause->setEnabled(false);
        m_paPause->setChecked(false);
        m_paScreenShot->setEnabled(false);
#if HAVE_QT6_RECORD
        m_paRecord->setEnabled(false);
        m_paRecord->setChecked(false);
        m_paRecordPause->setEnabled(false);
        m_paRecordPause->setChecked(false);
#endif
    }
}

bool CFrmPlayer::eventFilter(QObject *watched, QEvent *event)
{
    //qDebug(log) << Q_FUNC_INFO << event;
    if(&m_VideoWidget == watched)
    {
        switch(event->type()){
        case QEvent::MouseMove:
            qDebug(log) << "Mouse move";
            break;
        case QEvent::MouseButtonRelease:
            m_paPause->trigger();
            break;
        case QEvent::MouseButtonDblClick: {
            m_VideoWidget.setFullScreen(!m_VideoWidget.isFullScreen());
            if(!m_VideoWidget.isFullScreen()) {
                QSize s = size();
                AdjustCompone(s);
            }
            break;
        }
        case QEvent::KeyRelease:
        {
            QKeyEvent* k = (QKeyEvent*)(event);
            switch(k->key())
            {
            case Qt::Key_Escape:
                if(m_VideoWidget.isFullScreen()) {
                    m_VideoWidget.setFullScreen(false);
                    QSize s = size();
                    AdjustCompone(s);
                }
                break;
            case Qt::Key_Enter:
            case Qt::Key_Space:
                m_paPause->trigger();
                break;
            default:
                break;
            }
            break;
        }
        default:
            return false;
        }
        return true;
    }
    return false;
}
