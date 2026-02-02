// Author: Kang Lin <kl222@126.com>

#include <QFontMetrics>
#include <QGridLayout>
#include <QMessageBox>
#include <QCheckBox>
#include <QScreen>
#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QWidgetAction>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QKeySequence>
#include <QPushButton>
#include <QDateTime>
#include <QFileDialog>
#include <QLoggingCategory>
#include <QThread>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#ifdef HAVE_UPDATE
    #include "FrmUpdater.h"
#endif
#include "RabbitCommonDir.h"
#include "RabbitCommonTools.h"

#ifdef HAVE_ABOUT
    #include "DlgAbout.h"
#endif
#ifdef BUILD_QUIWidget
    #include "QUIWidget/QUIWidget.h"
#endif

#include "Operate.h"
#include "FrmFullScreenToolBar.h"
#include "ParameterDlgSettings.h"

#include "ViewTable.h"
#include "ViewSplitter.h"

#ifdef HAVE_ICE
    #include "Ice.h"
#endif

static Q_LOGGING_CATEGORY(log, "App.MainWindow")
static Q_LOGGING_CATEGORY(logRecord, "App.MainWindow.Record")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_pToolBarMenuAction(nullptr)
    , m_pMenuActivityGroup(nullptr)
    , m_ptbMenuActivity(nullptr)
    , m_pActionOperateMenu(nullptr)
    , m_pTBOperate(nullptr)
    , m_pActionTBOperate(nullptr)
    , m_pDockRecent(nullptr)
    , m_pRecent(nullptr)
    , m_pRecentDb(nullptr)
    , m_pDockActive(nullptr)
    , m_pFrmActive(nullptr)
    , m_pSecureLevel(nullptr)
    , m_pSignalStatus(nullptr)
    , ui(new Ui::MainWindow)
    , m_pView(nullptr)
    , m_pFullScreenToolBar(nullptr)
    , m_pRecentMenu(nullptr)
    , m_pDockFavorite(nullptr)
    , m_pFavoriteView(nullptr)
    , m_StatusBarMessage(this)
{
    bool check = false;

    ui->setupUi(this);
    ui->menubar->show();

    check = connect(&m_Manager, &CManager::sigNewOperate,
                    this, [&](COperate* pOperate, bool bOpenSettingsDialog){
        Start(pOperate, bOpenSettingsDialog);
    });
    Q_ASSERT(check);

#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID) && !defined(Q_OS_MACOS)
    // gnome icon isn't support svg
    setWindowIcon(QIcon(":/images/app"));
#endif

    m_StatusBarMessage.setSizePolicy(QSizePolicy::Policy::Expanding,
                                  QSizePolicy::Policy::Fixed);
    //m_StatusBarMessage.setWordWrap(true);
    this->statusBar()->addWidget(&m_StatusBarMessage);

    m_SendRate.setToolTip(tr("Send rate"));
    m_SendRate.setStatusTip(tr("Send rate"));
    // m_SendRate.setSizePolicy(QSizePolicy::Policy::Preferred,
    //                          QSizePolicy::Policy::Fixed);
    this->statusBar()->addPermanentWidget(&m_SendRate);
    m_ReceivesRate.setToolTip(tr("Receives rate"));
    m_ReceivesRate.setStatusTip(tr("Receives rate"));
    // m_ReceivesRate.setSizePolicy(QSizePolicy::Policy::Preferred,
    //                              QSizePolicy::Policy::Fixed);
    this->statusBar()->addPermanentWidget(&m_ReceivesRate);
    m_TotalSends.setToolTip(tr("Total sends"));
    m_TotalSends.setStatusTip(tr("Total sends"));
    // m_TotalSends.setSizePolicy(QSizePolicy::Policy::Preferred,
    //                            QSizePolicy::Policy::Fixed);
    this->statusBar()->addPermanentWidget(&m_TotalSends);
    m_TotalReceives.setToolTip(tr("Total receives"));
    m_TotalReceives.setStatusTip(tr("Total receives"));
    // m_TotalReceives.setSizePolicy(QSizePolicy::Policy::Preferred,
    //                            QSizePolicy::Policy::Fixed);
    this->statusBar()->addPermanentWidget(&m_TotalReceives);

    check = connect(&m_Timer, SIGNAL(timeout()), this, SLOT(slotTimeOut()));
    Q_ASSERT(check);

    m_pSecureLevel = new QLabel(statusBar());
    // QIcon icon = QIcon::fromTheme("network-wired");
    // QPixmap pixmap = icon.pixmap(icon.actualSize(QSize(64, 64)));
    // m_pSecureLevel->setPixmap(pixmap);
    m_pSecureLevel->hide();
    statusBar()->addPermanentWidget(m_pSecureLevel);

    //setFocusPolicy(Qt::NoFocus);
    //addToolBar(Qt::LeftToolBarArea, ui->toolBar);
    setAcceptDrops(true);

    RabbitCommon::CTools::AddStyleMenu(ui->menuTools);
    ui->menuTools->addMenu(RabbitCommon::CTools::GetLogMenu(this));

    m_pRecentMenu = new RabbitCommon::CRecentMenu(
        tr("Recently"), QIcon::fromTheme("document-open-recent"), false, this);
    check = connect(m_pRecentMenu, SIGNAL(recentFileTriggered(const QString&)),
                    this, SLOT(slotOpenFile(const QString&)));
    Q_ASSERT(check);
    check = connect(&m_Parameter, SIGNAL(sigRecentMenuMaxCountChanged(int)),
                    m_pRecentMenu, SLOT(setMaxCount(int)));
    Q_ASSERT(check);
    QAction* pRecentAction = ui->menuOperate->insertMenu(
        ui->actionOpenListRecent, m_pRecentMenu);
    pRecentAction->setStatusTip(pRecentAction->text());
    QToolButton* tbRecent = new QToolButton(ui->toolBar);
    tbRecent->setFocusPolicy(Qt::NoFocus);
    tbRecent->setPopupMode(QToolButton::InstantPopup);
    tbRecent->setMenu(m_pRecentMenu);
    tbRecent->setIcon(pRecentAction->icon());
    tbRecent->setText(pRecentAction->text());
    tbRecent->setToolTip(pRecentAction->toolTip());
    tbRecent->setStatusTip(pRecentAction->statusTip());
    ui->toolBar->insertWidget(ui->actionOpenListRecent, tbRecent);

#ifdef HAVE_UPDATE
    CFrmUpdater updater;
    ui->actionUpdate->setIcon(updater.windowIcon());
#endif

    QToolButton* tbStart = new QToolButton(ui->toolBar);
    tbStart->setFocusPolicy(Qt::NoFocus);
    tbStart->setPopupMode(QToolButton::InstantPopup);
    //tbStart->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    tbStart->setMenu(ui->menuStart);
    tbStart->setIcon(QIcon::fromTheme("media-playback-start"));
    tbStart->setText(tr("Start"));
    tbStart->setToolTip(tr("Start"));
    tbStart->setStatusTip(tr("Start"));
    ui->toolBar->insertWidget(ui->actionStop, tbStart);

    m_Parameter.Load();

    check = connect(&m_Parameter, SIGNAL(sigReceiveShortCutChanged()),
                    this, SLOT(slotShortCut()));
    Q_ASSERT(check);
    slotSystemTrayIconTypeChanged();
    check = connect(&m_Parameter, SIGNAL(sigSystemTrayIconTypeChanged()),
                    this,
                    SLOT(slotSystemTrayIconTypeChanged()));
    Q_ASSERT(check);
    check = connect(&m_Parameter, SIGNAL(sigEnableSystemTrayIcon()),
                    this, SLOT(slotEnableSystemTrayIcon()));
    Q_ASSERT(check);
    check = connect(&m_Parameter, SIGNAL(sigEnableTabToolTipChanged()),
                    this, SLOT(slotUpdateName()));
    Q_ASSERT(check);
    check = connect(&m_Parameter, SIGNAL(sigEnableTabIconChanged()),
                    this, SLOT(slotUpdateName()));
    Q_ASSERT(check);

    auto pTbMenu = new QToolButton(ui->toolBar);
    pTbMenu->setFocusPolicy(Qt::NoFocus);
    pTbMenu->setPopupMode(QToolButton::InstantPopup);
    //m_pTbMenu->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    pTbMenu->setText(ui->actionMain_menu_bar_M->text());
    pTbMenu->setIcon(ui->actionMain_menu_bar_M->icon());
    pTbMenu->setToolTip(ui->actionMain_menu_bar_M->toolTip());
    pTbMenu->setStatusTip(ui->actionMain_menu_bar_M->statusTip());
    QMenu *pMenu = new QMenu(pTbMenu);
    pMenu->addActions(this->menuBar()->actions());
    pTbMenu->setMenu(pMenu);
    m_pToolBarMenuAction = ui->toolBar->insertWidget(ui->actionTabBar_B, pTbMenu);
#if defined(Q_OS_ANDROID)
    m_pToolBarMenuAction->setVisible(true);
#else
    m_pToolBarMenuAction->setVisible(false);
#endif

    check = connect(&m_Parameter, SIGNAL(sigStartByTypeChanged()),
                    this, SLOT(slotStartByType()));
    Q_ASSERT(check);

    m_pDockFavorite = new QDockWidget(this);
    if(m_pDockFavorite)
    {
        m_pFavoriteView = new CFavoriteView(m_pDockFavorite);
        if(m_pFavoriteView)
        {
            if(m_pFavoriteView->m_pDockTitleBar)
                m_pDockFavorite->setTitleBarWidget(m_pFavoriteView->m_pDockTitleBar);
            check = connect(m_pFavoriteView, SIGNAL(sigStart(const QString&, bool)),
                            this, SLOT(slotOpenFile(const QString&, bool)));
            Q_ASSERT(check);
            check = connect(m_pFavoriteView, SIGNAL(sigFavorite()),
                            this, SLOT(on_actionAdd_to_favorite_triggered()));
            Q_ASSERT(check);
            check = connect(&m_Parameter, SIGNAL(sigFavoriteEditChanged(bool)),
                            m_pFavoriteView, SLOT(slotDoubleEditNode(bool)));
            Q_ASSERT(check);
            emit m_Parameter.sigFavoriteEditChanged(m_Parameter.GetFavoriteEdit());
            m_pDockFavorite->setWidget(m_pFavoriteView);
            m_pDockFavorite->setWindowTitle(m_pFavoriteView->windowTitle());
        }
        // Must set ObjectName then restore it. See: saveState help document
        m_pDockFavorite->setObjectName("dockFavorite");
#if defined(Q_OS_ANDROID)
        m_pDockFavorite->hide();
#endif
        ui->menuView->addAction(m_pDockFavorite->toggleViewAction());
        m_pDockFavorite->toggleViewAction()->setIcon(QIcon::fromTheme("emblem-favorite"));
        addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, m_pDockFavorite);
    }

    m_pRecentDb = new CRecentDatabase(this);
    m_pDockRecent = new QDockWidget(this);
    if(m_pDockRecent)
    {
        m_pRecent
            = new CFrmRecent(this, &m_Manager, m_pRecentDb, m_Parameter, true,
                                 m_pDockRecent);
        if(m_pRecent) {
            if(m_pRecent->m_pDockTitleBar)
                m_pDockRecent->setTitleBarWidget(
                    m_pRecent->m_pDockTitleBar);
            check = connect(m_pRecent,
                            SIGNAL(sigStart(const QString&, bool)),
                            this, SLOT(slotOpenFile(const QString&, bool)));
            Q_ASSERT(check);
            check = connect(m_pRecent, &CFrmRecent::sigAddToFavorite,
                            m_pFavoriteView, &CFavoriteView::slotAddToFavorite);
            Q_ASSERT(check);
            m_pDockRecent->setWidget(m_pRecent);
            m_pDockRecent->setWindowTitle(
                m_pRecent->windowTitle());
        }
        // Must set ObjectName then restore it. See: saveState help document
        m_pDockRecent->setObjectName("dockRecent");
#if defined(Q_OS_ANDROID)
        m_pDockRecent->hide();
#endif
        ui->menuView->addAction(m_pDockRecent->toggleViewAction());
        m_pDockRecent->toggleViewAction()->setIcon(QIcon::fromTheme("document-open-recent"));
        tabifyDockWidget(m_pDockFavorite, m_pDockRecent);
    }

    m_pDockActive = new QDockWidget(this);
    if(m_pDockActive)
    {
        m_pFrmActive = new CFrmActive(
            m_Operates, m_Parameter,
            ui->menuStart, ui->actionStop,
            m_pRecentMenu, m_pDockActive);
        if(m_pFrmActive) {
            m_pDockActive->setWidget(m_pFrmActive);
            if(m_pFrmActive->m_pDockTitleBar)
                m_pDockActive->setTitleBarWidget(
                    m_pFrmActive->m_pDockTitleBar);
            m_pDockActive->setWindowTitle(m_pFrmActive->windowTitle());
            check = connect(m_pFrmActive, SIGNAL(sigChanged(COperate*)),
                            this, SLOT(slotOperateChanged(COperate*)));
            Q_ASSERT(check);
            check = connect(m_pFrmActive, &CFrmActive::sigAddToFavorite,
                            m_pFavoriteView, &CFavoriteView::slotAddToFavorite);
            Q_ASSERT(check);
        }
        m_pDockActive->setObjectName("dockListActive");
#if defined(Q_OS_ANDROID)
        m_pDockActive->hide();
#endif
        ui->menuView->addAction(m_pDockActive->toggleViewAction());
        m_pDockActive->toggleViewAction()->setIcon(QIcon::fromTheme("network-wired"));
        tabifyDockWidget(m_pDockFavorite, m_pDockActive);
    }

    QActionGroup* pGBView = new QActionGroup(this);
    if(pGBView) {
        pGBView->addAction(ui->actionViewTab);
        pGBView->addAction(ui->actionViewSplit);
    }
    qDebug(log) << "View type:" << m_Parameter.GetViewType();
    if(CParameterApp::ViewType::Tab == m_Parameter.GetViewType()) {
        ui->actionViewTab->setChecked(true);
        on_actionViewTab_triggered();
    }
    if(CParameterApp::ViewType::Splitter == m_Parameter.GetViewType()) {
        ui->actionViewSplit->setChecked(true);
        on_actionViewSplit_triggered();
    }
    check = connect(&m_Parameter, &CParameterApp::sigViewTypeChanged,
                    this, [&](){
                        m_Parameter.Save();
                    });
    Q_ASSERT(check);

    m_pMenuActivityGroup = new QActionGroup(ui->menuActivity);
    m_ptbMenuActivity = new QToolButton(ui->toolBar);
    m_ptbMenuActivity->setFocusPolicy(Qt::NoFocus);
    m_ptbMenuActivity->setPopupMode(QToolButton::InstantPopup);
    m_ptbMenuActivity->setMenu(ui->menuActivity);
    m_ptbMenuActivity->setIcon(ui->menuActivity->icon());
    m_ptbMenuActivity->setText(ui->menuActivity->title());
    m_ptbMenuActivity->setToolTip(ui->menuActivity->toolTip());
    m_ptbMenuActivity->setStatusTip(ui->menuActivity->statusTip());
    ui->toolBar->insertWidget(ui->actionTabBar_B, m_ptbMenuActivity);

    EnableMenu(false);
    slotShortCut();

