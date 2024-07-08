#include "DlgGetPasswordRabbitVNC.h"
#include "ui_DlgGetPasswordRabbitVNC.h"

static int g_CDlgGetPasswordRabbitVNC = qRegisterMetaType<CDlgGetPasswordRabbitVNC>();

CDlgGetPasswordRabbitVNC::CDlgGetPasswordRabbitVNC(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgGetPasswordRabbitVNC),
    m_pConnecter(nullptr),
    m_pParameter(nullptr)
{
    ui->setupUi(this);
}

CDlgGetPasswordRabbitVNC::~CDlgGetPasswordRabbitVNC()
{
    delete ui;
}

CDlgGetPasswordRabbitVNC::CDlgGetPasswordRabbitVNC(const CDlgGetPasswordRabbitVNC& dlg)
{
    m_pConnecter = dlg.m_pConnecter;
    m_pParameter = dlg.m_pParameter;
}

void CDlgGetPasswordRabbitVNC::SetContext(void *pContext)
{
}

void CDlgGetPasswordRabbitVNC::SetConnecter(CConnecter *pConnecter)
{
    m_pConnecter = qobject_cast<CConnecterRabbitVNC*>(pConnecter);
    if(!m_pConnecter) return;

    m_pParameter = qobject_cast<CParameterRabbitVNC*>(m_pConnecter->GetParameter());
    ui->lbText->setText(tr("Set password for %1").arg(m_pConnecter->Name()));
    ui->wUser->SetParameter(&m_pParameter->m_Net.m_User);
}

void CDlgGetPasswordRabbitVNC::on_pbOK_clicked()
{
    ui->wUser->slotAccept();
    emit m_pConnecter->sigUpdateParameters(m_pConnecter);
    accept();
}

void CDlgGetPasswordRabbitVNC::on_pbCancel_clicked()
{
    reject();
}
