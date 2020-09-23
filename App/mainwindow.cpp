#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "FrmUpdater/FrmUpdater.h"
#include "DlgAbout/DlgAbout.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    CFrmUpdater updater;
    ui->actionUpdate_U->setIcon(updater.windowIcon());
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
