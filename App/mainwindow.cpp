#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "FrmUpdater/FrmUpdater.h"
#include "RabbitCommonDir.h"
#include "DlgAbout/DlgAbout.h"

#include "Connecter.h"
#include "FrmViewer.h"
#include "FrmFullScreenToolBar.h"

#include <QTabBar>
#include <QMessageBox>
//! @author: Kang Lin(kl222@126.com)

#include <QScreen>
#include <QApplication>
#include <QSettings>
#include <QToolButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_Style(this),
      m_pGBView(nullptr),
      m_pTab(nullptr),
      m_pFullScreenToolBar(nullptr)
{
    m_Style.LoadStyle();
    ui->setupUi(this);

    CFrmUpdater updater;
    ui->actionUpdate_U->setIcon(updater.windowIcon());
    
    m_pTab = new QTabWidget(this);
    m_pTab->setTabsClosable(true);
    m_pTab->setUsesScrollButtons(true);
    m_pTab->setMovable(true);
    m_pTab->setFocusPolicy(Qt::NoFocus);
    bool check = connect(m_pTab, SIGNAL(tabCloseRequested(int)),
                    this, SLOT(slotTabCloseRequested(int)));
    Q_ASSERT(check);
    check = connect(m_pTab, SIGNAL(currentChanged(int)),
            this, SLOT(slotCurrentChanged(int)));
    Q_ASSERT(check);

    this->setCentralWidget(m_pTab);

    // Connect menu and toolbar   
    foreach(auto m, m_ManageConnecter.GetManageConnecter())
    {
        QAction* p = ui->menuConnect_C->addAction(m->Name(), this, SLOT(slotConnect()));
        p->setToolTip(m->Description());
        p->setStatusTip(m->Description());
        p->setData(m->Protol());
        p->setIcon(m->Icon());
    }
    
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
    }
    check = connect(m_pGBView, SIGNAL(triggered(QAction*)),
                         this, SLOT(slotZoomChange(QAction*)));
    Q_ASSERT(check);

    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure());
    QString t = set.value("ZoomType").toString();
    if("Original" == t)
        ui->actionOriginal_O->setChecked(true);
    else if("Zoom" == t)
        ui->actionZoom_Z->setChecked(true);
    else if("AspectRation" == t)
        ui->actionKeep_AspectRation_K->setChecked(true);
}

MainWindow::~MainWindow()
{
    foreach (auto it, m_Connecters)
        it->DisConnect();
        
    if(m_pFullScreenToolBar) m_pFullScreenToolBar->close();
    if(m_pGBView) delete m_pGBView;
    
    delete ui;
}

void MainWindow::on_actionAbout_A_triggered()
{
    CDlgAbout about(this);
    about.m_AppIcon = QImage(":/image/App");
    if(about.isHidden())
#if defined (Q_OS_ANDROID)
        about.showMaximized();
#endif
        about.exec();    
}

void MainWindow::on_actionUpdate_U_triggered()
{
    CFrmUpdater* m_pfrmUpdater = new CFrmUpdater();
    m_pfrmUpdater->SetTitle(QImage(":/image/App"));
    m_pfrmUpdater->SetInstallAutoStartup();
#if defined (Q_OS_ANDROID)
    m_pfrmUpdater->showMaximized();
#else
    m_pfrmUpdater->show();
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
    if(this->isFullScreen())
    {
        QTabWidget* pTab = dynamic_cast<QTabWidget*>(this->centralWidget());
        if(pTab)
        {
            pTab->showNormal();
            pTab->tabBar()->show();
        }
        
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
        
        this->showNormal();      
        this->activateWindow();
        return;
    }

    //setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    this->showFullScreen();
    QTabWidget* pTab = dynamic_cast<QTabWidget*>(this->centralWidget());
    if(pTab)
    {
        pTab->tabBar()->hide();
        pTab->showFullScreen();
    }

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
    if(!m_pTab) return;
    QScrollArea* pScroll = GetScrollArea(m_pTab->currentIndex());
    if(!pScroll)
        return;
    pScroll->setWidgetResizable(true);
    
    CFrmViewer* pView = GetViewer(m_pTab->currentIndex());
    if(pView)
        pView->SetAdaptWindows(CFrmViewer::Zoom);
}

void MainWindow::on_actionKeep_AspectRation_K_toggled(bool arg1)
{
    if(!arg1) return;
    if(!m_pTab) return;
    QScrollArea* pScroll = GetScrollArea(m_pTab->currentIndex());
    if(!pScroll)
        return;
    pScroll->setWidgetResizable(true);
    
    CFrmViewer* pView = GetViewer(m_pTab->currentIndex());
    if(pView)
        pView->SetAdaptWindows(CFrmViewer::AspectRation);
}

void MainWindow::slotZoomChange(QAction* action)
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure());
    QString t;
    if(action == ui->actionOriginal_O)
        t = "Original";
    else if(action == ui->actionZoom_Z)
        t = "Zoom";
    else if(action == ui->actionKeep_AspectRation_K)
        t = "AspectRation";
    set.setValue("ZoomType", t);
}

void MainWindow::on_actionOriginal_O_toggled(bool arg1)
{   
    if(!arg1) return;
    if(!m_pTab) return;
    QScrollArea* pScroll = GetScrollArea(m_pTab->currentIndex());
    if(!pScroll)
        return;
    pScroll->setWidgetResizable(false);

    CFrmViewer* pView = GetViewer(m_pTab->currentIndex());
    if(pView)
        pView->SetAdaptWindows(CFrmViewer::Original);
}

void MainWindow::on_actionExit_E_triggered()
{
    qApp->exit();
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
    
    slotInformation(tr("Connecting to ") + p->ServerName());
    p->Connect();
}

