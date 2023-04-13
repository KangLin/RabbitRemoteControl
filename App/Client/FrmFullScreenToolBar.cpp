// Author: Kang Lin <kl222@126.com>

#include "FrmFullScreenToolBar.h"
#include "ui_FrmFullScreenToolBar.h"
#include "RabbitCommonDir.h"
#include <QStyleOption>
#include <QVBoxLayout>
#include <QSettings>
#include <QScreen>
#include <QApplication>

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
    bool check = false;
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    setFocusPolicy(Qt::NoFocus);

    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure());
    
    m_pNail = m_ToolBar.addAction(QIcon::fromTheme("nail"), tr("Nail"),
                                  this, SLOT(slotNail()));
    m_pNail->setCheckable(true);
    m_pNail->setChecked(set.value("FullScreen/Nail", true).toBool());
    m_pNail->setToolTip(tr("Nail"));
    m_pNail->setStatusTip(tr("Nail"));
    m_ToolBar.addSeparator();
    QAction* pFull = m_ToolBar.addAction(QIcon::fromTheme("view-restore"),
                        tr("Full"), this, SIGNAL(sigExitFullScreen()));
    pFull->setToolTip(tr("Full"));
    pFull->setStatusTip(tr("Full"));
    
    m_ToolBar.addSeparator();
    QAction* pZoomToWindow = m_ToolBar.addAction(QIcon::fromTheme("zoom-fit-best"),
       tr("Zoom to windows"), m_pMain, SLOT(on_actionZoomToWindow_Z_triggered()));
    m_pMain->m_pGBViewZoom->addAction(pZoomToWindow);
    QAction* pZoomOrigin = m_ToolBar.addAction(QIcon::fromTheme("zoom-original"),
                    tr("Origin"), m_pMain, SLOT(on_actionOriginal_O_triggered()));
    m_pMain->m_pGBViewZoom->addAction(pZoomOrigin);
    QAction* pZoomIn = m_ToolBar.addAction(QIcon::fromTheme("zoom-in"),
                    tr("Zoom In"), m_pMain, SLOT(on_actionZoom_In_triggered()));
    m_pMain->m_pGBViewZoom->addAction(pZoomIn);
//    QSpinBox* psbZoomFactor = new QSpinBox(&m_ToolBar);
//    psbZoomFactor->setRange(0, 9999999);
//    psbZoomFactor->setValue(100);
//    psbZoomFactor->setSuffix("%");
//    psbZoomFactor->setEnabled(false);
//    check = connect(psbZoomFactor, SIGNAL(valueChanged(int)),
//                    m_pMain, SLOT(slotZoomFactor(int)));
//    Q_ASSERT(check);
//    m_ToolBar.addWidget(psbZoomFactor);
    QAction* pZoomOut = m_ToolBar.addAction(QIcon::fromTheme("zoom-out"),
                  tr("Zoom Out"), m_pMain, SLOT(on_actionZoom_Out_triggered()));
    m_pMain->m_pGBViewZoom->addAction(pZoomOut);

    m_ToolBar.addSeparator();
    m_ToolBar.addAction(QIcon::fromTheme("camera-photo"), tr("ScreenShot"),
                        m_pMain, SLOT(on_actionScreenshot_triggered()));
    m_ToolBar.addAction(QIcon::fromTheme("emblem-favorite"), tr("Add to favorite"),
                        m_pMain, SLOT(on_actionAdd_to_favorite_triggered()));
    m_pShowTabBar = m_ToolBar.addAction(QIcon::fromTheme("tabbar"), tr("TabBar"),
                                        this, SLOT(slotShowTabBar()));
    m_pShowTabBar->setCheckable(true);
    m_pShowTabBar->setStatusTip(tr("Tab bar"));
    m_pShowTabBar->setToolTip(tr("Tab bar"));
    QAction* pDisconnect = m_ToolBar.addAction(QIcon::fromTheme("network-wireless"),
                        tr("Disconnect"), this, SIGNAL(sigDisconnect()));
    pDisconnect->setToolTip(tr("Disconnect"));
    pDisconnect->setStatusTip(tr("Disconnect"));
    m_ToolBar.addSeparator();
    QAction* pExit = m_ToolBar.addAction(QIcon::fromTheme("window-close"),
                        tr("Exit"), this, SIGNAL(sigExit()));
    pExit->setToolTip(tr("Exit"));
    pExit->setStatusTip(tr("Exit"));
    
    m_ToolBar.show();

    check = connect(&m_Timer, SIGNAL(timeout()),
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
    
    if(frameGeometry().top() > m_pMain->frameGeometry().height() >> 1)
        move(frameGeometry().left(),
             m_pMain->frameGeometry().height() - frameGeometry().height());
    return 0;    
}

void CFrmFullScreenToolBar::slotTimeOut()
{
    int area = 5;
    if(m_isHide) return;
    
    if(m_pNail->isChecked()) return;
    
    m_isHide = true;
    m_Timer.stop();
    m_ToolBar.hide();
    resize(width(), area);

    int h = m_pMain->frameGeometry().height() >> 1;
    if(frameGeometry().top() < h)
        move(frameGeometry().left(), 0);
    else
        move(frameGeometry().left(),  m_pMain->frameGeometry().height() - area);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void CFrmFullScreenToolBar::enterEvent(QEnterEvent *event)
#else
void CFrmFullScreenToolBar::enterEvent(QEvent *event)
#endif
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

void CFrmFullScreenToolBar::slotShowTabBar()
{
    emit sigShowTabBar(m_pShowTabBar->isChecked());
}

void CFrmFullScreenToolBar::slotNail()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure());
    set.setValue("FullScreen/Nail", m_pNail->isChecked());
}
