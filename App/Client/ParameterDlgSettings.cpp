// Author: Kang Lin <kl222@126.com>

#include <QScrollArea>
#include <QFileDialog>
#include <QLoggingCategory>
#include <QScreen>
#include <QApplication>

#include "ParameterDlgSettings.h"
#include "ui_ParameterDlgSettings.h"
#include "mainwindow.h"
#include "RabbitCommonDir.h"

#ifdef HAVE_ICE
#include "Ice.h"
#endif

static Q_LOGGING_CATEGORY(log, "App.MainWindow.Parameter")

CParameterDlgSettings::CParameterDlgSettings(CParameterApp *pPara,
                                             QList<QWidget *> wViewer,
                                             QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CParameterDlgSettings),
    m_pParameters(pPara)
{
    ui->setupUi(this);

    int nWidth = 0;
    int nHeigth = 0;
    foreach(auto p, wViewer) {
        if(!p) continue;
        nWidth = qMax(nWidth, p->frameSize().width());
        nHeigth = qMax(nHeigth, p->frameSize().height());
    }
    bool bScroll = false;
    QScreen* pScreen = QApplication::primaryScreen();
    QSize size = this->size();
    if(nWidth > size.width() || nHeigth > size.height())
        bScroll = true;
    // [connect accepted to slotAccept of widget]
    foreach(auto p, wViewer)
    {
        QString szMsg;
        QWidget* pView = p;
        if(bScroll)
        {
            QScrollArea* pScroll = new QScrollArea(ui->tabWidget);
            if(!pScroll) continue;
            pScroll->setWidget(p);
            pView = pScroll;
        }
        ui->tabWidget->addTab(pView, p->windowIcon(), p->windowTitle());
        bool check = false;
        check = connect(this, SIGNAL(accepted()), p, SLOT(slotAccept()));
        if(!check)
        {
            szMsg = "Class" + QString(p->metaObject()->className())
                + "must has slot slotAccept(), please add it. "
                + "Or the class derived from CParameterUI";
            qCritical(log) << szMsg;
        }
        Q_ASSERT_X(check, "CParameterDlgSettings", szMsg.toStdString().c_str());
    }
    // [connect accepted to slotAccept of widget]

#ifdef HAVE_ICE
    QWidget* pWidget = CICE::Instance()->GetParameterWidget(this);
    if(pWidget)
    {
        ui->tabWidget->addTab(pWidget, pWidget->windowTitle());
        bool check = false;
        check = connect(this, SIGNAL(accepted()), pWidget, SLOT(slotAccept()));
        if(!check)
        {
            szMsg = "Class" + QString(p->metaObject()->className())
            + "must has slot slotAccept(), please add it."
                + "Or the class derived from CParameterUI";
            qCritical(log) << szMsg;
        }
        Q_ASSERT_X(check, "CParameterDlgSettings", szMsg.toStdString().c_str());
    }
#endif

    ui->cbReciverShortCut->setChecked(m_pParameters->GetReceiveShortCut());
    ui->cbSaveMainWindowStatus->setChecked(m_pParameters->GetSaveMainWindowStatus());
    ui->cbResumLasterClose->setChecked(m_pParameters->GetOpenLasterClose());
    ui->cbStartByType->setChecked(m_pParameters->GetStartByType());

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
    ui->cbElided->setChecked(m_pParameters->GetTabElided());

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
    case CParameterApp::SystemTrayIconMenuType::Operate:
        ui->rbSystemTrayIconOperate->setChecked(true);
        break;
    default:
        break;
    }
    ui->cbShowSystemTrayIcon->setChecked(m_pParameters->GetEnableSystemTrayIcon());
    
    ui->cbFavoriteDoubleEdit->setChecked(m_pParameters->GetFavoriteEdit());
    ui->cbMessageBoxDisplayInfo->setChecked(m_pParameters->GetMessageBoxDisplayInformation());
    ui->cbKeepSplitView->setChecked(m_pParameters->GetKeepSplitViewWhenFullScreen());
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
    m_pParameters->SetStartByType(ui->cbStartByType->isChecked());

    if(ui->rbNorth->isChecked()) m_pParameters->SetTabPosition(QTabWidget::North);
    if(ui->rbSouth->isChecked()) m_pParameters->SetTabPosition(QTabWidget::South);
    if(ui->rbEast->isChecked()) m_pParameters->SetTabPosition(QTabWidget::East);
    if(ui->rbWest->isChecked()) m_pParameters->SetTabPosition(QTabWidget::West);
    m_pParameters->SetEnableTabToolTip(ui->cbTabToolTip->isChecked());
    m_pParameters->SetEnableTabIcon(ui->cbTabIcon->isChecked());
    m_pParameters->SetTabElided(ui->cbElided->isChecked());

    m_pParameters->SetRecentMenuMaxCount(ui->sbRecentMenuMaxCount->value());

    //NOTE: The order cannot be changed
    m_pParameters->SetEnableSystemTrayIcon(ui->cbShowSystemTrayIcon->isChecked());
    if(ui->rbSystemTrayIconNo->isChecked())
        m_pParameters->SetSystemTrayIconMenuType(CParameterApp::SystemTrayIconMenuType::No);
    if(ui->rbMenuBar->isChecked())
        m_pParameters->SetSystemTrayIconMenuType(CParameterApp::SystemTrayIconMenuType::MenuBar);
    if(ui->rbSystemTrayIconRecentOpen->isChecked())
        m_pParameters->SetSystemTrayIconMenuType(CParameterApp::SystemTrayIconMenuType::RecentOpen);
    if(ui->rbSystemTrayIconOperate->isChecked())
        m_pParameters->SetSystemTrayIconMenuType(CParameterApp::SystemTrayIconMenuType::Operate);

    m_pParameters->SetFavoriteEdit(ui->cbFavoriteDoubleEdit->isChecked());
    m_pParameters->SetMessageBoxDisplayInformation(ui->cbMessageBoxDisplayInfo->isChecked());
    
    m_pParameters->SetKeepSplitViewWhenFullScreen(ui->cbKeepSplitView->isChecked());
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
