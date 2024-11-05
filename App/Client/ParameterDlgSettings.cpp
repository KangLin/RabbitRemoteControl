// Author: Kang Lin <kl222@126.com>

#include "ParameterDlgSettings.h"
#include "ui_ParameterDlgSettings.h"
#include "mainwindow.h"
#include "RabbitCommonDir.h"

#ifdef HAVE_ICE
#include "Ice.h"
#endif

#include <QFileDialog>
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "App.MainWindow.Parameter")

CParameterDlgSettings::CParameterDlgSettings(CParameterApp *pPara,
                                             QList<QWidget *> wViewer,
                                             QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CParameterDlgSettings),
    m_pParameters(pPara)
{
    ui->setupUi(this);
    
    // [connect accepted to slotAccept of widget]
    foreach(auto p, wViewer)
    {
        ui->tabWidget->addTab(p, p->windowIcon(), p->windowTitle());
        bool check = false;
        check = connect(this, SIGNAL(accepted()), p, SLOT(slotAccept()));
        if(!check)
        {
            qCritical(log) << "Class" << p->metaObject()->className()
            << "must has slot slotAccept(), please add it";
        }
        Q_ASSERT(check);
    }
    // [connect accepted to slotAccept of widget]

#ifdef HAVE_ICE
    QWidget* pWidget = CICE::Instance()->GetParameterWidget(this);
    if(pWidget)
    {
        ui->tabWidget->addTab(pWidget, pWidget->windowTitle());
        bool check = false;
        check = connect(this, SIGNAL(accepted()),
                        pWidget, SLOT(slotAccept()));
        if(!check)
        {
            qCritical(log) << "Class" << pWidget->metaObject()->className()
            << "must has slot slotAccept(), please add it";
        }
        Q_ASSERT(check);
    }
#endif

    ui->cbReciverShortCut->setChecked(m_pParameters->GetReceiveShortCut());
    ui->cbSaveMainWindowStatus->setChecked(m_pParameters->GetSaveMainWindowStatus());
    ui->cbResumLasterClose->setChecked(m_pParameters->GetOpenLasterClose());
    
    switch (m_pParameters->GetTabPosition()) {
    case QTabWidget::North:
        ui->rbNorth->setChecked(true);
        break;
    case QTabWidget::South:
        ui->rbSouth->setChecked(true);
        break;
    case QTabWidget::West:
        ui->rbWest->setChecked(true);
        break;
    case QTabWidget::East:
        ui->rbEast->setChecked(true);
        break;
    }
    ui->cbTabToolTip->setChecked(m_pParameters->GetEnableTabToolTip());
    ui->cbTabIcon->setChecked(m_pParameters->GetEnableTabIcon());

    ui->sbRecentMenuMaxCount->setValue(m_pParameters->GetRecentMenuMaxCount());

    switch (m_pParameters->GetSystemTrayIconMenuType()) {
    case CParameterApp::SystemTrayIconMenuType::No:
        ui->rbSystemTrayIconNo->setChecked(true);
        break;
    case CParameterApp::SystemTrayIconMenuType::MenuBar:
        ui->rbMenuBar->setChecked(true);
        break;
    case CParameterApp::SystemTrayIconMenuType::RecentOpen:
        ui->rbSystemTrayIconRecentOpen->setChecked(true);
        break;
    case CParameterApp::SystemTrayIconMenuType::Remote:
        ui->rbSystemTrayIconRemote->setChecked(true);
        break;
    default:
        break;
    }
    ui->cbShowSystemTrayIcon->setChecked(m_pParameters->GetEnableSystemTrayIcon());
    
    ui->cbFavoriteDoubleEdit->setChecked(m_pParameters->GetFavoriteEdit());
    ui->cbMessageBoxDisplayInfo->setChecked(m_pParameters->GetMessageBoxDisplayInformation());
}

CParameterDlgSettings::~CParameterDlgSettings()
{
    delete ui;
}

void CParameterDlgSettings::on_pbOk_clicked()
{
    if(!m_pParameters) return;

    m_pParameters->SetReceiveShortCut(ui->cbReciverShortCut->isChecked());
    m_pParameters->SetSaveMainWindowStatus(ui->cbSaveMainWindowStatus->isChecked());
    m_pParameters->SetOpenLasterClose(ui->cbResumLasterClose->isChecked());
    
    if(ui->rbNorth->isChecked()) m_pParameters->SetTabPosition(QTabWidget::North);
    if(ui->rbSouth->isChecked()) m_pParameters->SetTabPosition(QTabWidget::South);
    if(ui->rbEast->isChecked()) m_pParameters->SetTabPosition(QTabWidget::East);
    if(ui->rbWest->isChecked()) m_pParameters->SetTabPosition(QTabWidget::West);
    m_pParameters->SetEnableTabToolTip(ui->cbTabToolTip->isChecked());
    m_pParameters->SetEnableTabIcon(ui->cbTabIcon->isChecked());

    m_pParameters->SetRecentMenuMaxCount(ui->sbRecentMenuMaxCount->value());

    //NOTE: The order cannot be changed
    m_pParameters->SetEnableSystemTrayIcon(ui->cbShowSystemTrayIcon->isChecked());
    if(ui->rbSystemTrayIconNo->isChecked())
        m_pParameters->SetSystemTrayIconMenuType(CParameterApp::SystemTrayIconMenuType::No);
    if(ui->rbMenuBar->isChecked())
        m_pParameters->SetSystemTrayIconMenuType(CParameterApp::SystemTrayIconMenuType::MenuBar);
    if(ui->rbSystemTrayIconRecentOpen->isChecked())
        m_pParameters->SetSystemTrayIconMenuType(CParameterApp::SystemTrayIconMenuType::RecentOpen);
    if(ui->rbSystemTrayIconRemote->isChecked())
        m_pParameters->SetSystemTrayIconMenuType(CParameterApp::SystemTrayIconMenuType::Remote);

    m_pParameters->SetFavoriteEdit(ui->cbFavoriteDoubleEdit->isChecked());
    m_pParameters->SetMessageBoxDisplayInformation(ui->cbMessageBoxDisplayInfo->isChecked());

    accept();
}

void CParameterDlgSettings::on_pbNo_clicked()
{
    reject();
}

void CParameterDlgSettings::on_cbShowSystemTrayIcon_toggled(bool checked)
{
    ui->gbSystemTrayIconContextMenu->setEnabled(checked);
}
