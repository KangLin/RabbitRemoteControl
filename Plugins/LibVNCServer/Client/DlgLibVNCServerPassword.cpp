#include "DlgLibVNCServerPassword.h"
#include "ui_DlgLibVNCServerPassword.h"

static int g_CDlgGetUserPassword = qRegisterMetaType<CDlgLibVNCServerPassword>();

CDlgLibVNCServerPassword::CDlgLibVNCServerPassword(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgLibVNCServerPassword),
    m_pConnecter(nullptr),
    m_pParameter(nullptr)
{
    ui->setupUi(this);
}

CDlgLibVNCServerPassword::~CDlgLibVNCServerPassword()
{
    delete ui;
}

CDlgLibVNCServerPassword::CDlgLibVNCServerPassword(const CDlgLibVNCServerPassword& dlg)
{
    m_pConnecter = dlg.m_pConnecter;
    m_pParameter = dlg.m_pParameter;
}

void CDlgLibVNCServerPassword::SetContext(void *pContext)
{
}

void CDlgLibVNCServerPassword::SetConnecter(CConnecter *pConnecter)
{
    m_pConnecter = qobject_cast<CConnecterLibVNCServer*>(pConnecter);
    if(!m_pConnecter) return;

    m_pParameter = qobject_cast<CParameterLibVNCServer*>(m_pConnecter->GetParameter());
}

void CDlgLibVNCServerPassword::showEvent(QShowEvent *event)
{
    ui->lbText->setText(tr("Set password for %1").arg(m_pConnecter->Name()));
    ui->leUser->setText(m_pParameter->GetUser());
    ui->lePassword->setText(m_pParameter->GetPassword());
    ui->cbSavePassword->setChecked(m_pParameter->GetSavePassword());
}

void CDlgLibVNCServerPassword::on_pbOK_clicked()
{
    m_pParameter->SetUser(ui->leUser->text());
    m_pParameter->SetPassword(ui->lePassword->text());
    m_pParameter->SetSavePassword(ui->cbSavePassword->isChecked());
    emit m_pConnecter->sigUpdateParameters(m_pConnecter);
    accept();
}

void CDlgLibVNCServerPassword::on_pbCancel_clicked()
{
    reject();
}
