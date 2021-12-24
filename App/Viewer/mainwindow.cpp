// Author: Kang Lin <kl222@126.com>

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#ifdef HAVE_UPDATE
#include "FrmUpdater/FrmUpdater.h"
#endif
#include "RabbitCommonDir.h"
#include "RabbitCommonStyle.h"
#include "RabbitCommonLog.h"
#ifdef HAVE_ABOUT
#include "DlgAbout/DlgAbout.h"
#endif
#ifdef BUILD_QUIWidget
    #include "QUIWidget/QUIWidget.h"
#endif

#include "Connecter.h"
#include "FrmFullScreenToolBar.h"
#include "DlgSettings.h"

#include <QMessageBox>
#include <QScreen>
#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QWidgetAction>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_pGBView(nullptr),
      m_ptbZoom(nullptr),
      m_psbZoomFactor(nullptr),
      m_pRecentMenu(nullptr),
      m_pView(nullptr),
      m_pFullScreenToolBar(nullptr),
      m_bFullScreen(isFullScreen())
{
    bool check = false;

    RabbitCommon::CStyle::Instance()->LoadStyle();
    ui->setupUi(this);

    m_pRecentMenu = new RabbitCommon::CRecentMenu(this);
    check = connect(m_pRecentMenu, SIGNAL(recentFileTriggered(const QString&)),
                    this, SLOT(slotRecentFileTriggered(const QString&)));
    Q_ASSERT(check);

    ui->actionRecently_connected->setMenu(m_pRecentMenu);

#ifdef HAVE_UPDATE
    CFrmUpdater updater;
    ui->actionUpdate_U->setIcon(updater.windowIcon());
#endif

    //TODO: Change view
    m_pView = new CViewTable(this);
    if(m_pView)
    {
        m_pView->setFocusPolicy(Qt::NoFocus);
        check = connect(m_pView, SIGNAL(sigCloseView(const QWidget*)),
                        this, SLOT(slotCloseView(const QWidget*)));
        Q_ASSERT(check);
        check = connect(m_pView, SIGNAL(sigAdaptWindows(const CFrmViewer::ADAPT_WINDOWS)),
                        this, SLOT(slotAdaptWindows(const CFrmViewer::ADAPT_WINDOWS)));
        Q_ASSERT(check);
        this->setCentralWidget(m_pView);
    }
    
    m_ManageConnecter.EnumPlugins(this);
    
    QToolButton* tbConnect = new QToolButton(ui->toolBar);
    tbConnect->setFocusPolicy(Qt::NoFocus);
    tbConnect->setPopupMode(QToolButton::MenuButtonPopup);
    //tbConnect->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    tbConnect->setMenu(ui->menuConnect_C);
    tbConnect->setIcon(QIcon(":/image/Connect"));
    tbConnect->setText(tr("Connect"));
    tbConnect->setToolTip(tr("Connect"));
    tbConnect->setStatusTip(tr("Connect"));
    ui->toolBar->insertWidget(ui->actionDisconnect_D, tbConnect);
    
    m_ptbZoom = new QToolButton(ui->toolBar);
    m_ptbZoom->setPopupMode(QToolButton::MenuButtonPopup);
    //m_ptbZoom->setToolButtonStyle(Qt::ToolButtonFollowStyle);
    m_ptbZoom->setMenu(ui->menuZoom);
    m_ptbZoom->setIcon(QIcon(":/image/Zoom"));
    m_ptbZoom->setText(tr("Zoom"));
    m_ptbZoom->setStatusTip(tr("Zoom"));
    m_ptbZoom->setToolTip(tr("Zoom"));
    m_ptbZoom->setEnabled(false);
    ui->toolBar->insertWidget(ui->actionFull_screen_F, m_ptbZoom);

    m_psbZoomFactor = new QSpinBox(ui->toolBar);
    m_psbZoomFactor->setRange(0, 9999999);
    m_psbZoomFactor->setValue(100);
    m_psbZoomFactor->setSuffix("%");
    m_psbZoomFactor->setEnabled(false);
    m_psbZoomFactor->setFocusPolicy(Qt::NoFocus);
    check = connect(m_psbZoomFactor, SIGNAL(valueChanged(int)),
                    this, SLOT(slotZoomFactor(int)));
    Q_ASSERT(check);
    QWidgetAction* pFactor = new QWidgetAction(ui->menuZoom);
    pFactor->setDefaultWidget(m_psbZoomFactor);
    ui->menuZoom->insertAction(ui->actionZoom_Out, pFactor);
    
    m_pGBView = new QActionGroup(this);
    if(m_pGBView) {
        m_pGBView->addAction(ui->actionZoomToWindow_Z);
        m_pGBView->addAction(ui->actionKeep_aspect_ration_to_windows_K);
        m_pGBView->addAction(ui->actionOriginal_O);
        m_pGBView->addAction(ui->actionZoom_In);
        m_pGBView->addAction(ui->actionZoom_Out);
        m_pGBView->setEnabled(false);
    }
    ui->actionZoomToWindow_Z->setChecked(true);
    
    setFocusPolicy(Qt::NoFocus);
    
    //TODO: modify the bug
    ui->actionZoom_window_to_remote_desktop->setVisible(false);
    
    check = connect(&m_Parameter, SIGNAL(sigReceiveShortCutChanged()),
                    this, SLOT(slotShortCut()));
    Q_ASSERT(check);
    m_Parameter.Load();
    slotShortCut();
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow::~MainWindow()";
    if(m_pFullScreenToolBar) m_pFullScreenToolBar->close();
    if(m_pGBView) delete m_pGBView;
    
    delete ui;
}

