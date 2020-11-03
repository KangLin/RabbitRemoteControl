#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "FrmUpdater/FrmUpdater.h"
#include "DlgAbout/DlgAbout.h"
#include <QScrollArea>
#include <QMessageBox>
#include "Connecter.h"
#include "FrmViewer.h"
#include <QTabBar>
#include "FrmFullScreenToolBar.h"
#include <QScreen>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      m_pFullScreenToolBar(nullptr)
{
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
    this->setCentralWidget(m_pTab);

    m_ManageConnecter.LoadPlugins();
}

MainWindow::~MainWindow()
{
    foreach (auto it, m_Connecters) {
        it->DisConnect();
    }
    
    if(m_pFullScreenToolBar) m_pFullScreenToolBar->close();
    
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
        this->showNormal();

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
        return;
    }

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

void MainWindow::on_actionOriginal_O_triggered()
{
}

void MainWindow::on_actionZoom_Z_triggered()
{
}

void MainWindow::on_actionKeep_AspectRation_K_triggered()
{
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

void MainWindow::on_actionConnect_C_triggered()
{
    CConnecter* p = m_ManageConnecter.CreateConnecter("VNC");
    if(nullptr == p) return;

    bool check = connect(p, SIGNAL(sigConnected()),
                         this, SLOT(slotConnected()));
    Q_ASSERT(check);
    
    QDialog* pDlg = p->GetDialogSettings();
    int nRet = pDlg->exec();
    switch(nRet)
    {
    case QDialog::Rejected:
        delete p;
        break;
    case QDialog::Accepted:
        p->Connect();
        break;
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
    
    int nIndex = m_pTab->addTab(pScroll, pView->windowTitle());
    m_pTab->setCurrentIndex(nIndex);
    m_Connecters[pView] = p;
    
    check = connect(pView, SIGNAL(sigSetWindowName(const QString&)),
                         this, SLOT(slotViewTitleChanged(const QString&)));
    Q_ASSERT(check);
}

void MainWindow::on_actionDisconnect_D_triggered()
{
    slotTabCloseRequested(m_pTab->currentIndex());
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
                QWidget* pScroll = m_pTab->widget(nIndex);
                m_pTab->removeTab(nIndex);
                delete pScroll;
            }
            // delete CConnecter*
            m_Connecters.remove(it.key());
            it.value()->deleteLater();
            break;
        }
    }
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

CFrmViewer* MainWindow::GetViewer(int index)
{
    QScrollArea* pScroll = dynamic_cast<QScrollArea*>(m_pTab->widget(index));
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
