// Author: Kang Lin <kl222@126.com>

#include "ParameterTemplateDesktop.h"
#include "DlgSettingsTemplateDesktop.h"
#include "ui_DlgSettingsTemplateDesktop.h"

CDlgSettingsTemplateDesktop::CDlgSettingsTemplateDesktop(CParameterTemplateDesktop *pPara, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgSettingsTemplateDesktop)
    , m_pPara(pPara)
{
    ui->setupUi(this);
}

CDlgSettingsTemplateDesktop::~CDlgSettingsTemplateDesktop()
{
    delete ui;
}

void CDlgSettingsTemplateDesktop::accept()
{
    // TODO: Accept parameters

    QDialog::accept();
}