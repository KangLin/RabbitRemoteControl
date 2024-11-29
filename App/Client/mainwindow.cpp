// Author: Kang Lin <kl222@126.com>

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

#include "Connecter.h"
#include "FrmFullScreenToolBar.h"
#include "ParameterDlgSettings.h"
#include "FrmListConnects.h"

#ifdef HAVE_ICE
#include "Ice.h"
#endif

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

static Q_LOGGING_CATEGORY(log, "App.MainWindow")
static Q_LOGGING_CATEGORY(logRecord, "App.MainWindow.Record")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    m_pActionConnecterMenu(nullptr),
    m_pDockListConnects(nullptr),
    m_pSignalStatus(nullptr),
    ui(new Ui::MainWindow),
    m_pView(nullptr),
    m_pFullScreenToolBar(nullptr),
    m_pRecentMenu(nullptr),
    m_pDockFavorite(nullptr),
    m_pFavoriteView(nullptr)
{
    bool check = false;

    ui->setupUi(this);
    
    setFocusPolicy(Qt::NoFocus);
    //addToolBar(Qt::LeftToolBarArea, ui->toolBar);
    setAcceptDrops(true);

    m_pDockFavorite = new QDockWidget(this);
    if(m_pDockFavorite)
    {
        m_pFavoriteView = new CFavoriteView(m_pDockFavorite);
        if(m_pFavoriteView)
        {
            m_pDockFavorite->setTitleBarWidget(m_pFavoriteView->m_pDockTitleBar);
            check = connect(m_pFavoriteView, SIGNAL(sigConnect(const QString&, bool)),
                            this, SLOT(slotOpenFile(const QString&, bool)));
            Q_ASSERT(check);
            check = connect(m_pFavoriteView, SIGNAL(sigFavorite()),
                            this, SLOT(on_actionAdd_to_favorite_triggered()));
            Q_ASSERT(check);
            check = connect(&m_Parameter, SIGNAL(sigFavoriteEditChanged(bool)),
                            m_pFavoriteView, SLOT(slotDoubleEditNode(bool)));
            Q_ASSERT(check);
            m_pDockFavorite->setWidget(m_pFavoriteView);
            m_pDockFavorite->setWindowTitle(m_pFavoriteView->windowTitle());
        }
        // Must set ObjectName then restore it. See: saveState help document
        m_pDockFavorite->setObjectName("dockFavorite");
        //m_pDockFavorite->hide();
        ui->menuView->addAction(m_pDockFavorite->toggleViewAction());
        addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, m_pDockFavorite);
    }

    m_pDockListConnects = new QDockWidget(this);
    if(m_pDockListConnects)
    {
        CFrmListConnects* pListConnects = new CFrmListConnects(&m_Client, true, m_pDockListConnects);
        if(pListConnects) {
            if(pListConnects->m_pDockTitleBar)
                m_pDockListConnects->setTitleBarWidget(pListConnects->m_pDockTitleBar);
            check = connect(pListConnects, SIGNAL(sigConnect(const QString&, bool)),
                            this, SLOT(slotOpenFile(const QString&, bool)));
            Q_ASSERT(check);
            m_pDockListConnects->setWidget(pListConnects);
            m_pDockListConnects->setWindowTitle(pListConnects->windowTitle());
        }
        // Must set ObjectName then restore it. See: saveState help document
        m_pDockListConnects->setObjectName("dockListConnects");
        //m_pDockListConnects->hide();
        ui->menuView->addAction(m_pDockListConnects->toggleViewAction());
        tabifyDockWidget(m_pDockFavorite, m_pDockListConnects);
    }

    RabbitCommon::CTools::AddStyleMenu(ui->menuTools);
    ui->menuTools->addMenu(RabbitCommon::CTools::GetLogMenu(this));

    m_pRecentMenu = new RabbitCommon::CRecentMenu(tr("Recently connected"),
                                                  QIcon::fromTheme("document-open-recent"),
                                                  this);
    check = connect(m_pRecentMenu, SIGNAL(recentFileTriggered(const QString&)),
                    this, SLOT(slotOpenFile(const QString&)));
    Q_ASSERT(check);
    check = connect(&m_Parameter, SIGNAL(sigRecentMenuMaxCountChanged(int)),
                    m_pRecentMenu, SLOT(setMaxCount(int)));
    Q_ASSERT(check);
    QAction* pRecentAction = ui->menuRemote->insertMenu(
        ui->actionOpenListConnections, m_pRecentMenu);
    pRecentAction->setStatusTip(pRecentAction->text());
    QToolButton* tbRecent = new QToolButton(ui->toolBar);
    tbRecent->setFocusPolicy(Qt::NoFocus);
    tbRecent->setPopupMode(QToolButton::InstantPopup);
    tbRecent->setMenu(m_pRecentMenu);
    tbRecent->setIcon(pRecentAction->icon());
    tbRecent->setText(pRecentAction->text());
    tbRecent->setToolTip(pRecentAction->toolTip());
    tbRecent->setStatusTip(pRecentAction->statusTip());
    ui->toolBar->insertWidget(ui->actionOpenListConnections, tbRecent);

