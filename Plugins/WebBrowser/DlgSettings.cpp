// Author: Kang Lin <kl222@126.com>

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
    ui->leHomeUrl->setText(m_pPara->GetHomeUrl());
    if(m_pPara->GetTabUrl().isEmpty() && !m_pPara->GetHomeUrl().isEmpty())
        ui->leTabUrl->setText(m_pPara->GetHomeUrl());
    else
        ui->leTabUrl->setText(m_pPara->GetTabUrl());
    ui->cbOpenPreious->setChecked(m_pPara->GetOpenPrevious());
}

CDlgSettings::~CDlgSettings()
{
    delete ui;
}

void CDlgSettings::accept()
{
    m_pPara->SetHomeUrl(ui->leHomeUrl->text());
    m_pPara->SetTabUrl(ui->leTabUrl->text());
    m_pPara->SetOpenPrevious(ui->cbOpenPreious->isChecked());
    QDialog::accept();
}
