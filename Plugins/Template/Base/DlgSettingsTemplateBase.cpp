// Author: Kang Lin <kl222@126.com>

#include "ParameterTemplateBase.h"
#include "DlgSettingsTemplateBase.h"
#include "ui_DlgSettingsTemplateBase.h"

CDlgSettingsTemplateBase::CDlgSettingsTemplateBase(CParameterTemplateBase *pPara, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgSettingsTemplateBase)
    , m_pPara(pPara)
{
    ui->setupUi(this);
}

CDlgSettingsTemplateBase::~CDlgSettingsTemplateBase()
{
    delete ui;
}