void MainWindow::on_actionAbout_A_triggered()
{
#ifdef HAVE_ABOUT
    CDlgAbout *about = new CDlgAbout(this);
    about->m_AppIcon = QImage(":/image/App");
    about->m_szCopyrightStartTime = "2020";
    if(about->isHidden())
    {
#ifdef BUILD_QUIWidget
        QUIWidget quiwidget;
        quiwidget.setMainWidget(about);
        quiwidget.setPixmap(QUIWidget::Lab_Ico, ":/image/App");
    #if defined (Q_OS_ANDROID)
        quiwidget.showMaximized();
    #endif
        quiwidget.exec();
#else
    #if defined (Q_OS_ANDROID)
        about->showMaximized();
    #endif
        about->exec();
#endif
    }
#endif
}

void MainWindow::on_actionUpdate_U_triggered()
{
#ifdef HAVE_UPDATE
    CFrmUpdater* m_pfrmUpdater = new CFrmUpdater();
    m_pfrmUpdater->SetTitle(QImage(":/image/App"));
    m_pfrmUpdater->SetInstallAutoStartup();
#ifdef BUILD_QUIWidget
    QUIWidget* pQuiwidget = new QUIWidget(nullptr, true);
    pQuiwidget->setMainWidget(m_pfrmUpdater);
#if defined (Q_OS_ANDROID)
    pQuiwidget->showMaximized();
#else
    pQuiwidget->show();
#endif 
#else
#if defined (Q_OS_ANDROID)
    m_pfrmUpdater->showMaximized();
#else
    m_pfrmUpdater->show();
#endif 
#endif
#endif
}

void MainWindow::on_actionStatusBar_S_triggered()
{
    ui->statusbar->setVisible(!ui->statusbar->isVisible());
    ui->actionStatusBar_S->setChecked(ui->statusbar->isVisible());
}

void MainWindow::on_actionToolBar_T_triggered()
{
    ui->toolBar->setVisible(!ui->toolBar->isVisible());
    ui->actionToolBar_T->setChecked(ui->toolBar->isVisible());
}

