// Author: Kang Lin <kl222@126.com>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "FrmStyle.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonTools.h"
#ifdef HAVE_ABOUT
#include "DlgAbout.h"
#endif

#include <QSettings>
#include <QDesktopServices>
#include <QLoggingCategory>

#ifdef HAVE_ICE
    #include "Ice.h"
#endif

Q_DECLARE_LOGGING_CATEGORY(App)

CMainWindow::CMainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_bStart(false)
{
    ui->setupUi(this);
    
    ui->menuService->addMenu(RabbitCommon::CTools::GetLogMenu(ui->menuService));
    
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
    QWidget* pIce = nullptr;
#ifdef HAVE_ICE
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(), QSettings::IniFormat);
    CICE::Instance()->GetParameter()->Load(set);
    pIce = CICE::Instance()->GetParameterWidget(ui->twConfigure);
    if(pIce)
    {
        // parameter widget must has slotAccept()
        bool check = connect(this, SIGNAL(sigAccept()), pIce, SLOT(slotAccept()));
        if(!check)
        {
            qCritical(App) << "Class" << pIce->metaObject()->className()
                            << "must has slot slotAccept(), please add it";
        }
        Q_ASSERT(check);
        int nIndex = ui->twConfigure->addTab(pIce, pIce->windowIcon(),
                                             pIce->windowTitle());
        if(-1 == nIndex)
            qCritical(App) << "addTab ice fail";
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
                if(pIce)
                    qCritical(App) << "Class" << pIce->metaObject()->className()
                                   << "must has slot slotAccept(), please add it";
            }
            Q_ASSERT(check);
            int nIndex = ui->twConfigure->addTab(w, plugin->Icon(),
                                                 plugin->DisplayName());
            if(-1 == nIndex)
                qCritical(App) << "addTab fail";
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
    
#ifdef HAVE_ICE
    if(m_bStart)
        CICE::Instance()->slotStop();
    else
        CICE::Instance()->slotStart();
#endif
    
    if(m_bStart)
    {
        ui->actionStart->setIcon(QIcon::fromTheme("media-playback-start"));
        ui->actionStart->setText(tr("Start"));
        SetStatusText(tr("Start service"));
        ui->actionStart->setChecked(false);
    }
    else
    {
        ui->actionStart->setIcon(QIcon::fromTheme("media-playback-stop"));
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
    QIcon icon = QIcon::fromTheme("system-settings");
    if(icon.isNull()) return;
    auto sizeList = icon.availableSizes();
    if(sizeList.isEmpty()) return;
    QPixmap p = icon.pixmap(*sizeList.begin());
    about->m_AppIcon = p.toImage();
    about->m_szCopyrightStartTime = "2020";
    about->m_szVersionRevision = RabbitRemoteControl_REVISION;
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

void CMainWindow::on_actionStyle_S_triggered()
{
    CFrmStyle* s = new CFrmStyle();
    s->show();
}