#ifdef HAVE_ICE
    if(CICE::Instance()->GetSignal())
    {
        check = connect(CICE::Instance()->GetSignal().data(),
                        SIGNAL(sigConnected()),
                        this, SLOT(slotSignalConnected()));
        Q_ASSERT(check);
        check = connect(CICE::Instance()->GetSignal().data(),
                        SIGNAL(sigDisconnected()),
                        this, SLOT(slotSignalDisconnected()));
        Q_ASSERT(check);
        check = connect(CICE::Instance()->GetSignal().data(),
                        SIGNAL(sigError(const int, const QString&)),
                        this, SLOT(slotSignalError(const int, const QString&)));
        Q_ASSERT(check);
    }
    CICE::Instance()->slotStart(statusBar());
    m_pSignalStatus = new QPushButton();
    m_pSignalStatus->setToolTip(tr("ICE signal status"));
    m_pSignalStatus->setStatusTip(m_pSignalStatus->toolTip());
    m_pSignalStatus->setWhatsThis(m_pSignalStatus->toolTip());
    slotSignalDisconnected();
    statusBar()->addPermanentWidget(m_pSignalStatus);
#endif

    if(m_Parameter.GetSaveMainWindowStatus())
    {
        QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                      QSettings::IniFormat);
        QByteArray geometry
            = set.value("MainWindow/Status/Geometry").toByteArray();
        if(!geometry.isEmpty())
            restoreGeometry(geometry);
        // Restores the state of this mainwindow's toolbars and dockwidgets
        QByteArray state = set.value("MainWindow/Status/State").toByteArray();
        if(!state.isEmpty())
            restoreState(state);

        ui->actionTabBar_B->setChecked(m_Parameter.GetTabBar());
        on_actionTabBar_B_toggled(m_Parameter.GetTabBar());
        ui->actionStatus_bar_S->setChecked(m_Parameter.GetStatusBar());
        statusBar()->setVisible(m_Parameter.GetStatusBar());
        ui->actionMain_menu_bar_M->setChecked(m_Parameter.GetMenuBar());
        menuBar()->setVisible(m_Parameter.GetMenuBar());
        ui->actionToolBar_T->setChecked(!ui->toolBar->isHidden());
        // macOS: Hide 'Main menu bar' on macOS since it's always visible
        #ifdef Q_OS_MACOS
        // Hide 'Main menu bar' on View menu
        ui->actionMain_menu_bar_M->setVisible(false);
        #else
        // Show 'Main menu bar' toolbar icon
        if(!m_Parameter.GetMenuBar()) {
            m_pToolBarMenuAction->setVisible(true);
        }
        #endif
    }
}