#ifdef HAVE_UPDATE
    CFrmUpdater updater;
    ui->actionUpdate->setIcon(updater.windowIcon());
#endif

    QToolButton* tbConnect = new QToolButton(ui->toolBar);
    tbConnect->setFocusPolicy(Qt::NoFocus);
    tbConnect->setPopupMode(QToolButton::InstantPopup);
    //tbConnect->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    tbConnect->setMenu(ui->menuConnect_C);
    tbConnect->setIcon(QIcon::fromTheme("network-wired"));
    tbConnect->setText(tr("Connect"));
    tbConnect->setToolTip(tr("Connect"));
    tbConnect->setStatusTip(tr("Connect"));
    m_pActionConnect = ui->toolBar->insertWidget(ui->actionDisconnect_D, tbConnect);

    EnableMenu(false);

    //TODO: Change view
    m_pView = new CViewTable(this);
    if(m_pView)
    {
        m_pView->setFocusPolicy(Qt::NoFocus);
        check = connect(m_pView, SIGNAL(sigCloseView(const QWidget*)),
                        this, SLOT(slotCloseView(const QWidget*)));
        Q_ASSERT(check);

        check = connect(m_pView, SIGNAL(sigCurrentChanged(const QWidget*)),
                        this, SLOT(slotCurrentViewChanged(const QWidget*)));
        Q_ASSERT(check);
        check = connect(m_pView, SIGNAL(customContextMenuRequested(const QPoint&)),
                        this, SLOT(slotCustomContextMenuRequested(const QPoint&)));
        Q_ASSERT(check);
        this->setCentralWidget(m_pView);
    }

    m_Client.EnumPlugins(this);
    m_Client.LoadSettings();

    check = connect(&m_Parameter, SIGNAL(sigReceiveShortCutChanged()),
                    this, SLOT(slotShortCut()));
    Q_ASSERT(check);
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
    m_Parameter.Load();
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
    CICE::Instance()->slotStart();
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
        QByteArray state = set.value("MainWindow/Status/State").toByteArray();
        if(!state.isEmpty())
            restoreState(state);
        
        ui->actionStatus_bar_S->setChecked(m_Parameter.GetStatusBar());
        statusBar()->setVisible(m_Parameter.GetStatusBar());
        ui->actionTabBar_B->setChecked(m_Parameter.GetTabBar());
        on_actionMain_menu_bar_M_toggled(m_Parameter.GetMenuBar());
        ui->actionMain_menu_bar_M->setChecked(m_Parameter.GetMenuBar());
        ui->actionToolBar_T->setChecked(!ui->toolBar->isHidden());
    }

    slotEnableSystemTrayIcon();

    LoadConnectLasterClose();
}

MainWindow::~MainWindow()
{
    qDebug(log) << "MainWindow::~MainWindow()";
    if(m_pFullScreenToolBar) m_pFullScreenToolBar->close();
    delete ui;
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
    about->m_szDetails = m_Client.Details();
    RC_SHOW_WINDOW(about);
#endif
}

