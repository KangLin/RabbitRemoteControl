#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "RabbitCommonLog.h"
#ifdef HAVE_ABOUT
#include "DlgAbout/DlgAbout.h"
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_bStart(false)
{
    ui->setupUi(this);
    
    Clean();
    Init();
}

MainWindow::~MainWindow()
{
    delete ui;
    foreach(auto plugin, m_Plugins.m_Plugins)
        plugin->Stop();
    
    foreach(auto service, m_Service)
        if(service)
            service->deleteLater();
}

void MainWindow::on_pbCancel_clicked()
{
    qApp->quit();
}

void MainWindow::on_pbSave_clicked()
{
    emit sigSave();
    foreach(auto service, m_Service)
    {
        service->SaveConfigure();
    }
}

int MainWindow::Init()
{
    foreach(auto plugin, m_Plugins.m_Plugins)
    {
        CService* pService = plugin->NewService();
        if(!pService) continue;
        pService->LoadConfigure();
        m_Service.push_back(pService);
        QWidget* w = pService->GetParameterWidget();
        if(w)
        {
            // parameter widget must has slotSave()
            bool check = connect(this, SIGNAL(sigSave()), w, SLOT(slotSave()));
            if(!check)
            {
                LOG_MODEL_ERROR("MainWindows",
                                "Class %s must has slot slotSave(), please add it",
                                w->metaObject()->className());
            }
            Q_ASSERT(check);
            int nIndex = ui->twConfigure->addTab(w, plugin->DisplayName());
            if(-1 == nIndex)
                LOG_MODEL_ERROR("MainWindow", "addTab fail");
        }
    }
    return 0;
}

int MainWindow::Clean()
{
    ui->twConfigure->clear();
    return 0;
}

void MainWindow::on_actionStart_triggered()
{
    foreach(auto plugin, m_Plugins.m_Plugins)
    {
        if(m_bStart)
            plugin->Stop();
        else
            plugin->Start();
    }
    if(m_bStart)
    {
        ui->actionStart->setIcon(QIcon(":/image/Start"));
        ui->actionStart->setText(tr("Start"));
        SetStatusText(tr("Start service"));
        ui->actionStart->setChecked(false);
    }
    else
    {
        ui->actionStart->setIcon(QIcon(":/image/Stop"));
        ui->actionStart->setText(tr("Stop"));
        SetStatusText(tr("Stop service"));
        ui->actionStart->setChecked(true);
    }
    m_bStart = !m_bStart;
}

int MainWindow::SetStatusText(QString szText)
{
    ui->actionStart->setToolTip(szText);
    ui->actionStart->setStatusTip(szText);
    ui->actionStart->setWhatsThis(szText);
    return 0;
}

void MainWindow::on_actionAbout_triggered()
{
#ifdef HAVE_ABOUT
    CDlgAbout *about = new CDlgAbout(this);
    about->m_AppIcon = QImage(":/image/Option");
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

