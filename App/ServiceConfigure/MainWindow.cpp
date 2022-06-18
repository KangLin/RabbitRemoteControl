// Author: Kang Lin <kl222@126.com>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "RabbitCommonLog.h"
#include "RabbitCommonStyle.h"
#include "RabbitCommonDir.h"
#ifdef HAVE_ABOUT
#include "DlgAbout/DlgAbout.h"
#endif

#include <QSettings>
#include <QDesktopServices>

#ifdef HAVE_ICE
    #include "Ice.h"
#endif

CMainWindow::CMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_bStart(false)
{
    RabbitCommon::CStyle::Instance()->LoadStyle();
    ui->setupUi(this);
    
    Clean();
    InitTab();
}

CMainWindow::~CMainWindow()
{
    delete ui;
    foreach(auto plugin, m_Plugins.m_Plugins)
        plugin->Stop();
    
    foreach(auto service, m_Service)
        if(service)
            service->deleteLater();
}

void CMainWindow::on_pbCancel_clicked()
{
    qApp->quit();
}

void CMainWindow::on_pbSave_clicked()
{
    emit sigAccept();
    foreach(auto service, m_Service)
    {
        service->SaveConfigure();
    }
#ifdef HAVE_ICE
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(), QSettings::IniFormat);
    CICE::Instance()->GetParameter()->Save(set);
#endif
}

int CMainWindow::InitTab()
{
#ifdef HAVE_ICE
    QWidget* pIce = CICE::Instance()->GetParameterWidget(this);
    if(pIce)
    {
        QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(), QSettings::IniFormat);
        CICE::Instance()->GetParameter()->Load(set);
        // parameter widget must has slotAccept()
        bool check = connect(this, SIGNAL(sigAccept()), pIce, SLOT(slotAccept()));
        if(!check)
        {
            LOG_MODEL_ERROR("MainWindows",
                            "Class %s must has slot slotAccept(), please add it",
                            pIce->metaObject()->className());
        }
        Q_ASSERT(check);
        int nIndex = ui->twConfigure->addTab(pIce, pIce->windowIcon(),
                                             pIce->windowTitle());
        if(-1 == nIndex)
            LOG_MODEL_ERROR("MainWindow", "addTab ice fail");
    }
#endif
    foreach(auto plugin, m_Plugins.m_Plugins)
    {
        CService* pService = plugin->NewService();
        if(!pService) continue;
        pService->LoadConfigure();
        m_Service.push_back(pService);
        QWidget* w = pService->GetParameterWidget(ui->twConfigure);
        if(w)
        {
            // parameter widget must has slotAccept()
            bool check = connect(this, SIGNAL(sigAccept()), w, SLOT(slotAccept()));
            if(!check)
            {
                LOG_MODEL_ERROR("MainWindows",
                                "Class %s must has slot slotAccept(), please add it",
                                w->metaObject()->className());
            }
            Q_ASSERT(check);
            int nIndex = ui->twConfigure->addTab(w, plugin->Icon(),
                                                 plugin->DisplayName());
            if(-1 == nIndex)
                LOG_MODEL_ERROR("MainWindow", "addTab fail");
        }
    }
    
    return 0;
}

int CMainWindow::Clean()
{
    ui->twConfigure->clear();
    return 0;
}

void CMainWindow::on_actionStart_triggered()
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

int CMainWindow::SetStatusText(QString szText)
{
    ui->actionStart->setToolTip(szText);
    ui->actionStart->setStatusTip(szText);
    ui->actionStart->setWhatsThis(szText);
    return 0;
}

void CMainWindow::on_actionAbout_triggered()
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

void CMainWindow::on_actionDefault_triggered()
{
    RabbitCommon::CStyle::Instance()->slotSetDefaultStyle();
}

void CMainWindow::on_actionOpen_triggered()
{
    RabbitCommon::CStyle::Instance()->slotStyle();
}

void CMainWindow::on_actionOpen_folder_triggered()
{
    QString szFolder;
    QString szFile;
    szFile = RabbitCommon::CDir::Instance()->GetFileUserConfigure();
    QSettings set(szFile, QSettings::IniFormat);
    szFolder = set.value("Configure/Folder",
                 RabbitCommon::CDir::Instance()->GetDirUserConfig()).toString();
    if(szFolder.isEmpty()) return;
    QDesktopServices::openUrl(QUrl::fromLocalFile(szFolder));
}
