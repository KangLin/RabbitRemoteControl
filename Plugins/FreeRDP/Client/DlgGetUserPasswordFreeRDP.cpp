#include "DlgGetUserPasswordFreeRDP.h"
#include "ui_DlgGetUserPasswordFreeRDP.h"
#include "ParameterNetUI.h"

static int g_CDlgGetUserPasswordFreeRDP = qRegisterMetaType<CDlgGetUserPasswordFreeRDP>();

CDlgGetUserPasswordFreeRDP::CDlgGetUserPasswordFreeRDP(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgGetUserPasswordFreeRDP),
    m_pConnecter(nullptr),
    m_pParameter(nullptr)
{
    ui->setupUi(this);
}

CDlgGetUserPasswordFreeRDP::CDlgGetUserPasswordFreeRDP(const CDlgGetUserPasswordFreeRDP& dlg)
{
    m_pConnecter = dlg.m_pConnecter;
    m_pParameter = dlg.m_pParameter;
}

CDlgGetUserPasswordFreeRDP::~CDlgGetUserPasswordFreeRDP()
{
    delete ui;
}

void CDlgGetUserPasswordFreeRDP::SetContext(void *pContext)
{
}

void CDlgGetUserPasswordFreeRDP::SetConnecter(CConnecter *pConnecter)
{
    m_pConnecter = qobject_cast<CConnecterFreeRDP*>(pConnecter);
    if(!m_pConnecter) return;

    m_pParameter = qobject_cast<CParameterFreeRDP*>(m_pConnecter->GetParameter());
    Q_ASSERT(m_pParameter);
    if(!m_pParameter) return;

    ui->lbText->setText(tr("Set password for %1").arg(m_pConnecter->Name()));
    ui->leDomain->setText(m_pParameter->GetDomain());
    ui->wUser->SetParameter(&m_pParameter->m_Net.m_User);
}

void CDlgGetUserPasswordFreeRDP::on_pbOK_clicked()
{
    Q_ASSERT(m_pParameter);
    ui->wUser->slotAccept();
    m_pParameter->SetDomain(ui->leDomain->text().toStdString().c_str());
    emit m_pConnecter->sigUpdateParameters(m_pConnecter);
    accept();
}

void CDlgGetUserPasswordFreeRDP::on_pbCancel_clicked()
{
    reject();
}
