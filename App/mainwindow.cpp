//! @author: Kang Lin(kl222@126.com)

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "FrmUpdater/FrmUpdater.h"
#include "RabbitCommonDir.h"
#include "DlgAbout/DlgAbout.h"
#ifdef BUILD_QUIWidget
    #include "QUIWidget/QUIWidget.h"
#endif

#include "Connecter.h"
#include "FrmFullScreenToolBar.h"

#include <QMessageBox>
#include <QScreen>
#include <QApplication>
#include <QToolButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_Style(this),
      m_pGBView(nullptr),
      m_pView(nullptr),
      m_pFullScreenToolBar(nullptr)
{
    bool check = false;
    m_Style.LoadStyle();
    ui->setupUi(this);

    CFrmUpdater updater;
    ui->actionUpdate_U->setIcon(updater.windowIcon());
    
    //TODO: Change view
    m_pView = new CViewTable(this);
    if(m_pView)
    {
        check = connect(m_pView, SIGNAL(sigCloseView(const QWidget*)),
                        this, SLOT(slotCloseView(const QWidget*)));
        Q_ASSERT(check);
        check = connect(m_pView, SIGNAL(sigAdaptWindows(const CFrmViewer::ADAPT_WINDOWS)),
                        this, SLOT(slotAdaptWindows(const CFrmViewer::ADAPT_WINDOWS)));
        Q_ASSERT(check);
        this->setCentralWidget(m_pView);
    }
    
    m_ManageConnecter.EnumPlugins(this);
    
    QToolButton* tb = new QToolButton(ui->toolBar);
    tb->setPopupMode(QToolButton::MenuButtonPopup);
    //tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    tb->setMenu(ui->menuConnect_C);
    tb->setIcon(QIcon(":/image/Connect"));
    tb->setText(tr("Connect"));
    tb->setToolTip(tr("Connect"));
    tb->setStatusTip(tr("Connect"));
    ui->toolBar->insertWidget(ui->actionDisconnect_D, tb);
    
    m_pGBView = new QActionGroup(this);
    if(m_pGBView) {
        m_pGBView->addAction(ui->actionZoom_Z);
        m_pGBView->addAction(ui->actionOriginal_O);
        m_pGBView->addAction(ui->actionKeep_AspectRation_K);
        m_pGBView->setEnabled(false);
    }
    ui->actionZoom_Z->setChecked(true);
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
}

void MainWindow::on_actionUpdate_U_triggered()
{
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
        pTab->SetFullScreen(!isFullScreen());
    }
    
    if(this->isFullScreen())
    {
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
    m_pFullScreenToolBar->show();
}

void MainWindow::on_actionZoom_Z_toggled(bool arg1)
{
    if(!arg1) return;
    if(!m_pView) return;
    m_pView->SetAdaptWindows(CFrmViewer::Zoom);
}

void MainWindow::on_actionKeep_AspectRation_K_toggled(bool arg1)
{
    if(!arg1) return;
    if(!m_pView) return;
    m_pView->SetAdaptWindows(CFrmViewer::AspectRation);
}

void MainWindow::on_actionOriginal_O_toggled(bool arg1)
{
    if(!arg1) return;
    if(!m_pView) return;
    m_pView->SetAdaptWindows(CFrmViewer::Original);
}