MainWindow::~MainWindow()
{
    qDebug(log) << "MainWindow::~MainWindow()";
    if(m_pFullScreenToolBar) m_pFullScreenToolBar->close();
    delete ui;
}

//! For time-consuming operations
void MainWindow::slotInitial()
{
    qDebug(log) << Q_FUNC_INFO;

    setEnabled(false);

    slotInformation(tr("Load plugins ......"));
    qApp->processEvents();
    m_Manager.Initial();
    m_Manager.EnumPlugins(this);

    if(m_pRecentDb)
        m_pRecentDb->OpenDatabase("recent_connect");

    if(m_pRecentMenu) {
        slotInformation(tr("Load recent menu ......"));
        qApp->processEvents();
        auto recents = m_pRecentDb->GetRecents(m_Parameter.GetRecentMenuMaxCount());
        //qDebug(log) << "recents totaol:" << recents.size() << m_Parameter.GetRecentMenuMaxCount();
        for(auto it = recents.rbegin(); it != recents.rend(); it++) {
            m_pRecentMenu->addRecentFile(it->szFile, it->szName, it->icon);
        }
    }

    if(m_pRecent) {
        slotInformation(tr("Load list recent dock ......"));
        qApp->processEvents();
        m_pRecent->Init();
    }

    if(m_pFavoriteView) {
        slotInformation(tr("Load favorite ......"));
        qApp->processEvents();
        m_pFavoriteView->Initial();
    }

    slotEnableSystemTrayIcon();

    setEnabled(true);

    slotInformation(tr("Load laster operate ......"));
    qApp->processEvents();
    LoadOperateLasterClose();

    slotInformation(tr("Ready"));

    return;
}

void MainWindow::SetView(CView* pView)
{
    qDebug(log) << Q_FUNC_INFO;
    if(!pView)
        return;
    if(m_pView)
        m_pView->disconnect();

    m_pView = pView;
    setCentralWidget(m_pView);

    bool check = false;
    //m_pView->setFocusPolicy(Qt::NoFocus);
    check = connect(m_pView, SIGNAL(sigCloseView(const QWidget*)),
                    this, SLOT(slotCloseView(const QWidget*)));
    Q_ASSERT(check);
    check = connect(m_pView, SIGNAL(sigCurrentChanged(const QWidget*)),
                    this, SLOT(slotCurrentViewChanged(const QWidget*)));
    Q_ASSERT(check);
    if(m_pFrmActive) {
        check = connect(m_pView, SIGNAL(sigCurrentChanged(const QWidget*)),
                        m_pFrmActive, SLOT(slotViewChanged(const QWidget*)));
        Q_ASSERT(check);
    }
    check = connect(m_pView, SIGNAL(customContextMenuRequested(const QPoint&)),
                    this, SLOT(slotCustomContextMenuRequested(const QPoint&)));
    Q_ASSERT(check);

    foreach (auto c, m_Operates) {
        m_pView->AddView(c->GetViewer());
        m_pView->SetWidowsTitle(
            c->GetViewer(), c->Name(), c->Icon(), c->Description());
    }
    m_pView->SetVisibleTab(m_Parameter.GetTabBar());
}

void MainWindow::on_actionViewTab_triggered()
{
    qDebug(log) << Q_FUNC_INFO;
    if(!ui->actionViewTab->isChecked()) return;
    m_Parameter.SetViewType(CParameterApp::ViewType::Tab);
    SetView(new CViewTable(&m_Parameter, this));
}

void MainWindow::on_actionViewSplit_triggered()
{
    qDebug(log) << Q_FUNC_INFO;
    if(!ui->actionViewSplit->isChecked()) return;
    m_Parameter.SetViewType(CParameterApp::ViewType::Splitter);
    SetView(new CViewSplitter(&m_Parameter, this));
}

void MainWindow::on_actionAbout_triggered()
{
#ifdef HAVE_ABOUT
    CDlgAbout *about = new CDlgAbout(this);
    QIcon icon = QIcon::fromTheme("app");
    if(!icon.isNull()) {
        auto sizeList = icon.availableSizes();
        if(!sizeList.isEmpty()) {
            QPixmap p = icon.pixmap(*sizeList.begin());
            about->m_AppIcon = p.toImage();
        }
    }
    about->m_szCopyrightStartTime = "2020";
    about->m_szVersionRevision = RabbitRemoteControl_REVISION;
    about->m_szDetails = m_Manager.Details();
    RC_SHOW_WINDOW(about);
#endif
}

void MainWindow::on_actionUpdate_triggered()
{
    if(!qEnvironmentVariable("SNAP").isEmpty()) {
        QDesktopServices::openUrl(QUrl("https://snapcraft.io/rabbitremotecontrol"));
    } else if(!qEnvironmentVariable("FLATPAK_ID").isEmpty()) {
        QDesktopServices::openUrl(QUrl("https://flathub.org/apps/io.github.KangLin.RabbitRemoteControl"));
    } else {
#ifdef HAVE_UPDATE
    CFrmUpdater* m_pfrmUpdater = new CFrmUpdater();
    QIcon icon = QIcon::fromTheme("app");
    if(!icon.isNull()) {
        auto sizeList = icon.availableSizes();
        if(!sizeList.isEmpty()) {
            QPixmap p = icon.pixmap(*sizeList.begin());
            m_pfrmUpdater->SetTitle(p.toImage());
        }
    }
    m_pfrmUpdater->SetInstallAutoStartup();
    RC_SHOW_WINDOW(m_pfrmUpdater);
#endif
    }
}