void MainWindow::on_actionUpdate_triggered()
{
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

void MainWindow::on_actionFull_screen_F_triggered()
{
    CView* pTab = qobject_cast<CView*>(this->centralWidget());
    if(pTab)
    {
        pTab->SetFullScreen(!isFullScreen());
    }

    if(isFullScreen())
    {
        qDebug(log) << "Exit full screen";
        ui->actionFull_screen_F->setIcon(QIcon::fromTheme("view-fullscreen"));
        ui->actionFull_screen_F->setText(tr("Full screen(&F)"));
        ui->actionFull_screen_F->setToolTip(tr("Full screen"));
        ui->actionFull_screen_F->setStatusTip(tr("Full screen"));
        ui->actionFull_screen_F->setWhatsThis(tr("Full screen"));

        ui->toolBar->setVisible(m_FullState.toolBar);
        ui->statusbar->setVisible(m_FullState.statusbar);
        ui->menubar->setVisible(m_FullState.menubar);

        m_pDockListConnects->setVisible(m_FullState.dockListConnects);
        m_pDockFavorite->setVisible(m_FullState.dockFavorite);
        // This is hade code. it is in RabbitCommon
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

        return;
    }

    qDebug(log) << "Entry full screen";
    emit sigFullScreen();
    //setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    this->showFullScreen();

    ui->actionFull_screen_F->setIcon(QIcon::fromTheme("view-restore"));
    ui->actionFull_screen_F->setText(tr("Exit full screen(&E)"));
    ui->actionFull_screen_F->setToolTip(tr("Exit full screen"));
    ui->actionFull_screen_F->setStatusTip(tr("Exit full screen"));
    ui->actionFull_screen_F->setWhatsThis(tr("Exit full screen"));

    m_FullState.toolBar = ui->toolBar->isVisible();
    ui->toolBar->setVisible(false);
    m_FullState.statusbar = ui->statusbar->isVisible();
    ui->statusbar->setVisible(false);
    m_FullState.menubar = ui->menubar->isVisible();
    ui->menubar->setVisible(false);

    m_FullState.dockListConnects = m_pDockListConnects->isVisible();
    m_pDockListConnects->setVisible(false);
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
    check = connect(m_pFullScreenToolBar, SIGNAL(sigDisconnect()),
                    this, SLOT(on_actionDisconnect_D_triggered()));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigConnecterMenuChanged(QAction*)),
                    m_pFullScreenToolBar,
                    SLOT(slotConnecterMenuChanged(QAction*)));
    Q_ASSERT(check);
    CViewTable* p = dynamic_cast<CViewTable*>(pTab);
    if(p)
    {
        check = connect(m_pFullScreenToolBar, SIGNAL(sigShowTabBar(bool)),
                        SLOT(on_actionTabBar_B_toggled(bool)));
        Q_ASSERT(check);
    }

    m_pFullScreenToolBar->show();
}

void MainWindow::slotCurrentViewChanged(const QWidget* pView)
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_pView && pView) {
        EnableMenu(true);
    } else
        EnableMenu(false);
}

void MainWindow::EnableMenu(bool bEnable)
{
    qDebug(log) << Q_FUNC_INFO << bEnable;
    ui->actionClone->setEnabled(bEnable);
    ui->actionAdd_to_favorite->setEnabled(bEnable);
    ui->actionCurrent_connect_parameters->setEnabled(bEnable);
    ui->actionDisconnect_D->setEnabled(bEnable);
    slotLoadConnecterMenu();
}

void MainWindow::slotLoadConnecterMenu()
{
    qDebug(log) << Q_FUNC_INFO;

    if(m_pActionConnecterMenu) {
        ui->menuTools->removeAction(m_pActionConnecterMenu);
        ui->toolBar->removeAction(m_pActionConnecterMenu);
        m_pActionConnecterMenu = nullptr;
    }

    if(!m_pView)
        return;
    auto pWin = m_pView->GetCurrentView();
    if(!pWin) {
        qDebug(log) << "The current view is empty";
        return;
    }
    foreach(auto c, m_Connecters)
    {
        if(c->GetViewer() == pWin)
        {
            qDebug(log) << "Load plugin menu";
            auto m = c->GetMenu(ui->menuTools);
            if(!m) return;
            m_pActionConnecterMenu = ui->menuTools->addMenu(m);
            ui->toolBar->insertAction(ui->actionFull_screen_F, m_pActionConnecterMenu);
            emit sigConnecterMenuChanged(m_pActionConnecterMenu);
        }
    }
}

