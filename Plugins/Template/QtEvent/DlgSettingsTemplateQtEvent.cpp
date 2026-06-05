// Author: Kang Lin <kl222@126.com>

#include "ParameterTemplateQtEvent.h"
#include "DlgSettingsTemplateQtEvent.h"
#include "ui_DlgSettingsTemplateQtEvent.h"

CDlgSettingsTemplateQtEvent::CDlgSettingsTemplateQtEvent(CParameterTemplateQtEvent *pPara, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgSettingsTemplateQtEvent)
    , m_pPara(pPara)
{
    ui->setupUi(this);
}

CDlgSettingsTemplateQtEvent::~CDlgSettingsTemplateQtEvent()
{
    delete ui;
}

void CDlgSettingsTemplateQtEvent::accept()
{
    // TODO: Accept parameters

    QDialog::accept();
}