void MainWindow::on_actionFull_screen_F_triggered()
{
    CView* pView = qobject_cast<CView*>(this->centralWidget());
    if(pView)
    {
        pView->SetFullScreen(!isFullScreen());
    }

    if(isFullScreen())
    {
        qDebug(log) << "Exit full screen";
        ui->actionFull_screen_F->setIcon(QIcon::fromTheme("view-fullscreen"));
        ui->actionFull_screen_F->setText(tr("Full screen"));
        ui->actionFull_screen_F->setToolTip(tr("Full screen"));
        ui->actionFull_screen_F->setStatusTip(tr("Full screen"));
        ui->actionFull_screen_F->setWhatsThis(tr("Full screen"));

        ui->toolBar->setVisible(m_FullState.toolBar);
        ui->statusbar->setVisible(m_FullState.statusbar);
        ui->menubar->setVisible(m_FullState.menubar);

        m_pDockActive->setVisible(m_FullState.dockListActive);
        m_pDockRecent->setVisible(m_FullState.dockListRecent);
        m_pDockFavorite->setVisible(m_FullState.dockFavorite);
        // TODO: This is hade code. it is in RabbitCommon
        QDockWidget* pDockDebugLog = findChild<QDockWidget*>("dockDebugLog");
        if(pDockDebugLog)
        {
            pDockDebugLog->setVisible(m_FullState.dockDebugLog);
        }

        if(m_pFullScreenToolBar)
        {
            // Delete it when the widget is close
            m_pFullScreenToolBar->close();
            m_pFullScreenToolBar = nullptr;
        }

        emit sigShowNormal();
        this->showNormal();
        this->activateWindow();

        emit sigFullScreen(false);
        return;
    }

    qDebug(log) << "Entry full screen";

    //setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    this->showFullScreen();
    emit sigFullScreen(true);

    ui->actionFull_screen_F->setIcon(QIcon::fromTheme("view-restore"));
    ui->actionFull_screen_F->setText(tr("Exit full screen"));
    ui->actionFull_screen_F->setToolTip(tr("Exit full screen"));
    ui->actionFull_screen_F->setStatusTip(tr("Exit full screen"));
    ui->actionFull_screen_F->setWhatsThis(tr("Exit full screen"));

    m_FullState.toolBar = ui->toolBar->isVisible();
    ui->toolBar->setVisible(false);
    m_FullState.statusbar = ui->statusbar->isVisible();
    ui->statusbar->setVisible(false);
    m_FullState.menubar = ui->menubar->isVisible();
    ui->menubar->setVisible(false);

    m_FullState.dockListActive = m_pDockActive->isVisible();
    m_pDockActive->setVisible(false);
    m_FullState.dockListRecent = m_pDockRecent->isVisible();
    m_pDockRecent->setVisible(false);
    m_FullState.dockFavorite = m_pDockFavorite->isVisible();
    m_pDockFavorite->setVisible(false);
    // This is hade code. it is in RabbitCommon
    QDockWidget* pDockDebugLog = findChild<QDockWidget*>("dockDebugLog");
    if(pDockDebugLog)
    {
        m_FullState.dockDebugLog = pDockDebugLog->isVisible();
        pDockDebugLog->setVisible(false);
    }

    if(m_pFullScreenToolBar) m_pFullScreenToolBar->close();
    // Delete it when the widget is close
    m_pFullScreenToolBar = new CFrmFullScreenToolBar(this);
    QScreen* pScreen = qApp->primaryScreen();
    if(pScreen) {
        QPoint pos(pScreen->geometry().left()
                       + (pScreen->geometry().width()
                          - m_pFullScreenToolBar->frameGeometry().width()) / 2,
                   pScreen->geometry().top());
        qDebug(log) << "Primary screen geometry:" << pScreen->geometry()
                    << "availableGeometry:" << pScreen->availableGeometry()
                    << pos << mapToGlobal(pos);
        m_pFullScreenToolBar->move(pos);
    }
    bool check = connect(m_pFullScreenToolBar, SIGNAL(sigExitFullScreen()),
                         this, SLOT(on_actionFull_screen_F_triggered()));
    Q_ASSERT(check);
    check = connect(m_pFullScreenToolBar, SIGNAL(sigExit()),
                    this, SLOT(on_actionExit_E_triggered()));
    Q_ASSERT(check);
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigOperateMenuChanged(QAction*)),
                    m_pFullScreenToolBar,
                    SLOT(slotOperateMenuChanged(QAction*)));
    Q_ASSERT(check);

    m_pFullScreenToolBar->show();
}

void MainWindow::slotViewerFocusIn(QWidget *pView)
{
    COperate* c = qobject_cast<COperate*>(sender());
    qDebug(log) << Q_FUNC_INFO << "Focus:" << c << pView;
    if(c && m_pView) {
        m_pView->SetCurrentView(c->GetViewer());
    }
}

void MainWindow::slotOperateChanged(COperate *o)
{
    if(o && m_pView) {
        m_pView->SetCurrentView(o->GetViewer());
    }
}

void MainWindow::slotCurrentViewChanged(const QWidget* pView)
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_pView && pView)
        EnableMenu(true);
    else
        EnableMenu(false);

    foreach(auto o, m_Operates) {
        if(o->GetViewer() == pView) {
            SetSecureLevel(o);
            SetStatsVisible(o->GetStats());
            foreach (auto a, ui->menuActivity->actions()) {
                if(a->data().value<COperate*>() == o)
                    a->setChecked(true);
            }
        }
    }
}

void MainWindow::EnableMenu(bool bEnable)
{
    qDebug(log) << Q_FUNC_INFO << bEnable;
    ui->actionClone->setEnabled(bEnable);
    ui->actionAdd_to_favorite->setEnabled(bEnable);
    ui->actionStop->setEnabled(bEnable);
    ui->actionTabBar_B->setEnabled(bEnable);
    ui->menuActivity->setEnabled(bEnable);
    m_ptbMenuActivity->setEnabled(bEnable);
    slotLoadOperateMenu();
}

void MainWindow::slotMenuActivity()
{
    QAction* pAction = qobject_cast<QAction*>(sender());
    Q_ASSERT(pAction);
    if(!pAction) return;
    COperate* p = pAction->data().value<COperate*>();
    if(p)
        slotOperateChanged(p);
}

void MainWindow::slotLoadOperateMenu()
{
    qDebug(log) << Q_FUNC_INFO;

    if(m_pActionOperateMenu) {
        ui->menuOperate->removeAction(m_pActionOperateMenu);
        m_pActionOperateMenu = nullptr;
    }
    ui->toolBar->removeAction(m_pActionTBOperate);

    if(!m_pView)
        return;
    auto pWin = m_pView->GetCurrentView();
    if(!pWin) {
        qDebug(log) << "The current view is empty";
        return;
    }
    foreach(auto op, m_Operates)
    {
        if(op->GetViewer() == pWin)
        {
            qDebug(log) << "Load plugin menu";
            auto m = op->GetMenu(ui->menuTools);
            if(!m) return;
            // Menu tool bar
            m_pActionOperateMenu = ui->menuOperate->insertMenu(ui->actionAdd_to_favorite, m);
            // ToolBar
            if(!m_pTBOperate)
                m_pTBOperate = new QToolButton(ui->toolBar);
            if(m_pTBOperate) {
                m_pTBOperate->setFocusPolicy(Qt::NoFocus);
                m_pTBOperate->setPopupMode(QToolButton::InstantPopup);
                m_pTBOperate->setMenu(m);
                m_pTBOperate->setIcon(m->icon());
                m_pTBOperate->setText(m->title());
                m_pTBOperate->setToolTip(m->toolTip());
                m_pTBOperate->setStatusTip(m->statusTip());
                m_pTBOperate->show();
                m_pActionTBOperate = ui->toolBar->insertWidget(ui->actionAdd_to_favorite, m_pTBOperate);
            }
            emit sigOperateMenuChanged(m_pActionOperateMenu);
        }
    }
}

/*!
 * \brief MainWindow::slotCustomContextMenuRequested
 * \param pos: The view is converted to global coordinates
 * \see CViewSplitterContainer::slotCustomContextMenuRequested
 *      CViewTable::CViewTable
 */