void MainWindow::slotCustomContextMenuRequested(const QPoint &pos)
{
    if(!m_pView)
        return;
    auto pWin = m_pView->GetCurrentView();
    if(!pWin) {
        qDebug(log) << "The current view is empty";
        return;
    }
    foreach(auto c, m_Connecters)
    {
        if(c->GetViewer() == pWin)
        {
            qDebug(log) << "Load plugin menu";
            auto m = c->GetMenu(ui->menuTools);
            if(!m) return;
            m->exec(m_pView->mapToGlobal(pos));
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

void MainWindow::slotUpdateParameters(CConnecter* pConnecter)
{
    auto it = m_ConfigureFiles.find(pConnecter);
    if(m_ConfigureFiles.end() == it)
        return;
    m_Client.SaveConnecter(it.value(), pConnecter);
}

void MainWindow::on_actionClone_triggered()
{
    if(!m_pView) return;
    QWidget* p = m_pView->GetCurrentView();
    foreach(auto c, m_Connecters)
    {
        if(c->GetViewer() == p)
        {
            auto it = m_ConfigureFiles.find(c);
            if(m_ConfigureFiles.end() == it)
                return;
            QString szFile = it.value();
            auto pConnecter = m_Client.LoadConnecter(szFile);
            if(!pConnecter) return;
            Connect(pConnecter, false, szFile);
            return;
        }
    }
}

void MainWindow::slotOpenFile(const QString& szFile, bool bOpenSettings)
{
    if(szFile.isEmpty()) return;
    CConnecter* p = m_Client.LoadConnecter(szFile);
    if(nullptr == p)
    {
        slotInformation(tr("Load file fail: ") + szFile);
        return;
    }
    
    Connect(p, bOpenSettings, szFile);
}

void MainWindow::on_actionOpenRRCFile_triggered()
{
    QString szFile = QFileDialog::getOpenFileName(this,
                                                  tr("Open rabbit remote control file"),
                                                  RabbitCommon::CDir::Instance()->GetDirUserData(),
                                                  tr("Rabbit remote control Files (*.rrc);;All files(*.*)"));
    if(szFile.isEmpty()) return;

    CConnecter* p = m_Client.LoadConnecter(szFile);
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
    CConnecter* p = m_Client.CreateConnecter(pAction->data().toString());
    if(nullptr == p) return;
    Connect(p, true);
}

/*!
 * \brief Connect
 * \param p: CConnecter instance pointer
 * \param set: whether open settings dialog.
 *            true: open settings dialog and save configure file
 *            false: don't open settings dialog
 * \param szFile: Configure file.
 *            if is empty. the use default configure file.
 * \return 
 */
int MainWindow::Connect(CConnecter *p, bool set, QString szFile)
{
    qDebug(log) << "MainWindow::Connect: set:" << set << "; File:" << szFile;
    bool bSave = false; //whether is save configure file
    Q_ASSERT(p);
    bool check = connect(p, SIGNAL(sigConnected()),
                         this, SLOT(slotConnected()));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigDisconnect()),
                    this, SLOT(slotDisconnect()));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigDisconnected()),
                    this, SLOT(slotDisconnected()));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigError(const int, const QString &)),
                    this, SLOT(slotError(const int, const QString&)));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigShowMessageBox(const QString&, const QString&,
                                                const QMessageBox::Icon&)),
                    this, SLOT(slotShowMessageBox(const QString&, const QString&,
                                            const QMessageBox::Icon&)));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigInformation(const QString&)),
                    this, SLOT(slotInformation(const QString&)));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigUpdateName(const QString&)),
                    this, SLOT(slotUpdateName(const QString&)));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigUpdateParameters(CConnecter*)),
                    this, SLOT(slotUpdateParameters(CConnecter*)));
    Q_ASSERT(check);

    if(set)
    {
        int nRet = p->OpenDialogSettings(this);
        switch(nRet)
        {
        case QDialog::Rejected:
            m_Client.DeleteConnecter(p);
            return 0;
        case QDialog::Accepted:
            bSave = true;
            break;
        }
    }

    if(szFile.isEmpty())
        szFile = RabbitCommon::CDir::Instance()->GetDirUserData()
                 + QDir::separator()
                 + p->Id()
                 + ".rrc";
    m_ConfigureFiles[p] = szFile;

    int nRet = 0;
    if(bSave)
        nRet = m_Client.SaveConnecter(szFile, p);
    if(0 == nRet)
        m_pRecentMenu->addRecentFile(szFile, p->Name());

    if(!p->Name().isEmpty())
        slotInformation(tr("Connecting to ") + p->Name());

    //* Show view. \see: slotConnected()
    if(m_pView)
    {
        m_pView->AddView(p->GetViewer());
        m_pView->SetWidowsTitle(p->GetViewer(), p->Name(), p->Icon(), p->Description());
        //qDebug(log) << "View:" << p->GetViewer();
    }

    m_Connecters.push_back(p);//*/

    p->Connect();

    return 0;
}

