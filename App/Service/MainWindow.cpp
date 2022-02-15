#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "RabbitCommonLog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    foreach(auto service, m_Plugins.m_Plugins)
    {
        QWidget* w = service->NewService()->GetParameterWidget();
        if(w)
        {
            int nIndex = ui->twConfigure->addTab(w, service->Name());
            if(-1 == nIndex)
                LOG_MODEL_ERROR("MainWindow", "addTab fail");
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