void MainWindow::on_actionFull_screen_F_triggered()
{
    CView* pTab = dynamic_cast<CView*>(this->centralWidget());
    if(pTab)
    {
        pTab->SetFullScreen(!m_bFullScreen);
    }
    
    if(m_bFullScreen)
    {
        m_bFullScreen = false;
        ui->actionFull_screen_F->setIcon(QIcon(":/image/FullScreen"));
        ui->actionFull_screen_F->setText(tr("Full screen(&F)"));
        ui->actionFull_screen_F->setToolTip(tr("Full screen"));
        ui->actionFull_screen_F->setStatusTip(tr("Full screen"));
        ui->actionFull_screen_F->setWhatsThis(tr("Full screen"));

        ui->toolBar->setVisible(true);
        //ui->toolBar->setAllowedAreas(Qt::AllToolBarAreas);
        ui->toolBar->setVisible(ui->actionToolBar_T->isChecked());

        ui->statusbar->setVisible(ui->actionStatusBar_S->isChecked());
        ui->menubar->setVisible(true);
        if(m_pFullScreenToolBar)
        {
            m_pFullScreenToolBar->close();
            m_pFullScreenToolBar = nullptr;
        }
        
        emit sigShowNormal();
        this->showNormal();
        this->activateWindow();
        
        return;
    }

    m_bFullScreen = true;
    emit sigFullScreen();
    //setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    this->showFullScreen();
   
    ui->actionFull_screen_F->setIcon(QIcon(":/image/ExitFullScreen"));
    ui->actionFull_screen_F->setText(tr("Exit full screen(&E)"));
    ui->actionFull_screen_F->setToolTip(tr("Exit full screen"));
    ui->actionFull_screen_F->setStatusTip(tr("Exit full screen"));
    ui->actionFull_screen_F->setWhatsThis(tr("Exit full screen"));

    ui->toolBar->setVisible(false);
    //ui->toolBar->setAllowedAreas(Qt::NoToolBarArea);

    ui->statusbar->setVisible(false);
    ui->menubar->setVisible(false);

    if(m_pFullScreenToolBar) m_pFullScreenToolBar->close();
    m_pFullScreenToolBar = new CFrmFullScreenToolBar(this);
    m_pFullScreenToolBar->move((qApp->primaryScreen()->geometry().width()
               - m_pFullScreenToolBar->frameGeometry().width()) / 2, 0);
    bool check = connect(m_pFullScreenToolBar, SIGNAL(sigExitFullScreen()),
                         this, SLOT(on_actionFull_screen_F_triggered()));
    Q_ASSERT(check);
    check = connect(m_pFullScreenToolBar, SIGNAL(sigExit()),
                    this, SLOT(on_actionExit_E_triggered()));
    Q_ASSERT(check);
    check = connect(m_pFullScreenToolBar, SIGNAL(sigDisconnect()), 
                    this, SLOT(on_actionDisconnect_D_triggered()));
    Q_ASSERT(check);
    
    CViewTable* p = dynamic_cast<CViewTable*>(pTab);
    if(p)
    {
        check = connect(m_pFullScreenToolBar, SIGNAL(sigShowTabBar(bool)),
                        SLOT(slotShowTabBar(bool)));
        Q_ASSERT(check);
    }
    m_pFullScreenToolBar->show();
}

void MainWindow::on_actionZoom_window_to_remote_desktop_triggered()
{
    LOG_MODEL_DEBUG("MainWindow", "main window:%d,%d", 
                    frameGeometry().width(),
                    frameGeometry().height());
    if(!m_pView) return;
    QSize s = m_pView->GetDesktopSize();
    LOG_MODEL_DEBUG("MainWindow", "size:%d,%d", 
                    s.width(),
                    s.height());
    resize(s);
}

void MainWindow::on_actionZoomToWindow_Z_triggered()
{
    if(!m_pView) return;
    m_pView->SetAdaptWindows(CFrmViewer::ZoomToWindow);
    if(m_ptbZoom)
        m_ptbZoom->setIcon(ui->actionZoomToWindow_Z->icon());
}

void MainWindow::on_actionKeep_aspect_ration_to_windows_K_toggled(bool arg1)
{
    if(!arg1) return;
    if(!m_pView) return;
    m_pView->SetAdaptWindows(CFrmViewer::KeepAspectRationToWindow);
    if(m_ptbZoom)
        m_ptbZoom->setIcon(ui->actionKeep_aspect_ration_to_windows_K->icon());
}

void MainWindow::on_actionOriginal_O_changed()
{
    ui->actionZoom_window_to_remote_desktop->setEnabled(ui->actionOriginal_O->isChecked());
}