//! [MainWindow slotConnected]
/*!
 * \brief When connected. enable accept keyboard and mouse event in view
 */
void MainWindow::slotConnected()
{
    CConnecter* p = dynamic_cast<CConnecter*>(sender());
    if(!p) return;

    /* If you put it here, when connected, the view is not displayed.
       So put it in the connect() display view.
       See: Connect(CConnecter *p, bool set, QString szFile)
     */
    /*
    if(m_pView)
    {
        m_pView->SetAdaptWindows(CFrmViewer::Auto, p->GetViewer());
        m_pView->AddView(p->GetViewer());
        m_pView->SetWidowsTitle(p->GetViewer(), p->Name(), p->Icon(), p->Description());
    }
    m_Connecters.push_back(p);//*/

    slotLoadConnecterMenu();

    slotInformation(tr("Connected to ") + p->Name());
    qDebug(log) << "MainWindow::slotConnected()" << p->Name();
}
//! [MainWindow slotConnected]

void MainWindow::slotCloseView(const QWidget* pView)
{
    qDebug(log) << "MainWindow::slotCloseView" << pView;
    if(!pView) return;
    foreach(auto c, m_Connecters)
    {
        if(c->GetViewer() == pView)
        {
            //TODO: Whether to save the setting
            emit c->sigUpdateParameters(c);
            c->DisConnect();
        }
    }
}

void MainWindow::on_actionDisconnect_D_triggered()
{
    qDebug(log) << "MainWindow::on_actionDisconnect_D_triggered()";
    if(!m_pView) return;
    
    QWidget* pView = m_pView->GetCurrentView();
    slotCloseView(pView);    
}

void MainWindow::slotDisconnect()
{
    qDebug(log) << "MainWindow::slotDisconnect()";
    CConnecter* pConnecter = dynamic_cast<CConnecter*>(sender());
    if(!pConnecter) return;
    //TODO: Whether to save the setting
    emit pConnecter->sigUpdateParameters(pConnecter);
    pConnecter->DisConnect();
}

void MainWindow::slotDisconnected()
{
    qDebug(log) << "MainWindow::slotDisconnected()";
    CConnecter* pConnecter = dynamic_cast<CConnecter*>(sender());
    foreach(auto c, m_Connecters)
    {
        if(c == pConnecter)
        {
            m_pView->RemoveView(c->GetViewer());
            m_Connecters.removeAll(c);
            m_ConfigureFiles.remove(c);
            m_Client.DeleteConnecter(c);
            return;
        }
    }
}

