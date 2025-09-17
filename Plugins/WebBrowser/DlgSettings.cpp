// Author: Kang Lin <kl222@126.com>

#include <QFileDialog>
#include <QLoggingCategory>
#include "DlgSettings.h"
#include "ui_DlgSettings.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.Parameter")
CDlgSettings::CDlgSettings(CParameterWebBrowser *para, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgSettings)
    , m_pPara(para)
    , m_pSearchModel(nullptr)
{
    ui->setupUi(this);
    ui->leName->setText(m_pPara->GetName());
    ui->leHomeUrl->setText(m_pPara->GetHomeUrl());
    if(m_pPara->GetTabUrl().isEmpty() && !m_pPara->GetHomeUrl().isEmpty())
        ui->leTabUrl->setText(m_pPara->GetHomeUrl());
    else
        ui->leTabUrl->setText(m_pPara->GetTabUrl());
    ui->leDownloadFolder->setText(m_pPara->GetDownloadFolder());
    ui->cbDownloadShowWindow->setChecked(m_pPara->GetShowDownloadManager());
    ui->cbDownloadSave->setChecked(m_pPara->GetShowDownloadLocation());
    ui->cbOpenPreious->setChecked(m_pPara->GetOpenPrevious());
    ui->cbClearCookie->setChecked(m_pPara->GetClearCookie());
    ui->cbClearHttpCache->setChecked(m_pPara->GetClearHttpCache());
    ui->cbPrintFinishedPrompt->setChecked(m_pPara->GetPromptPrintFinished());

    m_pSearchModel = new QStandardItemModel(this);
    auto searchList = m_pPara->GetSearchEngineList();
    foreach(auto i, searchList)
    {
        QStandardItem* item = new QStandardItem(i);
        m_pSearchModel->appendRow(item);
    }
    ui->lstSearchEngine->setModel(m_pSearchModel);
    int index = searchList.indexOf(m_pPara->GetSearchEngine());
    QModelIndex indexModel;
    indexModel = m_pSearchModel->index(index, 0);
    ui->lstSearchEngine->setCurrentIndex(indexModel);
}

CDlgSettings::~CDlgSettings()
{
    delete ui;
}

void CDlgSettings::accept()
{
    m_pPara->SetName(ui->leName->text());
    m_pPara->SetHomeUrl(ui->leHomeUrl->text());
    m_pPara->SetTabUrl(ui->leTabUrl->text());
    m_pPara->SetDownloadFolder(ui->leDownloadFolder->text());
    m_pPara->SetShowDownloadManager(ui->cbDownloadShowWindow->isChecked());
    m_pPara->SetShowDownloadLocation(ui->cbDownloadSave->isChecked());
    m_pPara->SetOpenPrevious(ui->cbOpenPreious->isChecked());
    m_pPara->SetClearHttpCache(ui->cbClearHttpCache->isChecked());
    m_pPara->SetClearCookie(ui->cbClearCookie->isChecked());
    m_pPara->SetPromptPrintFinished(ui->cbPrintFinishedPrompt->isChecked());
    auto index = ui->lstSearchEngine->currentIndex();
    if(index.isValid()) {
        auto search = m_pSearchModel->item(index.row())->text();
        m_pPara->SetSearchEngine(search);
    }
    QDialog::accept();
}

void CDlgSettings::on_leHomeUrl_editingFinished()
{
    if(ui->leTabUrl->text().isEmpty())
        ui->leTabUrl->setText(ui->leHomeUrl->text());
}

void CDlgSettings::on_pbDownloadFolder_clicked()
{
    QString szDir = ui->leDownloadFolder->text();
    if(szDir.isEmpty())
        szDir = m_pPara->GetDownloadFolder();
    szDir = QFileDialog::getExistingDirectory(this, tr("Download folder"), szDir);
    if(!szDir.isEmpty())
        ui->leDownloadFolder->setText(szDir);
}