void MainWindow::on_actionOriginal_O_triggered()
{
    qDebug() << "on_actionOriginal_O_triggered()";
    if(!m_pView) return;
    m_pView->SetAdaptWindows(CFrmViewer::Original);
    if(m_psbZoomFactor)
        m_psbZoomFactor->setValue(m_pView->GetZoomFactor() * 100);
    if(m_ptbZoom)
        m_ptbZoom->setIcon(ui->actionOriginal_O->icon());
}

void MainWindow::on_actionZoom_In_triggered()
{
    if(!m_pView) return;
    m_pView->slotZoomIn();
    if(m_psbZoomFactor)
        m_psbZoomFactor->setValue(m_pView->GetZoomFactor() * 100);
    m_pView->SetAdaptWindows(CFrmViewer::Zoom);
    if(m_ptbZoom)
        m_ptbZoom->setIcon(ui->actionZoom_In->icon());
}

void MainWindow::on_actionZoom_Out_triggered()
{
    if(!m_pView) return;
    m_pView->slotZoomOut();
    if(m_psbZoomFactor)
        m_psbZoomFactor->setValue(m_pView->GetZoomFactor() * 100);
    m_pView->SetAdaptWindows(CFrmViewer::Zoom);
    if(m_ptbZoom)
        m_ptbZoom->setIcon(ui->actionZoom_Out->icon());
}

void MainWindow::slotZoomFactor(int v)
{
    m_pView->slotZoomFactor(((double)v) / 100);
    m_pView->SetAdaptWindows(CFrmViewer::Zoom);
}

void MainWindow::slotAdaptWindows(const CFrmViewer::ADAPT_WINDOWS aw)
{
    if(!m_pView)
        return;
    if(m_pGBView)
        m_pGBView->setEnabled(true);
    if(m_ptbZoom)
        m_ptbZoom->setEnabled(true);
    if(m_psbZoomFactor)
        m_psbZoomFactor->setEnabled(true);
    switch (aw) {
    case CFrmViewer::Auto:
    case CFrmViewer::Original:
        ui->actionOriginal_O->setChecked(true);
        if(m_ptbZoom)
            m_ptbZoom->setIcon(ui->actionOriginal_O->icon());
        break;
    case CFrmViewer::ZoomToWindow:
        ui->actionZoomToWindow_Z->setChecked(true);
        if(m_ptbZoom)
            m_ptbZoom->setIcon(ui->actionZoomToWindow_Z->icon());
        break;
    case CFrmViewer::KeepAspectRationToWindow:
        ui->actionKeep_aspect_ration_to_windows_K->setChecked(true);
        if(m_ptbZoom)
            m_ptbZoom->setIcon(ui->actionKeep_aspect_ration_to_windows_K->icon());
        break;
    case CFrmViewer::Zoom:
        ui->actionZoom_Out->setChecked(true);
        if(m_ptbZoom)
            m_ptbZoom->setIcon(QIcon(":/image/Zoom"));
        break;
    case CFrmViewer::Disable:
        if(m_ptbZoom)
            m_ptbZoom->setIcon(QIcon(":/image/Zoom"));
        if(m_pGBView)
            m_pGBView->setEnabled(false);
        if(m_ptbZoom)
            m_ptbZoom->setEnabled(false);
        if(m_psbZoomFactor)
            m_psbZoomFactor->setEnabled(false);
        break;
    default:
        break;
    }
}

void MainWindow::on_actionExit_E_triggered()
{
    close();
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Escape:
        if(isFullScreen())
            on_actionFull_screen_F_triggered();
        break;
    }
}

void MainWindow::slotRecentFileTriggered(const QString& szFile)
{
    CConnecter* p = m_ManageConnecter.LoadConnecter(szFile);
    if(nullptr == p)
    {
        slotInformation(tr("Load file fail: ") + szFile);
        return;
    }

    Connect(p, false);
}

void MainWindow::on_actionOpen_O_triggered()
{
    QString szFile = RabbitCommon::CDir::GetOpenFileName(this,
                     tr("Open rabbit remote control file"),
                     RabbitCommon::CDir::Instance()->GetDirUserData(), 
                     tr("Rabbit remote control Files (*.rrc);;All files(*.*)"));
    if(szFile.isEmpty()) return;

    CConnecter* p = m_ManageConnecter.LoadConnecter(szFile);
    if(nullptr == p)
    {
        slotInformation(tr("Load file fail: ") + szFile);
        return;
    }

    Connect(p, true);
}