void MainWindow::slotSignalConnected()
{
    m_pSignalStatus->setToolTip(tr("ICE signal status: Connected"));
    m_pSignalStatus->setStatusTip(m_pSignalStatus->toolTip());
    m_pSignalStatus->setWhatsThis(m_pSignalStatus->toolTip());
    //m_pSignalStatus->setText(tr("Connected"));
    m_pSignalStatus->setIcon(QIcon::fromTheme("newwork-wired"));
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
    slotInformation(szInfo);
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
    slotInformation(szInfo);
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
    statusBar()->showMessage(szInfo);
}

void MainWindow::slotUpdateName()
{
    foreach (auto pConnecter, m_Connecters)
    {
        m_pView->SetWidowsTitle(pConnecter->GetViewer(),
                                pConnecter->Name(),
                                pConnecter->Icon(),
                                pConnecter->Description());
    }
}

void MainWindow::slotUpdateName(const QString& szName)
{
    CConnecter* pConnecter = dynamic_cast<CConnecter*>(sender());
    if(!pConnecter) return;
    m_pView->SetWidowsTitle(pConnecter->GetViewer(), szName,
                            pConnecter->Icon(), pConnecter->Description());
}

int MainWindow::onProcess(const QString &id, CPluginClient *pPlug)
{
    Q_UNUSED(id);
    // Connect menu and toolbar
    QAction* p = ui->menuConnect_C->addAction(pPlug->Protocol()
                                                  + ": " + pPlug->DisplayName(),
                                              this, SLOT(slotConnect()));
    p->setToolTip(pPlug->Description());
    p->setStatusTip(pPlug->Description());
    p->setData(id);
    p->setIcon(pPlug->Icon());

    return 0;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug(log) << "MainWindow::closeEvent()";
    
    if(m_Parameter.GetSaveMainWindowStatus())
        if(isFullScreen())
            on_actionFull_screen_F_triggered();
    
    SaveConnectLasterClose();
    
    foreach (auto it, m_Connecters)
    {
        //TODO: Whether to save the setting
        emit it->sigUpdateParameters(it);
        it->DisConnect();
    }
    
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);    
    if(m_Parameter.GetSaveMainWindowStatus())
    {
        set.setValue("MainWindow/Status/Geometry", saveGeometry());
        set.setValue("MainWindow/Status/State", saveState());
        // Other parameters of main windows is saved CParameterApp::~CParameterApp()
    } else {
        set.remove("MainWindow/Status/Geometry");
        set.remove("MainWindow/Status/State");
    }
    QMainWindow::closeEvent(event);
}

int MainWindow::LoadConnectLasterClose()
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

int MainWindow::SaveConnectLasterClose()
{
    QFile f(RabbitCommon::CDir::Instance()->GetDirUserConfig()
            + QDir::separator() + "LasterClose.dat");
    f.open(QFile::WriteOnly);
    if(m_Parameter.GetOpenLasterClose())
    {
        QDataStream d(&f);
        foreach(auto it, m_ConfigureFiles)
        {
            d << it;
        }
    }
    f.close();
    return 0;
}

void MainWindow::on_actionSend_ctl_alt_del_triggered()
{
    if(m_pView)
        m_pView->slotSystemCombination();
}

void MainWindow::on_actionTabBar_B_toggled(bool bShow)
{
    CViewTable* p = dynamic_cast<CViewTable*>(m_pView);
    if(p)
    {
        p->ShowTabBar(bShow);
        m_Parameter.SetTabBar(bShow);
    }
}

void MainWindow::on_actionMain_menu_bar_M_toggled(bool checked)
{
    qDebug(log) << "MainWindow::on_actionMain_menu_bar_M_triggered:" << checked;
    if(ui->toolBar->isHidden() && !checked)
    {
        if( QMessageBox::StandardButton::Yes
            == QMessageBox::information(this, tr("Hide menu bar"),
                                        tr("The menu bar will be hidden, the tool bar must be showed."),
                                        QMessageBox::StandardButton::Yes
                                            | QMessageBox::StandardButton::No))
        {
            ui->actionToolBar_T->setChecked(true);
        } else
            return;
    }
    
    menuBar()->setVisible(checked);
    m_Parameter.SetMenuBar(checked);
    if(checked)
    {
        ui->toolBar->removeAction(ui->actionMain_menu_bar_M);
    }
    else
    {
        ui->toolBar->insertAction(ui->actionTabBar_B,
                                  ui->actionMain_menu_bar_M);
    }
}

