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
}

void CDlgGetPasswordRabbitVNC::showEvent(QShowEvent *event)
{
    ui->lbText->setText(tr("Set password for %1").arg(m_pConnecter->Name()));
    ui->lePassword->setText(m_pParameter->GetPassword());
    ui->cbSavePassword->setChecked(m_pParameter->GetSavePassword());
    ui->leUser->setText(m_pParameter->GetUser());
}

void CDlgGetPasswordRabbitVNC::on_pbOK_clicked()
{
    m_pParameter->SetUser(ui->leUser->text());
    m_pParameter->SetPassword(ui->lePassword->text());
    m_pParameter->SetSavePassword(ui->cbSavePassword->isChecked());
    emit m_pConnecter->sigUpdateParameters(m_pConnecter);
    accept();
}

void CDlgGetPasswordRabbitVNC::on_pbCancel_clicked()
{
    reject();
}
