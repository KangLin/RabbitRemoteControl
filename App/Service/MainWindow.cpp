#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "RabbitCommonLog.h"

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
            bool check = connect(this, SIGNAL(sigSave()),
                                 w, SLOT(slotSave()));
            Q_ASSERT(check);
            int nIndex = ui->twConfigure->addTab(w, plugin->Name());
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
        ui->actionStart->setText("Start");
        ui->actionStart->setChecked(false);
    }
    else
    {
        ui->actionStart->setIcon(QIcon(":/image/Stop"));
        ui->actionStart->setText("Stop");
        ui->actionStart->setChecked(true);
    }
    m_bStart = !m_bStart;
}
