// Author: Kang Lin <kl222@126.com>

#include "FrmFullScreenToolBar.h"
#include "ui_FrmFullScreenToolBar.h"
#include "ui_mainwindow.h"
#include "RabbitCommonDir.h"
#include <QStyleOption>
#include <QVBoxLayout>
#include <QSettings>
#include <QScreen>
#include <QApplication>
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "App.MainWindow.FullScreen")
static Q_LOGGING_CATEGORY(logRecord, "App.MainWindow.FullScreen.Record")

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
    m_pOperateMenu(nullptr),
    m_pNail(nullptr),
    m_pMain(pMain),
    m_TimeOut(3000),
    m_isHide(false)
{
    bool check = false;
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    //setFocusPolicy(Qt::NoFocus);

    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure());
    m_ToolBar.addSeparator();
    m_pNail = m_ToolBar.addAction(QIcon::fromTheme("nail"), tr("Nail"),
                                  this, SLOT(slotNail()));
    m_pNail->setCheckable(true);
    m_pNail->setChecked(set.value("FullScreen/Nail", true).toBool());
    m_pNail->setToolTip(tr("Nail"));
    m_pNail->setStatusTip(tr("Nail"));
    m_ToolBar.addSeparator();
    m_ToolBar.addAction(m_pMain->ui->actionFull_screen_F);

    m_ToolBar.addSeparator();
    if(m_pMain->m_pActionOperateMenu) {
        m_pOperateMenu = m_pMain->m_pActionOperateMenu;
        m_ToolBar.addAction(m_pOperateMenu);
    }

    m_ToolBar.addAction(m_pMain->ui->actionStop);
    m_ToolBar.addSeparator();
    m_ToolBar.addAction(m_pMain->ui->actionExit_E);

    m_ToolBar.show();

    check = connect(&m_Timer, SIGNAL(timeout()),
                    this, SLOT(slotTimeOut()));
    Q_ASSERT(check);

    m_Timer.start(m_TimeOut);

    ReToolBarSize();
}

CFrmFullScreenToolBar::~CFrmFullScreenToolBar()
{
    qDebug(log) << "CFrmFullScreenToolBar::~CFrmFullScreenToolBar()";

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

void CFrmFullScreenToolBar::slotNail()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure());
    set.setValue("FullScreen/Nail", m_pNail->isChecked());
}

void CFrmFullScreenToolBar::slotOperateMenuChanged(QAction* pAction)
{
    if(m_pOperateMenu) {
        m_ToolBar.removeAction(m_pOperateMenu);
        m_pOperateMenu = pAction;
        m_ToolBar.insertAction(m_pMain->ui->actionStop, m_pOperateMenu);
    }
}