void MainWindow::slotCustomContextMenuRequested(const QPoint &pos)
{
    if(!m_pView)
        return;
    auto pWin = m_pView->GetCurrentView();
    if(!pWin) {
        qDebug(log) << "The current view is empty";
        return;
    }
    foreach(auto p, m_Operates)
    {
        if(p->GetViewer() == pWin)
        {
            qDebug(log) << "Load plugin menu";
            auto m = p->GetMenu(ui->menuTools);
            if(!m) return;
            // Note: The view is converted to global coordinates
            m->exec(pos);
        }
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
    QMainWindow::keyReleaseEvent(event);
}

void MainWindow::slotUpdateParameters(COperate *pOperate)
{
    m_Manager.SaveOperate(pOperate);
}

void MainWindow::on_actionClone_triggered()
{
    if(!m_pView) return;
    QWidget* p = m_pView->GetCurrentView();
    foreach(auto pOperate, m_Operates)
    {
        if(pOperate->GetViewer() == p)
        {
            QString szFile = pOperate->GetSettingsFile();
            auto pOperate = m_Manager.LoadOperate(szFile);
            if(!pOperate) return;
            Start(pOperate, false, szFile);
            return;
        }
    }
}

void MainWindow::slotOpenFile(const QString& szFile, bool bOpenSettings)
{
    if(szFile.isEmpty()) return;
    COperate* p = m_Manager.LoadOperate(szFile);
    if(nullptr == p)
    {
        slotStatusMessage(tr("Load file fail: ") + szFile, MessageLevel::Error);
        return;
    }
    
    Start(p, bOpenSettings, szFile);
}

void MainWindow::on_actionOpenRRCFile_triggered()
{
    QString szFile = QFileDialog::getOpenFileName(
        this,
        tr("Open rabbit remote control file"),
        RabbitCommon::CDir::Instance()->GetDirUserData(),
        tr("Rabbit remote control Files (*.rrc);;All files(*.*)"));
    if(szFile.isEmpty()) return;

    COperate* p = m_Manager.LoadOperate(szFile);
    if(nullptr == p)
    {
        slotStatusMessage(tr("Load file fail: ") + szFile, MessageLevel::Error);
        return;
    }

    Start(p, true);
}

void MainWindow::slotStart()
{
    if(nullptr == m_pView)
    {
        Q_ASSERT(false);
        return;
    }
    QAction* pAction = qobject_cast<QAction*>(this->sender());
    Q_ASSERT(pAction);
    COperate* p = m_Manager.CreateOperate(pAction->data().toString());
    if(nullptr == p) return;
    Start(p, true);
}

/*!
 * \brief Start
 * \param p: COperate instance pointer
 * \param set: whether open settings dialog.
 *            true: open settings dialog and save configure file
 *            false: don't open settings dialog
 * \param szFile: Configure file.
 *            if is empty. the use default configure file.
 * \return 
 */
int MainWindow::Start(COperate *pOperate, bool set, QString szFile)
{
    qDebug(log) << "MainWindow::Start: set:" << set << "; File:" << szFile;
    bool bSave = false; //whether is save configure file
    Q_ASSERT(pOperate);
    bool check = connect(pOperate, SIGNAL(sigRunning()),
                         this, SLOT(slotRunning()));
    Q_ASSERT(check);
    check = connect(pOperate, SIGNAL(sigStop()),
                    this, SLOT(slotStop()));
    Q_ASSERT(check);
    check = connect(pOperate, SIGNAL(sigFinished()),
                    this, SLOT(slotFinished()));
    Q_ASSERT(check);
    check = connect(pOperate, SIGNAL(sigError(const int, const QString &)),
                    this, SLOT(slotError(const int, const QString&)));
    Q_ASSERT(check);
    check = connect(pOperate, SIGNAL(sigShowMessageBox(const QString&, const QString&,
                                                const QMessageBox::Icon&)),
                    this, SLOT(slotShowMessageBox(const QString&, const QString&,
                                            const QMessageBox::Icon&)));
    Q_ASSERT(check);
    check = connect(pOperate, SIGNAL(sigInformation(const QString&)),
                    this, SLOT(slotInformation(const QString&)));
    Q_ASSERT(check);
    check = connect(pOperate, SIGNAL(sigUpdateName(const QString&)),
                    this, SLOT(slotUpdateName(const QString&)));
    Q_ASSERT(check);
    check = connect(pOperate, SIGNAL(sigUpdateParameters(COperate*)),
                    this, SLOT(slotUpdateParameters(COperate*)));
    Q_ASSERT(check);
    check = connect(pOperate, &COperate::sigFullScreen,
                    this, [this, pOperate](bool bFull) {
        if(m_pView && m_pView->GetCurrentView() == pOperate->GetViewer()) {
            if((bFull && !isFullScreen()) || (!bFull && isFullScreen()))
                on_actionFull_screen_F_triggered();
        }
    });
    Q_ASSERT(check);
    if(set)
    {
        int nRet = pOperate->OpenDialogSettings(this);
        switch(nRet)
        {
        case QDialog::Rejected:
            m_Manager.DeleteOperate(pOperate);
            return 0;
        case QDialog::Accepted:
            bSave = true;
            break;
        }
    }

    if(szFile.isEmpty())
        szFile = pOperate->GetSettingsFile();
    else
        pOperate->SetSettingsFile(szFile);

    int nRet = 0;
    if(bSave)
        nRet = m_Manager.SaveOperate(pOperate);
    if(0 == nRet) {
        m_pRecentMenu->addRecentFile(szFile, pOperate->Name(), pOperate->Icon());
        CRecentDatabase::RecentItem item;
        item.szOperateId = pOperate->Id();
        item.icon = pOperate->Icon();
        item.szName = pOperate->Name();
        item.szProtocol = pOperate->Protocol();
        item.szType = pOperate->GetTypeName();
        item.szDescription = pOperate->Description();
        item.szFile = szFile;
        item.time = QDateTime::currentDateTime();
        m_pRecentDb->AddRecent(item);
    }
    
    if(!pOperate->Name().isEmpty())
        slotInformation(tr("Starting: ") + pOperate->Name());

    //* Show view. \see: slotRunning()
    if(m_Operates.contains(pOperate)) {
        if(m_pView)
            m_pView->SetCurrentView(pOperate->GetViewer());
        return 0;
    }
    if(m_pView)
    {
        m_pView->AddView(pOperate->GetViewer());
        m_pView->SetWidowsTitle(pOperate->GetViewer(), pOperate->Name(),
                                pOperate->Icon(), pOperate->Description());
        //qDebug(log) << "View:" << p->GetViewer();
        check = connect(pOperate, SIGNAL(sigViewerFocusIn(QWidget*)),
                        this, SLOT(slotViewerFocusIn(QWidget*)));
        Q_ASSERT(check);
    }
    m_Operates.insert(pOperate);
    StartTimer();

    m_pFrmActive->slotLoad();
    m_pFrmActive->slotViewChanged(m_pView->GetCurrentView());

    QVariant vOperate;
    vOperate.setValue(pOperate);
    QAction* m_pActionmenuActivity = ui->menuActivity->addAction(
        pOperate->Icon(), pOperate->Name(), this, SLOT(slotMenuActivity()));
    m_pActionmenuActivity->setData(vOperate);
    m_pActionmenuActivity->setCheckable(true);
    m_pMenuActivityGroup->addAction(m_pActionmenuActivity);
    m_pActionmenuActivity->setChecked(true);
    //*/

    nRet = pOperate->Start();
    if(nRet)
        pOperate->Stop();
    return nRet;
}

//! [MainWindow slotRunning]
/*!
 * \brief When connected. enable accept keyboard and mouse event in view
 */
void MainWindow::slotRunning()
{
    COperate* p = qobject_cast<COperate*>(sender());
    if(!p) return;

    /* If you put it here, when running, the view is not displayed.
       So put it in the Start() display view.
       See: Start(COperate *p, bool set, QString szFile)
     */
    /*
    if(-1 == m_Operates.indexOf(p)) {
        m_Operates.push_back(p);
        if(m_pView)
        {
            m_pView->AddView(p->GetViewer());
            m_pView->SetWidowsTitle(p->GetViewer(), p->Name(), p->Icon(), p->Description());
        }
    } else {
        m_pView->SetCurrentView(p->GetViewer());
    }
    //*/

    SetSecureLevel(p);

    auto m = p->GetMenu();
    if(m) {
        m->addSeparator();
        m->addAction(ui->actionClone);
    }
    slotLoadOperateMenu();

    slotInformation(tr("Connected to ") + p->Name());
    qDebug(log) << Q_FUNC_INFO << p->Name();
}
//! [MainWindow slotRunning]

void MainWindow::slotCloseView(const QWidget* pView)
{
    qDebug(log) << "MainWindow::slotCloseView" << pView;
    if(!pView) return;
    foreach(auto p, m_Operates)
    {
        if(p->GetViewer() == pView)
        {
            //TODO: Whether to save the setting
            emit p->sigUpdateParameters(p);
            p->Stop();
        }
    }
}

void MainWindow::on_actionStop_triggered()
{
    qDebug(log) << Q_FUNC_INFO;
    if(!m_pView) return;
    
    QWidget* pView = m_pView->GetCurrentView();
    slotCloseView(pView);    
}

void MainWindow::slotStop()
{
    COperate* pOperate = qobject_cast<COperate*>(sender());
    if(!pOperate) return;
    qDebug(log) << Q_FUNC_INFO << pOperate->Name();
    //TODO: Whether to save the setting
    emit pOperate->sigUpdateParameters(pOperate);
    pOperate->Stop();
}

void MainWindow::slotFinished()
{
    COperate* pOperate = qobject_cast<COperate*>(sender());
    if(!pOperate) return;

    qDebug(log) << Q_FUNC_INFO << pOperate->Name();
    foreach(auto a, ui->menuActivity->actions()) {
        COperate* o = a->data().value<COperate*>();
        if(o == pOperate) {
            ui->menuActivity->removeAction(a);
            m_pMenuActivityGroup->removeAction(a);
        }
    }
    foreach(auto p, m_Operates)
    {
        if(p == pOperate)
        {
            m_pView->RemoveView(p->GetViewer());
            m_Operates.remove(p);
            m_Manager.DeleteOperate(p);
            m_pFrmActive->slotLoad();
            m_pFrmActive->slotViewChanged(m_pView->GetCurrentView());
            break;
        }
    }
    if(m_Operates.isEmpty())
        if(m_pActionOperateMenu) {
            ui->menuTools->removeAction(m_pActionOperateMenu);
            ui->toolBar->removeAction(m_pActionOperateMenu);
            m_pActionOperateMenu = nullptr;
        }

    StartTimer();
}

// 该函数将label控件变成一个圆形指示灯，需要指定颜色color以及直径size
// size  单位是像素
void SetIndicator(QLabel* label, QColor color, int size)
{
    QString min_width = QString("min-width: %1px;").arg(size);              // 最小宽度：size
    QString min_height = QString("min-height: %1px;").arg(size);            // 最小高度：size
    QString max_width = QString("max-width: %1px;").arg(size);              // 最小宽度：size
    QString max_height = QString("max-height: %1px;").arg(size);            // 最小高度：size
    // 再设置边界形状及边框
    QString border_radius = QString("border-radius: %1px;").arg(size / 2);  // 边框是圆角，半径为size/2
    QString border = QString("border:1px solid ") + color.name() + ";";     // 边框为1px 
    // 最后设置背景颜色
    QString background = "background-color: ";
    background += color.name() + ";";
    QString SheetStyle = min_width + min_height + max_width + max_height + border_radius + border + background;
    label->setStyleSheet(SheetStyle);
}

void MainWindow::SetSecureLevel(COperate* o)
{
    if(!m_pSecureLevel) return;
    if(o) {
        if(COperate::SecurityLevel::No == o->GetSecurityLevel())
        {
            m_pSecureLevel->hide();
            return;
        }
        SetIndicator(m_pSecureLevel, o->GetSecurityLevelColor(), statusBar()->height() / 2);
        m_pSecureLevel->setToolTip(o->GetSecurityLevelString());
        m_pSecureLevel->setStatusTip(o->GetSecurityLevelString());
        m_pSecureLevel->show();
    } else
        m_pSecureLevel->hide();
}

void MainWindow::slotSignalConnected()
{
    m_pSignalStatus->setToolTip(tr("ICE signal status: Connected"));
    m_pSignalStatus->setStatusTip(m_pSignalStatus->toolTip());
    m_pSignalStatus->setWhatsThis(m_pSignalStatus->toolTip());
    //m_pSignalStatus->setText(tr("Connected"));
    m_pSignalStatus->setIcon(QIcon::fromTheme("network-wired"));
}

void MainWindow::slotSignalDisconnected()
{
    m_pSignalStatus->setToolTip(tr("ICE signal status: Disconnected"));
    m_pSignalStatus->setStatusTip(m_pSignalStatus->toolTip());
    m_pSignalStatus->setWhatsThis(m_pSignalStatus->toolTip());
    //m_pSignalStatus->setText(tr("Disconnected"));
    m_pSignalStatus->setIcon(QIcon::fromTheme("network-wireless"));
}

void MainWindow::slotSignalError(const int nError, const QString &szInfo)
{
    slotSignalDisconnected();
    slotError(nError, szInfo);
}

void MainWindow::slotSignalPushButtonClicked(bool checked)
{
#ifdef HAVE_ICE
    if(checked)
        CICE::Instance()->slotStart();
    else
        CICE::Instance()->slotStop();
#endif
}

void MainWindow::slotError(const int nError, const QString &szInfo)
{
    Q_UNUSED(nError);
    slotStatusMessage(szInfo, MessageLevel::Error);
}

void MainWindow::slotShowMessageBox(
    const QString &title, const QString &message,
    const QMessageBox::Icon &icon)
{
    slotInformation(message);
    if(!m_Parameter.GetMessageBoxDisplayInformation())
        return;
    
    QMessageBox msg(icon, title, message, QMessageBox::Ok, this);
    QCheckBox* cb = new QCheckBox(
        tr("Use message box to display information"), this);
    cb->setChecked(true);
    msg.setCheckBox(cb);
    RC_SHOW_WINDOW(&msg);
    if(!cb->isChecked())
    {
        m_Parameter.SetMessageBoxDisplayInformation(false);
        m_Parameter.Save();
    }
}

void MainWindow::slotInformation(const QString& szInfo)
{
    slotStatusMessage(szInfo, MessageLevel::Normal);
}

void MainWindow::slotStatusMessage(QString szMessage, MessageLevel level)
{
    QPalette pe;
    switch ((MessageLevel)level) {
    case MessageLevel::Error:
        pe.setColor(QPalette::WindowText, Qt::red);
        break;
    case MessageLevel::Warning:
        pe.setColor(QPalette::WindowText, Qt::yellow);
        break;
    default:
        break;
    }

    m_StatusBarMessage.setPalette(pe);
    m_StatusBarMessage.setToolTip(szMessage);
    QFontMetrics metrics(m_StatusBarMessage.font());

    szMessage = metrics.elidedText(szMessage, Qt::ElideRight, m_StatusBarMessage.width());

    m_StatusBarMessage.setText(szMessage);
}

void MainWindow::slotUpdateName()
{
    foreach (auto p, m_Operates)
    {
        m_pView->SetWidowsTitle(p->GetViewer(),
                                p->Name(),
                                p->Icon(),
                                p->Description());
    }
}

void MainWindow::slotUpdateName(const QString& szName)
{
    COperate* p = qobject_cast<COperate*>(sender());
    if(!p) return;
    // Update view
    m_pView->SetWidowsTitle(p->GetViewer(), szName,
                            p->Icon(), p->Description());

    // Update recent menu
    if(m_pRecentMenu)
        m_pRecentMenu->updateRecentFile(p->GetSettingsFile(), szName, p->Icon());

    // Update recent list view dock widget
    if(m_pRecentDb) {
        m_pRecentDb->UpdateRecent(
            p->GetSettingsFile(), p->Name(), p->Description());
    }

    // Update activity menu
    foreach(auto a, ui->menuActivity->actions()) {
        if(a->data().value<COperate*>() == p) {
            a->setText(szName);
            a->setIcon(p->Icon());
            break;
        }
    }

    // Update favorite
    m_pFavoriteView->slotUpdateFavorite(
        p->GetSettingsFile(), szName, p->Description(), p->Icon());

    //Update activity list view dock widget
    m_pFrmActive->slotLoad();
}

QAction* MainWindow::GetStartAction(QMenu* pMenu, CPlugin *pPlug)
{
    QString szTitle;
    if(!pPlug->Protocol().isEmpty())
        szTitle = pPlug->Protocol() + ": ";
    szTitle += pPlug->DisplayName();
    QAction* p = pMenu->addAction(szTitle);
    p->setToolTip(pPlug->Description());
    p->setStatusTip(pPlug->Description());
    p->setData(pPlug->Id());
    p->setIcon(pPlug->Icon());
    return p;    
}

void MainWindow::slotStartByType()
{
    qDebug(log) << Q_FUNC_INFO;
    auto m = ui->menuStart->actions();
    foreach(auto a, m) {
        a->deleteLater();
    }
    foreach (auto a, m_MenuStartByType) {
        a->deleteLater();
    }
    ui->menuStart->clear();
    m_MenuStartByType.clear();
    m_Manager.EnumPlugins(this);
}

int MainWindow::onProcess(const QString &id, CPlugin *pPlugin)
{
    Q_UNUSED(id);
    QMenu* m = ui->menuStart;
    if(m_Parameter.GetStartByType()) {
        auto it = m_MenuStartByType.find(pPlugin->Type());
        if(it == m_MenuStartByType.end()) {
            m = new QMenu(pPlugin->TypeName(pPlugin->Type()), ui->menuStart);
            m_MenuStartByType[pPlugin->Type()] = m;
            ui->menuStart->addMenu(m);
        } else
            m = *it;
    }
    // Start menu and toolbar
    QAction* p = GetStartAction(m, pPlugin);
    bool check = false;
    check = connect(p, SIGNAL(triggered()), this, SLOT(slotStart()));
    Q_ASSERT(check);
    return 0;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug(log) << "MainWindow::closeEvent()";
    
    if(m_Parameter.GetSaveMainWindowStatus())
        if(isFullScreen())
            on_actionFull_screen_F_triggered();
    
    SaveOperateLasterClose();
    
    foreach (auto it, m_Operates)
    {
        //TODO: Whether to save the setting
        emit it->sigUpdateParameters(it);
        it->Stop();
    }
    
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);    
    if(m_Parameter.GetSaveMainWindowStatus())
    {
        set.setValue("MainWindow/Status/Geometry", saveGeometry());
        set.setValue("MainWindow/Status/State", saveState());

        m_Parameter.SetTabBar(ui->actionTabBar_B->isChecked());
        m_Parameter.SetMenuBar(menuBar()->isVisible());
        m_Parameter.SetStatusBar(statusBar()->isVisible());
    } else {
        set.remove("MainWindow/Status/Geometry");
        set.remove("MainWindow/Status/State");
    }

    m_Manager.SaveSettings();
    m_Parameter.Save();

    QMainWindow::closeEvent(event);

    //TODO: Wait for the background thread to exit
    QThread::sleep(1);
}

