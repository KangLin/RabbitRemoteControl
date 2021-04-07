//! @author: Kang Lin(kl222@126.com)

#include "FrmFullScreenToolBar.h"
#include "ui_FrmFullScreenToolBar.h"
#include "RabbitCommonDir.h"
#include <QStyleOption>
#include <QVBoxLayout>
#include <QSettings>

CFrmFullScreenToolBar::CFrmFullScreenToolBar(MainWindow *pMain, QWidget *parent) :
    QWidget(parent,
            Qt::FramelessWindowHint
            #ifndef WIN32
            | Qt::X11BypassWindowManagerHint  //这个标志是在x11下有用,查看帮>助QWidget::showFullScreen(),符合ICCCM协议的不需要这个
            #endif
            | Qt::Tool
            | Qt::WindowStaysOnTopHint
            | Qt::CustomizeWindowHint
    ),
    ui(new Ui::CFrmFullScreenToolBar),
    m_ToolBar(this),
    m_pMain(pMain),
    m_TimeOut(3000),
    m_isHide(false)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure());
    
    m_pNail = m_ToolBar.addAction(QIcon(":/image/Nail"), tr("Nail"), this, SLOT(slotNail()));
    m_pNail->setCheckable(true);
    m_pNail->setChecked(set.value("FullScreen/Nail", true).toBool());
    m_ToolBar.addSeparator();
    m_ToolBar.addAction(QIcon(":/image/ExitFullScreen"),
                        tr("Full"), this, SIGNAL(sigExitFullScreen()));
    m_ToolBar.addAction(QIcon(":/image/Exit"),
                        tr("Exit"), this, SIGNAL(sigExit()));
    m_ToolBar.addSeparator();
    m_ToolBar.addAction(QIcon(":/image/Disconnect"),
                        tr("Disconnect"), this, SIGNAL(sigDisconnect()));
    m_ToolBar.show();
    
    bool check = connect(&m_Timer, SIGNAL(timeout()),
                         this, SLOT(slotTimeOut()));
    Q_ASSERT(check);
    
    m_Timer.start(m_TimeOut);
    
    ReToolBarSize();
}

CFrmFullScreenToolBar::~CFrmFullScreenToolBar()
{
    m_Timer.stop();
    delete ui;
}

void CFrmFullScreenToolBar::mouseMoveEvent(QMouseEvent *event)
{    
    if(Qt::LeftButton != event->buttons())
        return;

    QPointF p = event->pos();
    move(x() + (p.x() - m_Pos.x()), y() + (p.y() - m_Pos.y()));
}

void CFrmFullScreenToolBar::mousePressEvent(QMouseEvent *event)
{
    m_Pos = event->pos();
}

int CFrmFullScreenToolBar::ReToolBarSize()
{
    int marginW = style()->pixelMetric(
                QStyle::PM_FocusFrameHMargin) << 1;
    int marginH = style()->pixelMetric(
                QStyle::PM_FocusFrameVMargin) << 1;
    QMargins cm = contentsMargins();
    
    QSize size = m_ToolBar.frameSize();
    
    resize(marginW + cm.left() + cm.right() + size.width(),
           marginH + cm.top() + cm.bottom() + size.height());

    return 0;    
}

void CFrmFullScreenToolBar::slotTimeOut()
{
    if(m_isHide) return;
    
    if(m_pNail->isChecked()) return;
    
    m_isHide = true;
    m_Timer.stop();
    m_ToolBar.hide();
    resize(width(), 5);
}

void CFrmFullScreenToolBar::slotNail()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure());
    set.setValue("FullScreen/Nail", m_pNail->isChecked());
}

void CFrmFullScreenToolBar::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_Timer.stop();
    if(m_isHide)
    {
        m_ToolBar.show();
        ReToolBarSize();
        m_isHide = false;
    }
}

void CFrmFullScreenToolBar::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    if(m_pNail->isChecked()) return;
    m_Timer.stop();
    m_Timer.start(m_TimeOut);
}