void MainWindow::slotConnect()
{
    if(nullptr == m_pView)
    {
        Q_ASSERT(false);
        return;
    }
    QAction* pAction = dynamic_cast<QAction*>(this->sender());    
    CConnecter* p = m_ManageConnecter.CreateConnecter(pAction->data().toString());
    if(nullptr == p) return;

    Connect(p, true);
}

int MainWindow::Connect(CConnecter *p, bool set)
{
    bool check = connect(p, SIGNAL(sigConnected()),
                         this, SLOT(slotConnected()));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigDisconnected()),
                             this, SLOT(slotDisconnected()));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigError(const int, const QString &)),
                    this, SLOT(slotError(const int, const QString&)));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigInformation(const QString&)),
                    this, SLOT(slotInformation(const QString&)));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigUpdateName(const QString&)),
                    this, SLOT(slotUpdateServerName(const QString&)));
    Q_ASSERT(check);
        
    if(set)
    {
        int nRet = p->OpenDialogSettings(this);
        switch(nRet)
        {
        case QDialog::Rejected:
            delete p;
            return 0;
        case QDialog::Accepted:
            break;
        }
    }

    QString szFile = RabbitCommon::CDir::Instance()->GetDirUserData()
            + QDir::separator()
            + p->Id()
            + ".rrc";
    
    int nRet = m_ManageConnecter.SaveConnecter(szFile, p);
    if(0 == nRet)
        m_pRecentMenu->addRecentFile(szFile, p->Name());

    if(!p->Name().isEmpty())
        slotInformation(tr("Connecting to ") + p->Name());
    if(m_pView)
    {
        m_pView->SetAdaptWindows(CFrmViewer::Auto, p->GetViewer());
        m_pView->AddView(p->GetViewer());
        m_pView->SetWidowsTitle(p->GetViewer(), p->Name()); 
    }
    
    m_Connecters.push_back(p);
    
    p->Connect();
    
    return 0;
}

void MainWindow::slotConnected()
{
    CConnecter* p = dynamic_cast<CConnecter*>(sender());
    if(!p) return;

//    if(m_pView)
//    {
//        m_pView->AddView(p->GetViewer());
//    }
//    m_Connecters.push_back(p);

    slotInformation(tr("Connected to ") + p->Name());
}

void MainWindow::slotCloseView(const QWidget* pView)
{
    if(!pView) return;
    foreach(auto c, m_Connecters)
    {
        if(c->GetViewer() == pView)
        {
            //TODO: Whether to save the setting
            c->Save();
            c->DisConnect();
        }
    }
}

void MainWindow::on_actionDisconnect_D_triggered()
{
    if(!m_pView) return;
    
    QWidget* pView = m_pView->GetCurrentView();
    slotCloseView(pView);    
}

void MainWindow::slotDisconnected()
{
    CConnecter* pConnecter = dynamic_cast<CConnecter*>(sender());
    foreach(auto c, m_Connecters)
    {
        if(c == pConnecter)
        {
            m_pView->RemoveView(c->GetViewer());
            m_Connecters.removeAll(c);
            c->deleteLater();
            return;
        }
    }
}

void MainWindow::slotError(const int nError, const QString &szInfo)
{
    Q_UNUSED(nError);
    this->statusBar()->showMessage(szInfo);
}

void MainWindow::slotInformation(const QString& szInfo)
{
    this->statusBar()->showMessage(szInfo);
}

void MainWindow::slotUpdateServerName(const QString& szName)
{
    CConnecter* pConnecter = dynamic_cast<CConnecter*>(sender());
    if(!pConnecter) return;
    m_pView->SetWidowsTitle(pConnecter->GetViewer(), szName);
}

void MainWindow::on_actionOpenStyle_O_triggered()
{
    RabbitCommon::CStyle::Instance()->slotStyle();
}

void MainWindow::on_actionDefaultStyle_D_triggered()
{
    RabbitCommon::CStyle::Instance()->slotSetDefaultStyle();
}

