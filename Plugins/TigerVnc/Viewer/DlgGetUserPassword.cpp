#include "DlgGetUserPassword.h"
#include "ui_DlgGetUserPassword.h"

static int g_CDlgTigerVNCGetPassword = qRegisterMetaType<CDlgTigerVNCGetPassword>();

CDlgTigerVNCGetPassword::CDlgTigerVNCGetPassword(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgTigerVNCGetPassword),
    m_pConnecter(nullptr),
    m_pParameter(nullptr)
{
    ui->setupUi(this);
}

CDlgTigerVNCGetPassword::~CDlgTigerVNCGetPassword()
{
    delete ui;
}

CDlgTigerVNCGetPassword::CDlgTigerVNCGetPassword(const CDlgTigerVNCGetPassword& dlg)
{
    m_pConnecter = dlg.m_pConnecter;
    m_pParameter = dlg.m_pParameter;
}

void CDlgTigerVNCGetPassword::SetContext(void *pContext)
{
}

void CDlgTigerVNCGetPassword::SetConnecter(CConnecter *pConnecter)
{
    m_pConnecter = qobject_cast<CConnecterTigerVnc*>(pConnecter);
    if(!m_pConnecter) return;

    m_pParameter = qobject_cast<CParameterTigerVnc*>(m_pConnecter->GetParameter());
}

void CDlgTigerVNCGetPassword::showEvent(QShowEvent *event)
{
    ui->lbText->setText(tr("Set password for %1").arg(m_pConnecter->Name()));
    ui->lePassword->setText(m_pParameter->GetPassword());
    ui->cbSavePassword->setChecked(m_pParameter->GetSavePassword());
    ui->leUser->setText(m_pParameter->GetUser());
}

void CDlgTigerVNCGetPassword::on_pbOK_clicked()
{
    m_pParameter->SetUser(ui->leUser->text());
    m_pParameter->SetPassword(ui->lePassword->text());
    m_pParameter->SetSavePassword(ui->cbSavePassword->isChecked());
    emit m_pConnecter->sigUpdateParamters(m_pConnecter);
    accept();
}

void CDlgTigerVNCGetPassword::on_pbCancel_clicked()
{
    reject();
}
