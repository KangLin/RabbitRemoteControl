// Author: Kang Lin <kl222@126.com>

#include <QKeyEvent>
#include <QMouseEvent>
#include <QLoggingCategory>
#include <QToolButton>
#include <QTime>
#include <QVBoxLayout>
#include <QResizeEvent>

#include "FrmPlayer.h"
#ifndef WITH_QVideoWidget
#include <QGraphicsVideoItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#endif

static Q_LOGGING_CATEGORY(log, "FrmPlayer")

#ifndef WITH_QVideoWidget
class CVideoGraphicsView : public QGraphicsView {
public:
    using QGraphicsView::QGraphicsView;
protected:
    void resizeEvent(QResizeEvent *e) override {
        QGraphicsView::resizeEvent(e);
        if (scene() && !scene()->items().isEmpty()) {
            auto *item = scene()->items().first();
            fitInView(item, Qt::KeepAspectRatio);
        }
    }
};
#endif

CFrmPlayer::CFrmPlayer(QWidget *parent) : QWidget(parent)
    , m_paStart(nullptr)
    , m_paPause(nullptr)
#if HAVE_QT6_RECORD
    , m_paRecord(nullptr)
    , m_paRecordPause(nullptr)
#endif
    , m_paMuted(nullptr)
    , m_paVolume(nullptr)
#ifdef WITH_QVideoWidget
    , m_pVideoWidget(nullptr)
#else
    , m_pGraphicsView(nullptr)
    , m_pVideoItem(nullptr)
#endif
    , m_pToolBar(nullptr)
    , tm_ToolBar(this)
    , m_pbVideo(Qt::Horizontal, this)
    , m_pbVolume(Qt::Horizontal, this)
    , m_bMoveVideo(false)
    , m_pParameter(nullptr)
    , m_pLabel(nullptr)
    , m_bFullScreen(false)
{
    bool check = false;

    qDebug(log) << Q_FUNC_INFO;

    setFocusPolicy(Qt::WheelFocus);

    QVBoxLayout* pLayout = new QVBoxLayout(this);
    setLayout(pLayout);

#ifdef WITH_QVideoWidget
    m_pVideoWidget = new QVideoWidget(this);
    m_pVideoWidget->setFocusPolicy(Qt::WheelFocus);
    m_pVideoWidget->installEventFilter(this);
    m_pVideoWidget->setMouseTracking(true);
    // ← 遍历内部子对象也装过滤器 + 开 tracking
    for (QObject *o : m_pVideoWidget->findChildren<QObject*>()) {
        o->installEventFilter(this);
        if (auto *w = qobject_cast<QWidget*>(o))
            w->setMouseTracking(true);
    }
    pLayout->addWidget(m_pVideoWidget);
#else
    // 创建场景和视频项
    QGraphicsScene *pScene = new QGraphicsScene(this);
    m_pVideoItem = new QGraphicsVideoItem;
    pScene->addItem(m_pVideoItem);
    // 创建视图并添加到布局
    m_pGraphicsView = new CVideoGraphicsView(pScene, this);
    m_pGraphicsView->setRenderHint(QPainter::SmoothPixmapTransform);
    m_pGraphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pGraphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pGraphicsView->setFrameStyle(0);
    m_pGraphicsView->setBackgroundBrush(Qt::black);
    m_pGraphicsView->setMouseTracking(true);
    m_pGraphicsView->viewport()->setMouseTracking(true);
    m_pGraphicsView->viewport()->installEventFilter(this);
    // 关视频尺寸变化时，重新 fitInView
    connect(m_pVideoItem, &QGraphicsVideoItem::nativeSizeChanged,
            this, [this](const QSizeF &size) {
                if (size.isEmpty())
                    return;
                if(!m_pGraphicsView || !m_pVideoItem)
                    return;
                // 让 videoItem 的矩形匹配视频原始尺寸
                m_pVideoItem->setSize(size);
                UpdateGraphicsVideoGeometry();
            });
    pLayout->addWidget((m_pGraphicsView));
#endif

    m_pToolBar = new QToolBar(this);
    m_paStart = m_pToolBar->addAction(
        QIcon::fromTheme("media-playback-start"), tr("Start"));
    m_paStart->setCheckable(true);
    check = connect(m_paStart, SIGNAL(toggled(bool)),
                    this, SLOT(slotStart(bool)));
    Q_ASSERT(check);

    m_paPause = m_pToolBar->addAction(
        QIcon::fromTheme("media-playback-pause"), tr("pause"));
    m_paPause->setCheckable(true);
    m_paPause->setEnabled(false);

    m_pToolBar->addSeparator();
    m_pToolBar->addAction(QIcon::fromTheme("media-seek-backward"), tr("Backward"),
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
    m_pToolBar->addWidget(&m_pbVideo);

    m_pToolBar->addAction(QIcon::fromTheme("media-seek-forward"), tr("Forward"),
                          this, [&](){
                              qDebug(log) << "Forward action";
                              emit sigChangePosition(m_pbVideo.value() + 1000);
                          });

    m_pToolBar->addSeparator();
    m_pLabel = new QLabel(m_pToolBar);
    m_pLabel->setText("00:00:00 / 00:00:00");
    m_pToolBar->addWidget(m_pLabel);
    m_pToolBar->addSeparator();

    m_paScreenShot = m_pToolBar->addAction(
        QIcon::fromTheme("camera-photo"), tr("ScreenShot"));
    m_paScreenShot->setEnabled(false);

#if HAVE_QT6_RECORD
    m_paRecordPause = m_pToolBar->addAction(
        QIcon::fromTheme("media-playback-pause"), tr("Record pause"));
    m_paRecordPause->setCheckable(true);
    m_paRecordPause->setEnabled(false);

    m_paRecord = m_pToolBar->addAction(
        QIcon::fromTheme("media-record"), tr("Record"));
    m_paRecord->setCheckable(true);
    m_paRecord->setEnabled(false);
    check = connect(m_paRecord, &QAction::toggled, this, [&](bool checked){
        m_paRecordPause->setEnabled(checked);
        m_paRecordPause->setChecked(false);
    });
    Q_ASSERT(check);
#endif

    m_paSettings = m_pToolBar->addAction(
        QIcon::fromTheme("system-settings"), tr("Settings"));

    m_paMuted = m_pToolBar->addAction(
        QIcon::fromTheme("audio-volume-medium"), tr("Audio"));
    m_paMuted->setCheckable(true);
    check = connect(m_paMuted, SIGNAL(toggled(bool)),
                    this, SLOT(slotAudioMuted(bool)));
    Q_ASSERT(check);

    m_pbVolume.setRange(0, 100);
    m_pbVolume.setValue(0);
    m_pbVolume.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_paVolume = m_pToolBar->addWidget(&m_pbVolume);
    check = connect(&m_pbVolume, SIGNAL(sliderMoved(int)),
                    this, SLOT(slotAduioVolume(int)));
    Q_ASSERT(check);

    pLayout->addWidget(m_pToolBar);
}

CFrmPlayer::~CFrmPlayer()
{
    qDebug(log) << Q_FUNC_INFO;
}

QVideoSink *CFrmPlayer::videoSink()
{
#ifdef WITH_QVideoWidget
    return m_pVideoWidget->videoSink();
#else
    return m_pVideoItem->videoSink();
#endif
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

//! [Full Screen]
int CFrmPlayer::OnFullScreen(bool bFull)
{
    int nRet = 0;
    m_bFullScreen = bFull;
    if(bFull) {
        m_Margins = layout()->contentsMargins();
        layout()->setContentsMargins(QMargins());
    } else {
        layout()->setContentsMargins(m_Margins);
    }
    if(m_pToolBar && m_bFullScreen)
        m_pToolBar->hide();
    return nRet;
}
//! [Full Screen]

void CFrmPlayer::focusInEvent(QFocusEvent *event)
{
    qDebug(log) << Q_FUNC_INFO << event << this;
    Q_UNUSED(event)
    emit sigViewerFocusIn(this);
}

void CFrmPlayer::focusOutEvent(QFocusEvent *event)
{
    qDebug(log) << Q_FUNC_INFO << event << this;
    Q_UNUSED(event)
}

void CFrmPlayer::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug(log) << Q_FUNC_INFO;
    Q_UNUSED(event)
    if(m_pToolBar && m_pToolBar->isHidden()) {
        m_pToolBar->show();
        StartTimer();
    }
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
        StartTimer();
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
        tm_ToolBar.stop();
        m_pToolBar->show();
    }
}

