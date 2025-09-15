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
{
    ui->setupUi(this);
    ui->leName->setText(m_pPara->GetName());
    ui->leHomeUrl->setText(m_pPara->GetHomeUrl());
    if(m_pPara->GetTabUrl().isEmpty() && !m_pPara->GetHomeUrl().isEmpty())
        ui->leTabUrl->setText(m_pPara->GetHomeUrl());
    else
        ui->leTabUrl->setText(m_pPara->GetTabUrl());
    ui->leDownloadFolder->setText(m_pPara->GetDownloadFolder());
    ui->cbOpenPreious->setChecked(m_pPara->GetOpenPrevious());
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
    m_pPara->SetOpenPrevious(ui->cbOpenPreious->isChecked());
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
