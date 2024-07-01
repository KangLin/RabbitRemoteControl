#include "DlgGetUserPasswordFreeRDP.h"
#include "ui_DlgGetUserPasswordFreeRDP.h"

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
}

void CDlgGetUserPasswordFreeRDP::showEvent(QShowEvent *event)
{
    Q_ASSERT(m_pParameter);
    if(!m_pParameter) return;
    
    QString szDomain = m_pParameter->GetDomain();
    
    ui->lbText->setText(tr("Set password for %1").arg(m_pConnecter->Name()));
    ui->leDomain->setText(szDomain);
    ui->leUser->setText(m_pParameter->m_Net.m_User.GetUser());
    ui->lePassword->setText(m_pParameter->m_Net.m_User.GetPassword());
    ui->cbSavePassword->setChecked(m_pParameter->m_Net.m_User.GetSavePassword());
    
}

void CDlgGetUserPasswordFreeRDP::on_pbOK_clicked()
{
    Q_ASSERT(m_pParameter);
    m_pParameter->m_Net.m_User.SetUser(ui->leUser->text());
    m_pParameter->m_Net.m_User.SetPassword(ui->lePassword->text());
    m_pParameter->m_Net.m_User.SetSavePassword(ui->cbSavePassword->isChecked());
    m_pParameter->SetDomain(ui->leDomain->text().toStdString().c_str());
    emit m_pConnecter->sigUpdateParameters(m_pConnecter);
    accept();
}

void CDlgGetUserPasswordFreeRDP::on_pbCancel_clicked()
{
    reject();
}
