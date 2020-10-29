#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "FrmUpdater/FrmUpdater.h"
#include "DlgAbout/DlgAbout.h"
#include <QScrollArea>
#include <QMessageBox>
#include "Connecter.h"
#include "FrmViewer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    CFrmUpdater updater;
    ui->actionUpdate_U->setIcon(updater.windowIcon());
        
    m_pTab = new QTabWidget(this);
    m_pTab->setTabsClosable(true);
    m_pTab->setUsesScrollButtons(true);
    m_pTab->setMovable(true);
    this->setCentralWidget(m_pTab);
    
    m_ManageConnecter.LoadPlugins();
}

MainWindow::~MainWindow()
{
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
        this->showNormal();
        ui->actionFull_screen_F->setIcon(QIcon(":/image/FullScreen"));
        ui->actionFull_screen_F->setText(tr("Full screen(&F)"));
        ui->actionFull_screen_F->setToolTip(tr("Full screen"));
        ui->actionFull_screen_F->setStatusTip(tr("Full screen"));
        ui->actionFull_screen_F->setWhatsThis(tr("Full screen"));
        
        ui->toolBar->setAllowedAreas(Qt::AllToolBarAreas);
        ui->toolBar->setVisible(ui->actionToolBar_T->isChecked());
        
        ui->statusbar->setVisible(ui->actionStatusBar_S->isChecked());
        ui->menubar->setVisible(true);
        return;
    }
    
    this->showFullScreen();
    ui->actionFull_screen_F->setIcon(QIcon(":/image/ExitFullScreen"));
    ui->actionFull_screen_F->setText(tr("Exit full screen(&E)"));
    ui->actionFull_screen_F->setToolTip(tr("Exit full screen"));
    ui->actionFull_screen_F->setStatusTip(tr("Exit full screen"));
    ui->actionFull_screen_F->setWhatsThis(tr("Exit full screen"));
        
    ui->toolBar->setVisible(true);
    ui->toolBar->setAllowedAreas(Qt::NoToolBarArea);
        
    ui->statusbar->setVisible(false);
    ui->menubar->setVisible(false);
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

void MainWindow::on_actionDisconnect_D_triggered()
{
    auto it = m_Connecters.find(m_pTab->currentIndex());
    if(m_Connecters.end() != it)
        it.value()->DisConnect();
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

    int index = m_pTab->addTab(pScroll, pView->windowTitle());
    m_Connecters[index] = p;
    
    check = connect(pView, SIGNAL(sigSetWindowName(const QString&)),
                         this, SLOT(slotViewTitleChanged(const QString&)));
    Q_ASSERT(check);
    
}

void MainWindow::slotDisconnected()
{
    QMap<int, CConnecter*>::iterator it;
    for(it = m_Connecters.begin(); it != m_Connecters.end(); it++)
    {
        if(it.value() == sender())
        {
            // Close view
            QWidget* pScroll = m_pTab->widget(it.key());
            m_pTab->removeTab(it.key());
            delete pScroll;
            // delete CConnecter*
            m_Connecters.remove(it.key());
            sender()->deleteLater();
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
    auto it = m_Connecters.find(index);
    if(it != m_Connecters.end())
        return it.value();
    return nullptr;
}