void CFrmPlayer::slotTimeOut()
{
    if(!m_pToolBar)
        return;
    m_pToolBar->hide();
}

void CFrmPlayer::StartTimer()
{
    if(m_pParameter && m_pParameter->GetHideToolBar() <= 0
        || !m_bFullScreen)
        return;
    if(m_paStart->isChecked()) {
        tm_ToolBar.stop();
        tm_ToolBar.singleShot(m_pParameter->GetHideToolBar() * 1000,
                              this, SLOT(slotTimeOut()));
    }
}

bool CFrmPlayer::eventFilter(QObject *watched, QEvent *event)
{
//qDebug(log) << Q_FUNC_INFO << event;
#ifdef WITH_QVideoWidget
    if(m_pVideoWidget == watched
        || m_pVideoWidget->isAncestorOf(qobject_cast<QWidget*>(watched)))
    {
        switch(event->type()){
        case QEvent::MouseMove: {
            qDebug(log) << "Mouse move";
            QMouseEvent* e = (QMouseEvent*)event;
            mouseMoveEvent(e);
            return true;
        }
        case QEvent::MouseButtonRelease:
            m_paPause->trigger();
            break;
        case QEvent::MouseButtonDblClick: {
            m_pVideoWidget->setFullScreen(!m_pVideoWidget->isFullScreen());
            break;
        }
        case QEvent::KeyRelease:
        {
            QKeyEvent* k = (QKeyEvent*)(event);
            switch(k->key())
            {
            case Qt::Key_Escape:
                if(m_pVideoWidget->isFullScreen()) {
                    m_pVideoWidget->setFullScreen(false);
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
        case QEvent::FocusIn:
        {
            //qDebug(log) << Q_FUNC_INFO << event;
            emit sigViewerFocusIn(this);
            return false;
        }
        default:
            return false;
        }
        return true;
    }
#else
    if(m_pGraphicsView->viewport() == watched) {
        switch(event->type()){
        case QEvent::MouseMove: {
            qDebug(log) << "Mouse move";
            QMouseEvent* e = (QMouseEvent*)event;
            mouseMoveEvent(e);
            return true;
        }
        default:
            break;
        }
    }
#endif
    return false; // 让 widget 自己继续处理
}

#ifndef WITH_QVideoWidget
void CFrmPlayer::UpdateGraphicsVideoGeometry()
{
    if (!m_pGraphicsView || !m_pVideoItem)
        return;
    if (m_pVideoItem->boundingRect().isEmpty())
        return;

    // 保证场景矩形和 item 一致，避免偏移
    m_pGraphicsView->setSceneRect(m_pVideoItem->boundingRect());
    m_pGraphicsView->fitInView(m_pVideoItem, Qt::KeepAspectRatio);
}
#endif
