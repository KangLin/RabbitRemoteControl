#include "FrmParameterViewer.h"
#include "ui_FrmParameterViewer.h"

CFrmParameterViewer::CFrmParameterViewer(CParameterViewer *pParameter, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFrmParameterViewer),
    m_pPara(pParameter)
{
    ui->setupUi(this);

    ui->cbHookKeyboard->setChecked(m_pPara->GetHookKeyboard());
}

CFrmParameterViewer::~CFrmParameterViewer()
{
    delete ui;
}

void CFrmParameterViewer::slotAccept()
{
    m_pPara->SetHookKeyboard(ui->cbHookKeyboard->isChecked());
}