void MainWindow::on_actionToolBar_T_toggled(bool checked)
{
    qDebug(log) << "MainWindow::on_actionToolBar_T_triggered:" << checked;
    if(menuBar()->isHidden() && !checked)
    {
        if( QMessageBox::StandardButton::Yes
            == QMessageBox::information(this, tr("Hide tool bar"),
                                        tr("The tool bar will be hidden, the menu bar must be showed."),
                                        QMessageBox::StandardButton::Yes
                                            | QMessageBox::StandardButton::No))
        {
            ui->actionMain_menu_bar_M->setChecked(true);
        } else
            return;
    }
    ui->toolBar->setVisible(checked);
}

void MainWindow::on_actionStatus_bar_S_toggled(bool checked)
{
    statusBar()->setVisible(checked);
    m_Parameter.SetStatusBar(checked);
}

// [Get the widget that settings client parameters]
void MainWindow::on_actionSettings_triggered()
{
    CParameterDlgSettings set(&m_Parameter, m_Client.GetSettingsWidgets(this), this);
    if(CParameterDlgSettings::Accepted == RC_SHOW_WINDOW(&set))
    {
        m_Client.SaveSettings();
        m_Parameter.Save();
    }
}
// [Get the widget that settings client parameters]

void MainWindow::slotShortCut()
{
    if(m_Parameter.GetReceiveShortCut())
    {
        setFocusPolicy(Qt::WheelFocus);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        ui->actionFull_screen_F->setShortcut(
            QKeySequence(QKeyCombination(Qt::CTRL, Qt::Key_R),
                         QKeyCombination(Qt::Key_F)));
        ui->actionScreenshot->setShortcut(
            QKeySequence(QKeyCombination(Qt::CTRL, Qt::Key_R),
                         QKeyCombination(Qt::Key_S)));
#else
        ui->actionFull_screen_F->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R, Qt::Key_F));
        ui->actionScreenshot->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R, Qt::Key_S));
#endif
    } else {
        setFocusPolicy(Qt::NoFocus);
        ui->actionFull_screen_F->setShortcut(QKeySequence());
        ui->actionScreenshot->setShortcut(QKeySequence());
    }
}

void MainWindow::on_actionCurrent_connect_parameters_triggered()
{
    if(!m_pView) return;
    QWidget* p = m_pView->GetCurrentView();
    foreach(auto c, m_Connecters)
    {
        if(c->GetViewer() == p)
        {
            int nRet = c->OpenDialogSettings(this);
            if(QDialog::Accepted == nRet)
            {
                emit c->sigUpdateParameters(c);
                return;
            }
        }
    }
}

void MainWindow::on_actionOpenListConnections_triggered()
{
    CFrmListConnects* p = new CFrmListConnects(&m_Client, false);
    if(!p) return;
    bool check = connect(p, SIGNAL(sigConnect(const QString&, bool)),
                         this, SLOT(slotOpenFile(const QString&, bool)));
    Q_ASSERT(check);

    QDialog d;
    d.resize(540, 400);
    d.setWindowIcon(windowIcon());
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
    foreach(auto c, m_Connecters)
    {
        if(c->GetViewer() == p)
        {
            auto it = m_ConfigureFiles.find(c);
            if(m_ConfigureFiles.end() == it)
                return;
            m_pFavoriteView->AddFavorite(c->Name(), it.value());
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
        pMenu->addMenu(ui->menuRemote);
        pMenu->addMenu(ui->menuView);
        pMenu->addMenu(ui->menuTools);
        m_TrayIcon->setContextMenu(pMenu);
        break;
    }
    case CParameterApp::SystemTrayIconMenuType::Remote:
        m_TrayIcon->setContextMenu(ui->menuRemote);
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
