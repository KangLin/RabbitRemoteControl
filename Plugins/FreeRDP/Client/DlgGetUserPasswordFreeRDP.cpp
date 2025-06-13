#include "DlgGetUserPasswordFreeRDP.h"
#include "ui_DlgGetUserPasswordFreeRDP.h"
#include "ParameterNetUI.h"

static int g_CDlgGetUserPasswordFreeRDP = qRegisterMetaType<CDlgGetUserPasswordFreeRDP>();

CDlgGetUserPasswordFreeRDP::CDlgGetUserPasswordFreeRDP(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgGetUserPasswordFreeRDP),
    m_pParameter(nullptr)
{
    ui->setupUi(this);
}

CDlgGetUserPasswordFreeRDP::CDlgGetUserPasswordFreeRDP(const CDlgGetUserPasswordFreeRDP& dlg)
{
    m_pParameter = dlg.m_pParameter;
}

CDlgGetUserPasswordFreeRDP::~CDlgGetUserPasswordFreeRDP()
{
    delete ui;
}

void CDlgGetUserPasswordFreeRDP::SetContext(void *pContext)
{
    m_pParameter = (CParameterFreeRDP*)(pContext);
    Q_ASSERT(m_pParameter);
    if(!m_pParameter) return;
    
    auto &net = m_pParameter->m_Net;
    ui->lbText->setText(tr("Set password for %1:%2").arg(net.GetHost()).arg(net.GetPort()));
    ui->leDomain->setText(m_pParameter->GetDomain());
    ui->wUser->SetParameter(&net.m_User);
}

void CDlgGetUserPasswordFreeRDP::on_pbOK_clicked()
{
    Q_ASSERT(m_pParameter);
    ui->wUser->Accept();
    m_pParameter->SetDomain(ui->leDomain->text().toStdString().c_str());
    emit m_pParameter->sigChanged();
    accept();
}

void CDlgGetUserPasswordFreeRDP::on_pbCancel_clicked()
{
    reject();
}
