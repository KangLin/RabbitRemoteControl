
#include <QKeyEvent>
#include <QMouseEvent>
#include <QLoggingCategory>
#include "FrmPlayer.h"

static Q_LOGGING_CATEGORY(log, "FrmPlayer")

CFrmPlayer::CFrmPlayer(QWidget *parent) : QVideoWidget(parent)
    , m_ToolBar(this)
    , m_nInterval(3)
{
    setFocusPolicy(Qt::FocusPolicy::WheelFocus);
    /*
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    QPalette p = palette();
    p.setColor(QPalette::Window, Qt::black);
    setPalette(p);
    //*/
#ifndef Q_OS_ANDROID // QTBUG-95723
    setAttribute(Qt::WA_OpaquePaintEvent);
#endif

    bool check = false;
    check = connect(&m_Timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));
    Q_ASSERT(check);
    m_ToolBar.addAction(QIcon::fromTheme("media-playback-start"), tr("Start"),
                        this, [&](){
        qDebug(log) << "Start action";
    });
}

CFrmPlayer::~CFrmPlayer()
{
    qDebug(log) << "CFrmPlayer::~CFrmPlayer()";
}

void CFrmPlayer::keyPressEvent(QKeyEvent *event)
{
    qDebug(log) << "CFrmPlayer::keyPressEvent";
    m_Start = QTime::currentTime();
    if ((event->key() == Qt::Key_Escape || event->key() == Qt::Key_Back) && isFullScreen()) {
        setFullScreen(false);
        event->accept();
    } else if (event->key() == Qt::Key_Enter && event->modifiers() & Qt::Key_Alt) {
        setFullScreen(!isFullScreen());
        event->accept();
    } else {
        QVideoWidget::keyPressEvent(event);
    }
}

void CFrmPlayer::mouseDoubleClickEvent(QMouseEvent *event)
{
    qDebug(log) << "CFrmPlayer::mouseDoubleClickEvent";
    setFullScreen(!isFullScreen());
    event->accept();
    m_Start = QTime::currentTime();
}

void CFrmPlayer::mousePressEvent(QMouseEvent *event)
{
    qDebug(log) << "CFrmPlayer::mousePressEvent()";
    QVideoWidget::mousePressEvent(event);
    m_Start = QTime::currentTime();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void CFrmPlayer::enterEvent(QEnterEvent *event)
#else
void CFrmPlayer::enterEvent(QEvent *event)
#endif
{
    qDebug(log) << "CFrmPlayer::enterEvent()";
    m_ToolBar.show();
    m_Start = QTime::currentTime();
    if(!m_Timer.isActive())
        m_Timer.start(m_nInterval);
}

void CFrmPlayer::leaveEvent(QEvent *event)
{
    qDebug(log) << "CFrmPlayer::leaveEvent()";
}

void CFrmPlayer::slotTimeout()
{
    if(m_Start.secsTo(QTime::currentTime()) > m_nInterval) {
        m_ToolBar.hide();
        m_Timer.stop();
    }
}

void CFrmPlayer::mouseMoveEvent(QMouseEvent *event)
{
    qDebug(log) << "CFrmPlayer::mouseMoveEvent()";
    m_ToolBar.show();

    m_Start = QTime::currentTime();
    if(!m_Timer.isActive())
        m_Timer.start(m_nInterval);
    QVideoWidget::mousePressEvent(event);
}

void CFrmPlayer::resizeEvent(QResizeEvent *event)
{
    QVideoWidget::resizeEvent(event);
    qDebug(log) << "CFrmPlayer::resizeEvent()" << event;
    QSize s = event->size();
    int left = (s.width() - m_ToolBar.frameGeometry().width()) >> 1;
    int top = s.height() - m_ToolBar.frameGeometry().height();
    m_ToolBar.move(left, top);
}