void MainWindow::slotConnect()
{
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
    
    QDialog* pDlg = p->GetDialogSettings();
    if(pDlg)
    {
        int nRet = pDlg->exec();
        switch(nRet)
        {
        case QDialog::Rejected:
            delete p;
            break;
        case QDialog::Accepted:
            QString szFile = RabbitCommon::CDir::Instance()->GetDirUserData()
                    + QDir::separator()
                    + p->Protol()
                    + "_"
                    + p->ServerName().replace(":", "_")
                    + ".rrc";
            m_ManageConnecter.SaveConnecter(szFile, p);
            slotInformation(tr("Connecting to ") + p->ServerName());
            p->Connect();
            break;
        }
    } else {
        qWarning() << "The protol[" << p->Protol() << "] don't settings dialog";
    }
}

void MainWindow::slotConnected()
{
    CConnecter* p = dynamic_cast<CConnecter*>(sender());
    if(!p) return;
    
    bool check = connect(p, SIGNAL(sigDisconnected()),
                         this, SLOT(slotDisconnected()));
    Q_ASSERT(check);
    
    CFrmViewer* pView = p->GetViewer();
    QScrollArea* pScroll = new QScrollArea(m_pTab);
    pScroll->setAlignment(Qt::AlignCenter);
    pScroll->setBackgroundRole(QPalette::Dark);
    pScroll->setWidget(pView);
    pScroll->setFocusPolicy(Qt::NoFocus);
    if(ui->actionZoom_Z->isChecked()) {
        pScroll->setWidgetResizable(true);
        pView->SetAdaptWindows(CFrmViewer::Zoom);
    } else if(ui->actionKeep_AspectRation_K->isChecked()) {
        pScroll->setWidgetResizable(true);
        pView->SetAdaptWindows(CFrmViewer::AspectRation);
    }

    int nIndex = m_pTab->addTab(pScroll, pView->windowTitle());
    m_pTab->setCurrentIndex(nIndex);
    m_Connecters[pView] = p;
    
    check = connect(p, SIGNAL(sigServerName(const QString&)),
                         this, SLOT(slotViewTitleChanged(const QString&)));
    Q_ASSERT(check);
    
    slotInformation(tr("Connected to ") + p->ServerName());
}

void MainWindow::on_actionDisconnect_D_triggered()
{
    slotTabCloseRequested(m_pTab->currentIndex());
}

void MainWindow::slotCurrentChanged(int index)
{
    CFrmViewer* pView = GetViewer(index);
    if(!pView) return;
    switch (pView->AdaptWindows()) {
    case CFrmViewer::Auto:
    case CFrmViewer::OriginalCenter:
    case CFrmViewer::Original:
        ui->actionOriginal_O->setChecked(true);
        break;
    case CFrmViewer::Zoom:
        ui->actionZoom_Z->setChecked(true);
        break;
    case CFrmViewer::AspectRation:
        ui->actionKeep_AspectRation_K->setChecked(true);
        break;
    }
}

void MainWindow::slotTabCloseRequested(int index)
{
    CFrmViewer* pView = GetViewer(index);
    auto it = m_Connecters.find(pView);
    if(m_Connecters.end() != it)
        it.value()->DisConnect();
}

void MainWindow::slotDisconnected()
{
    QMap<CFrmViewer*, CConnecter*>::iterator it;
    for(it = m_Connecters.begin(); it != m_Connecters.end(); it++)
    {
        if(it.value() == sender())
        {
            // Close view
            int nIndex = GetViewIndex(it.key());
            if(nIndex >= 0)
            {
                // Delete the scroll, the scroll will delete child widget
                QWidget* pScroll = m_pTab->widget(nIndex);
                m_pTab->removeTab(nIndex);
                delete pScroll;
            }
            // delete CConnecter*
            m_Connecters.remove(it.key());
            sender()->deleteLater();
            break;
        }
    }
}

void MainWindow::slotError(const int nError, const QString &szInfo)
{
    this->statusBar()->showMessage(szInfo);
}

void MainWindow::slotInformation(const QString& szInfo)
{
    this->statusBar()->showMessage(szInfo);
}

void MainWindow::slotViewTitleChanged(const QString& szName)
{
    Q_UNUSED(szName)
    for(int i = 0; i < m_pTab->count(); i++)
    {
        CFrmViewer* pView = GetViewer(i);
        if(pView)
            m_pTab->setTabText(i, pView->windowTitle());
    }
}

QScrollArea* MainWindow::GetScrollArea(int index)
{
    return dynamic_cast<QScrollArea*>(m_pTab->widget(index));
}

CFrmViewer* MainWindow::GetViewer(int index)
{
    QScrollArea* pScroll = GetScrollArea(index);
    if(!pScroll) return nullptr;
    
    return dynamic_cast<CFrmViewer*>(pScroll->widget());
}

CConnecter* MainWindow::GetConnecter(int index)
{
    auto it = m_Connecters.find(GetViewer(index));
    if(it != m_Connecters.end())
        return it.value();
    return nullptr;
}

int MainWindow::GetViewIndex(CFrmViewer *pView)
{
    for(int i = 0; i < m_pTab->count(); i++)
    {
        QScrollArea* pScroll = dynamic_cast<QScrollArea*>(m_pTab->widget(i));
        if(pScroll->widget() == pView)
            return i;
    }
    return -1;
}

void MainWindow::on_actionOpenStyle_O_triggered()
{
    m_Style.slotStyle();
}

void MainWindow::on_actionDefaultStyle_D_triggered()
{
    m_Style.SetDefaultStyle();
}