int MainWindow::LoadOperateLasterClose()
{
    if(!m_Parameter.GetOpenLasterClose())
        return 0;
    
    QFile f(RabbitCommon::CDir::Instance()->GetDirUserConfig()
            + QDir::separator() + "LasterClose.dat");
    if(f.open(QFile::ReadOnly))
    {
        QDataStream d(&f);
        while(1){
            QString szFile;
            d >> szFile;
            if(szFile.isEmpty())
                break;
            slotOpenFile(szFile);
        }
        f.close();
    }
    return 0;
}

int MainWindow::SaveOperateLasterClose()
{
    QFile f(RabbitCommon::CDir::Instance()->GetDirUserConfig()
            + QDir::separator() + "LasterClose.dat");
    bool bRet = f.open(QFile::WriteOnly);
    if(bRet && m_Parameter.GetOpenLasterClose())
    {
        QDataStream d(&f);
        foreach(auto it, m_Operates)
        {
            d << it->GetSettingsFile();
        }
    }
    f.close();
    return 0;
}

void MainWindow::on_actionTabBar_B_toggled(bool bShow)
{
    if(m_pView)
    {
        m_pView->SetVisibleTab(bShow);
        m_Parameter.SetTabBar(bShow);
    } else {
        if(m_pDockActive)
            m_pDockActive->setVisible(bShow);
    }
}