void MainWindow::slotAdaptWindows(const CFrmViewer::ADAPT_WINDOWS aw)
{
    QString t = "Disable";
    m_pGBView->setEnabled(true);
    switch (aw) {
    case CFrmViewer::Auto:
    case CFrmViewer::Original:
        ui->actionOriginal_O->setChecked(true);
        t = "Original";
        break;
    case CFrmViewer::Zoom:
        ui->actionZoom_Z->setChecked(true);
        t = "Original";
        break;
    case CFrmViewer::AspectRation:
        ui->actionKeep_AspectRation_K->setChecked(true);
        t = "AspectRation";
        break;
    case CFrmViewer::Disable:
        m_pGBView->setEnabled(false);   
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

void MainWindow::on_actionOpen_O_triggered()
{
    QString file = RabbitCommon::CDir::GetOpenFileName(this,
                                  tr("Open rabbit remote control file"),
           RabbitCommon::CDir::Instance()->GetDirUserData(), 
           tr("Rabbit remote control Files (*.rrc);;All files(*.*)"));
    if(file.isEmpty()) return;
    CConnecter* p = m_ManageConnecter.LoadConnecter(file);
    if(nullptr == p) return;

    bool check = connect(p, SIGNAL(sigConnected()),
                         this, SLOT(slotConnected()));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigError(const int, const QString &)),
                    this, SLOT(slotError(const int, const QString&)));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigInformation(const QString&)),
                    this, SLOT(slotInformation(const QString&)));
    Q_ASSERT(check);
    
    int nRet = p->OpenDialogSettings(this);
    switch(nRet)
    {
    case QDialog::Rejected:
        delete p;
        break;
    case QDialog::Accepted:
        QString szFile = RabbitCommon::CDir::Instance()->GetDirUserData()
                + QDir::separator()
                + p->Protol()
                + "_" + p->Name() + "_"
                + p->GetServerName().replace(":", "_")
                + ".rrc";
        m_ManageConnecter.SaveConnecter(szFile, p);
        Connect(p);
        break;
    }
    
//    Connect(p);
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

    bool check = connect(p, SIGNAL(sigConnected()),
                         this, SLOT(slotConnected()));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigError(const int, const QString &)),
                    this, SLOT(slotError(const int, const QString&)));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigInformation(const QString&)),
                    this, SLOT(slotInformation(const QString&)));
    Q_ASSERT(check);
    
    int nRet = p->OpenDialogSettings(this);
    switch(nRet)
    {
    case QDialog::Rejected:
        delete p;
        break;
    case QDialog::Accepted:
        QString szFile = RabbitCommon::CDir::Instance()->GetDirUserData()
                + QDir::separator()
                + p->Protol()
                + "_" + p->Name() + "_"
                + p->GetServerName().replace(":", "_")
                + ".rrc";
        m_ManageConnecter.SaveConnecter(szFile, p);
        Connect(p);
        break;
    }
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

    slotInformation(tr("Connected to ") + p->GetServerName());
}

int MainWindow::Connect(CConnecter* p)
{
    if(!p->GetServerName().isEmpty())
        slotInformation(tr("Connecting to ") + p->GetServerName());
    if(m_pView)
    {
        CFrmViewer::ADAPT_WINDOWS aw = CFrmViewer::Zoom;
        if(ui->actionOriginal_O->isChecked())
            aw = CFrmViewer::Original;
        else if(ui->actionZoom_Z->isChecked())
            aw = CFrmViewer::Zoom;
        else if(ui->actionKeep_AspectRation_K->isChecked())
            aw = CFrmViewer::AspectRation;
        m_pView->SetAdaptWindows(aw, p->GetViewer());
        m_pView->AddView(p->GetViewer());
        m_pView->SetWidowsTitle(p->GetViewer(), p->GetServerName()); 
    }
    
    m_Connecters.push_back(p);
    bool check = connect(p, SIGNAL(sigDisconnected()),
                         this, SLOT(slotDisconnected()));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigServerName(const QString&)),
                         this, SLOT(slotUpdateServerName(const QString&)));
    Q_ASSERT(check);
    
    p->Connect();
    
    return 0;
}

void MainWindow::slotCloseView(const QWidget* pView)
{
    if(!pView) return;
    foreach(auto c, m_Connecters)
    {
        if(c->GetViewer() == pView)
            c->DisConnect();
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
            //TODO: c->deleteLater();
            delete c;
            m_Connecters.removeAll(c);
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
    m_Style.slotStyle();
}

void MainWindow::on_actionDefaultStyle_D_triggered()
{
    m_Style.SetDefaultStyle();
}

int MainWindow::onProcess(const QString &id, CPluginFactory *pFactory)
{
    Q_UNUSED(id);
    // Connect menu and toolbar
    QAction* p = ui->menuConnect_C->addAction(pFactory->Protol() + ": " + pFactory->Name(), this, SLOT(slotConnect()));
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
