#include "DlgGetUserPassword.h"
#include "ui_DlgGetUserPassword.h"

static int g_CDlgGetUserPassword = qRegisterMetaType<CDlgGetUserPassword>();

CDlgGetUserPassword::CDlgGetUserPassword(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgGetUserPassword),
    m_pConnecter(nullptr),
    m_pParameter(nullptr)
{
    ui->setupUi(this);
}

CDlgGetUserPassword::CDlgGetUserPassword(const CDlgGetUserPassword& dlg)
{
    m_pConnecter = dlg.m_pConnecter;
    m_pParameter = dlg.m_pParameter;
}

CDlgGetUserPassword::~CDlgGetUserPassword()
{
    delete ui;
}

void CDlgGetUserPassword::SetContext(void *pContext)
{
}

void CDlgGetUserPassword::SetConnecter(CConnecter *pConnecter)
{
    m_pConnecter = qobject_cast<CConnecterFreeRdp*>(pConnecter);
    if(!m_pConnecter) return;

    m_pParameter = qobject_cast<CParameterFreeRdp*>(m_pConnecter->GetParameter());
}

void CDlgGetUserPassword::showEvent(QShowEvent *event)
{
    Q_ASSERT(m_pParameter);
    if(!m_pParameter) return;
    
    QString szDomain = freerdp_settings_get_string(m_pParameter->m_pSettings, FreeRDP_Domain);
    
    ui->lbText->setText(tr("Set password for %1").arg(m_pConnecter->Name()));
    ui->leDomain->setText(szDomain);
    ui->leUser->setText(m_pParameter->GetUser());
    ui->lePassword->setText(m_pParameter->GetPassword());
    ui->cbSavePassword->setChecked(m_pParameter->GetSavePassword());
    
}

void CDlgGetUserPassword::on_pbOK_clicked()
{
    Q_ASSERT(m_pParameter);
    m_pParameter->SetUser(ui->leUser->text());
    m_pParameter->SetPassword(ui->lePassword->text());
    m_pParameter->SetSavePassword(ui->cbSavePassword->isChecked());
    freerdp_settings_set_string(m_pParameter->m_pSettings, FreeRDP_Domain,
                                ui->leDomain->text().toStdString().c_str());
    emit m_pConnecter->sigUpdateParamters(m_pConnecter);
    accept();
}

void CDlgGetUserPassword::on_pbCancel_clicked()
{
    reject();
}