void MainWindow::on_actionMain_menu_bar_M_toggled(bool checked)
{
    qDebug(log) << Q_FUNC_INFO << checked;
    if(ui->toolBar->isHidden() && !checked)
    {
        if( QMessageBox::StandardButton::Yes
            == QMessageBox::information(this, tr("Hide menu bar"),
                                        tr("The menu bar will be hidden, the tool bar must be showed."),
                                        QMessageBox::StandardButton::Yes
                                            | QMessageBox::StandardButton::No))
        {
            ui->actionToolBar_T->setChecked(true);
            if(m_pToolBarMenuAction)
                m_pToolBarMenuAction->setVisible(true);
        } else {
            ui->actionMain_menu_bar_M->setChecked(true);
#if !defined(Q_OS_ANDROID)
            if(m_pToolBarMenuAction)
                m_pToolBarMenuAction->setVisible(false);
#endif
            return;
        }
    }

    menuBar()->setVisible(checked);
    m_Parameter.SetMenuBar(checked);
#if !defined(Q_OS_ANDROID)
    if(m_pToolBarMenuAction)
        m_pToolBarMenuAction->setVisible(!checked);
#endif
}

void MainWindow::on_actionToolBar_T_toggled(bool checked)
{
    qDebug(log) << Q_FUNC_INFO << checked;
    if(menuBar()->isHidden() && !checked)
    {
        if( QMessageBox::StandardButton::Yes
            == QMessageBox::information(this, tr("Hide tool bar"),
                                        tr("The tool bar will be hidden, the menu bar must be showed."),
                                        QMessageBox::StandardButton::Yes
                                            | QMessageBox::StandardButton::No))
        {
            ui->actionMain_menu_bar_M->setChecked(true);
        } else {
            ui->actionToolBar_T->setChecked(true);
            return;
        }
    }
    ui->toolBar->setVisible(checked);
}

void MainWindow::on_actionStatus_bar_S_toggled(bool checked)
{
    statusBar()->setVisible(checked);
    m_Parameter.SetStatusBar(checked);
}

// [Get the widget that settings plugin parameters]
void MainWindow::on_actionSettings_triggered()
{
    CParameterDlgSettings set(&m_Parameter, this);
    auto viewers = m_Manager.GetSettingsWidgets(&set);
    set.SetViewers(viewers);
    if(CParameterDlgSettings::Accepted == RC_SHOW_WINDOW(&set))
    {
        m_Manager.SaveSettings();
        m_Parameter.Save();
    }
    foreach(auto v, viewers) {
        v->deleteLater();
    }
}
// [Get the widget that settings plugin parameters]

void MainWindow::slotShortCut()
{
    if(m_Parameter.GetReceiveShortCut())
    {
        setFocusPolicy(Qt::WheelFocus);
        ui->actionFull_screen_F->setShortcuts(QKeySequence::FullScreen);
    } else {
        setFocusPolicy(Qt::NoFocus);
        ui->actionFull_screen_F->setShortcut(QKeySequence());
    }
}

void MainWindow::on_actionOpenListRecent_triggered()
{
    CFrmRecent* p = new CFrmRecent(this, &m_Manager, m_pRecentDb, m_Parameter, false);
    if(!p) return;
    bool check = connect(p, SIGNAL(sigStart(const QString&, bool)),
                         this, SLOT(slotOpenFile(const QString&, bool)));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigAddToFavorite(QString,QString,QString,QIcon)),
                    m_pFavoriteView, SLOT(slotAddToFavorite(QString,QString,QString,QIcon)));
    Q_ASSERT(check);
    
    p->Init();
    QDialog d;
    d.resize(540, 400);
    d.setWindowIcon(windowIcon());
    d.setWindowTitle(p->windowTitle());
    QGridLayout* pLayout = new QGridLayout(&d);
    if(pLayout){
        pLayout->addWidget(p);
        check = connect(p, SIGNAL(destroyed()), &d, SLOT(reject()));
        d.setLayout(pLayout);
    }
    RC_SHOW_WINDOW(&d);
}

void MainWindow::on_actionAdd_to_favorite_triggered()
{
    if(!m_pView || !m_pFavoriteView) return;
    QWidget* p = m_pView->GetCurrentView();
    foreach(auto c, m_Operates)
    {
        if(c->GetViewer() == p)
        {
            m_pFavoriteView->slotAddToFavorite(
                c->GetSettingsFile(), c->Name(), c->Description(), c->Icon());
        }
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug(log) << "dragEnterEvent";
    
    if(event->mimeData()->hasUrls())
    {
        qWarning(log) << event->mimeData()->urls();
        event->acceptProposedAction();
    }
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    //qDebug(log) << "dragMoveEvent";
}

void MainWindow::dropEvent(QDropEvent *event)
{
    qDebug(log) << "dropEvent";
    if(!event->mimeData()->hasUrls())
        return;
    auto urls = event->mimeData()->urls();
    foreach(auto url, urls)
    {
        if(url.isLocalFile())
            slotOpenFile(url.toLocalFile());
    }
}

void MainWindow::slotSystemTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    //qDebug(log) << "MainWindow::slotSystemTrayIconActivated";

    Q_UNUSED(reason)
#if defined(Q_OS_ANDROID)
    showMaximized();
#else
    switch(reason)
    {
    case QSystemTrayIcon::Trigger:
    {
        showNormal();
        activateWindow();
        break;
    }
    default:
        break;
    }
#endif
}