int MainWindow::onProcess(const QString &id, CPluginViewer *pFactory)
{
    Q_UNUSED(id);
    // Connect menu and toolbar
    QAction* p = ui->menuConnect_C->addAction(pFactory->Protol()
                                              + ": " + pFactory->DisplayName(),
                                              this, SLOT(slotConnect()));
    p->setToolTip(pFactory->Description());
    p->setStatusTip(pFactory->Description());
    p->setData(id);
    p->setIcon(pFactory->Icon());

    return 0;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    qDebug() << "MainWindow::closeEvent()";
    foreach (auto it, m_Connecters)
        it->DisConnect();
}

void MainWindow::on_actionSend_ctl_alt_del_triggered()
{
    if(m_pView)
        m_pView->slotSystemCombination();
}

void MainWindow::on_actionShow_TabBar_B_triggered()
{
    slotShowTabBar(ui->actionShow_TabBar_B->isChecked());
}

void MainWindow::slotShowTabBar(bool bShow)
{
    CViewTable* p = dynamic_cast<CViewTable*>(m_pView);
    if(p)
        p->ShowTabBar(bShow);
}

void MainWindow::on_actionScreenshot_triggered()
{
    if(!m_pView || !m_pView->GetCurrentView()) return;
    /*
    QString szFile = RabbitCommon::CDir::GetSaveFileName(this,
                                          tr("Open save screenslot file"),
                              RabbitCommon::CDir::Instance()->GetDirUserImage(), 
                             tr("PNG(*.png);;JEPG(*.jpg);;All files(*.*)"));//*/
    QString szFile = RabbitCommon::CDir::Instance()->GetDirUserImage()
            + QDir::separator()
            + "RabbitRemoteControl_ScreenSlot_"
            + QDateTime::currentDateTime().toLocalTime().toString("yyyy_MM_dd_hh_mm_ss_zzz")
            + ".png";
    if(szFile.isEmpty()) return;
    int nRet = m_pView->Screenslot(szFile, m_Parameter.GetScreenShot());
    if(0 == nRet)
    {
        slotInformation(tr("Save screenslot to ") + szFile);
        switch (m_Parameter.GetScreenShotEndAction()) {
        case CParameterApp::OpenFile:
            QDesktopServices::openUrl(QUrl::fromLocalFile(szFile));
            break;
        case CParameterApp::OpenFolder:
        {
            QFileInfo f(szFile);
            QString path = f.absolutePath();
            QUrl url = QUrl::fromLocalFile(path);
            QDesktopServices::openUrl(url);
            break;
        }
        default:
            break;
        }
    }
    return ;
}

void MainWindow::on_actionSettings_triggered()
{
    CDlgSettings set(&m_Parameter, this);
    if(CDlgSettings::Accepted == set.exec())
        m_Parameter.Save();
}

void MainWindow::slotShortCut()
{
    if(m_Parameter.GetReceiveShortCut())
    {
        setFocusPolicy(Qt::WheelFocus);
        if(m_psbZoomFactor)
            m_psbZoomFactor->setFocusPolicy(Qt::WheelFocus);
        if(m_ptbZoom)
            m_ptbZoom->setFocusPolicy(Qt::WheelFocus);
        ui->actionFull_screen_F->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R, Qt::Key_F));
        ui->actionScreenshot->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R, Qt::Key_S));
        ui->actionZoom_In->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R, Qt::Key_Plus));
        ui->actionZoom_Out->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R, Qt::Key_Minus));
        ui->actionOriginal_O->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R, Qt::Key_O));
        ui->actionZoomToWindow_Z->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R, Qt::Key_W));
    } else {
        setFocusPolicy(Qt::NoFocus);
        if(m_psbZoomFactor)
            m_psbZoomFactor->setFocusPolicy(Qt::NoFocus);
        if(m_ptbZoom)
            m_ptbZoom->setFocusPolicy(Qt::NoFocus);
        ui->actionFull_screen_F->setShortcut(QKeySequence());
        ui->actionScreenshot->setShortcut(QKeySequence());
        ui->actionZoom_In->setShortcut(QKeySequence());
        ui->actionZoom_Out->setShortcut(QKeySequence());
        ui->actionOriginal_O->setShortcut(QKeySequence());
        ui->actionZoomToWindow_Z->setShortcut(QKeySequence());
    }
}