void MainWindow::slotSystemTrayIconTypeChanged()
{
    //qDebug(log) << "MainWindow::slotSystemTrayIconTypeChanged:" << m_Parameter.GetEnableSystemTrayIcon();
    if(!QSystemTrayIcon::isSystemTrayAvailable())
    {
        qWarning(log) << "System tray is not available";
        return;
    }
    
    if(!m_Parameter.GetEnableSystemTrayIcon())
    {
        qDebug(log) << "Disable system tray icon";
        return;
    }

    if(m_TrayIcon)
        m_TrayIcon.reset();

    m_TrayIcon = QSharedPointer<QSystemTrayIcon>(new QSystemTrayIcon(this));
    if(QSystemTrayIcon::isSystemTrayAvailable())
    {
        bool check = connect(
            m_TrayIcon.data(),
            SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,
            SLOT(slotSystemTrayIconActivated(QSystemTrayIcon::ActivationReason)));
        Q_ASSERT(check);
        m_TrayIcon->setIcon(this->windowIcon());
        m_TrayIcon->setToolTip(windowTitle());
        m_TrayIcon->show();
    } else
        qWarning(log) << "System tray is not available";

    switch (m_Parameter.GetSystemTrayIconMenuType())
    {
    case CParameterApp::SystemTrayIconMenuType::MenuBar:
    {
        QMenu* pMenu = new QMenu(this);
        pMenu->addMenu(ui->menuOperate);
        pMenu->addMenu(ui->menuView);
        pMenu->addMenu(ui->menuTools);
        m_TrayIcon->setContextMenu(pMenu);
        break;
    }
    case CParameterApp::SystemTrayIconMenuType::Operate:
        m_TrayIcon->setContextMenu(ui->menuOperate);
        break;
    case CParameterApp::SystemTrayIconMenuType::RecentOpen:
        m_TrayIcon->setContextMenu(m_pRecentMenu);
        break;
    case CParameterApp::SystemTrayIconMenuType::View:
        m_TrayIcon->setContextMenu(ui->menuView);
        break;
    case CParameterApp::SystemTrayIconMenuType::Tools:
        m_TrayIcon->setContextMenu(ui->menuTools);
        break;
    case CParameterApp::SystemTrayIconMenuType::No:
        m_TrayIcon->setContextMenu(nullptr);
        break;
    }   
}

void MainWindow::slotEnableSystemTrayIcon()
{
    //qDebug(log) << "MainWindow::slotEnableSystemTryIcon()";
    if(m_TrayIcon)
    {
        if(!m_Parameter.GetEnableSystemTrayIcon())
            m_TrayIcon.reset();
    } else
        slotSystemTrayIconTypeChanged();
}

void MainWindow::slotTimeOut()
{
    auto pWin = m_pView->GetCurrentView();
    if(!pWin) {
        qDebug(log) << "The current view is empty";
        return;
    }
    foreach(auto p, m_Operates)
    {
        if(p->GetViewer() == pWin)
        {
            auto pStats = p->GetStats();
            if(!pStats) {
                SetStatsVisible(false);
                break;
            }
            SetStatsVisible(true);
            pStats->slotCalculating();
            m_TotalSends.setText("⇈ " + pStats->TotalSends());
            m_TotalReceives.setText("⇊ " + pStats->TotalReceives());
            m_SendRate.setText("↑ " + pStats->SendRate());
            m_ReceivesRate.setText("↓ " + pStats->ReceiveRate());
            break;
        }
    }
}

void MainWindow::SetStatsVisible(bool visible)
{
    if(m_SendRate.isVisible() == visible)
        return;
    m_SendRate.setVisible(visible);
    m_ReceivesRate.setVisible(visible);
    m_TotalSends.setVisible(visible);
    m_TotalReceives.setVisible(visible);
}

void MainWindow::StartTimer()
{
    bool bStart = false;
    int nMinInterval = 60;
    foreach(auto o, m_Operates) {
        if(o && o->GetStats()) {
            nMinInterval = qMin(nMinInterval, o->GetStats()->GetInterval());
            bStart = true;
        }
    }
    if(bStart)
        m_Timer.start(nMinInterval * 1000);
    else
        m_Timer.stop();
}

void MainWindow::on_actionUser_manual_triggered()
{
    QString szUrl = "https://github.com/KangLin/RabbitRemoteControl/wiki/UserManual";
    if(RabbitCommon::CTools::GetLanguage() == "zh_CN"
        || RabbitCommon::CTools::GetLanguage() == "zh_TW")
        szUrl += "_zh_CN";    
}

void MainWindow::on_actionLayoutDefault_triggered()
{
    qDebug(log) << Q_FUNC_INFO;
    if(!m_pDockActive->toggleViewAction()->isChecked())
        m_pDockActive->toggleViewAction()->trigger();
    if(!m_pDockFavorite->toggleViewAction()->isChecked())
        m_pDockFavorite->toggleViewAction()->trigger();
    if(!m_pDockRecent->toggleViewAction()->isChecked())
        m_pDockRecent->toggleViewAction()->trigger();
    m_Parameter.SetTabPosition(QTabWidget::North);
    if(!ui->actionTabBar_B->isChecked())
        ui->actionTabBar_B->trigger();
    if(!ui->actionMain_menu_bar_M->isChecked()) {
        ui->actionMain_menu_bar_M->trigger();
#if !defined(Q_OS_ANDROID)
        m_pToolBarMenuAction->setVisible(false);
#endif
    }
    if(!ui->actionToolBar_T->isChecked())
        ui->actionToolBar_T->trigger();
    addToolBar(Qt::TopToolBarArea, ui->toolBar);
    if(!ui->actionStatus_bar_S->isChecked())
        ui->actionStatus_bar_S->trigger();
}

void MainWindow::on_actionLayoutSimple_triggered()
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_pDockActive->toggleViewAction()->isChecked())
        m_pDockActive->toggleViewAction()->trigger();
    if(m_pDockFavorite->toggleViewAction()->isChecked())
        m_pDockFavorite->toggleViewAction()->trigger();
    if(m_pDockRecent->toggleViewAction()->isChecked())
        m_pDockRecent->toggleViewAction()->trigger();
    m_Parameter.SetTabPosition(QTabWidget::East);
    if(!ui->actionTabBar_B->isChecked())
        ui->actionTabBar_B->trigger();
    if(!ui->actionMain_menu_bar_M->isChecked()) {
        ui->actionMain_menu_bar_M->trigger();
#if !defined(Q_OS_ANDROID)
        m_pToolBarMenuAction->setVisible(false);
#endif
    }
    if(!ui->actionToolBar_T->isChecked())
        ui->actionToolBar_T->trigger();
    addToolBar(Qt::LeftToolBarArea, ui->toolBar);
    if(!ui->actionStatus_bar_S->isChecked())
        ui->actionStatus_bar_S->trigger();
}

void MainWindow::on_actionLayoutMinimalism_triggered()
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_pDockActive->toggleViewAction()->isChecked())
        m_pDockActive->toggleViewAction()->trigger();
    if(m_pDockFavorite->toggleViewAction()->isChecked())
        m_pDockFavorite->toggleViewAction()->trigger();
    if(m_pDockRecent->toggleViewAction()->isChecked())
        m_pDockRecent->toggleViewAction()->trigger();
    m_Parameter.SetTabPosition(QTabWidget::East);
    if(ui->actionTabBar_B->isChecked())
        ui->actionTabBar_B->trigger();
#if !defined(Q_OS_MACOS)
    if(ui->actionMain_menu_bar_M->isChecked()) {
        ui->actionMain_menu_bar_M->trigger();
        m_pToolBarMenuAction->setVisible(true);
    }
#endif
    if(!ui->actionToolBar_T->isChecked())
        ui->actionToolBar_T->trigger();
#if defined(Q_OS_LINUX)
    addToolBar(Qt::RightToolBarArea, ui->toolBar);
#else
    addToolBar(Qt::LeftToolBarArea, ui->toolBar);
#endif
    if(ui->actionStatus_bar_S->isChecked())
        ui->actionStatus_bar_S->trigger();
}
