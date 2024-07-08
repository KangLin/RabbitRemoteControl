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
    Q_UNUSED(event)
    ui->lbText->setText(tr("Set password for %1").arg(m_pConnecter->Name()));
    ui->wUser->SetParameter(&m_pParameter->m_Net.m_User);
}

void CDlgLibVNCServerPassword::on_pbOK_clicked()
{
    ui->wUser->slotAccept();
    emit m_pConnecter->sigUpdateParameters(m_pConnecter);
    accept();
}

void CDlgLibVNCServerPassword::on_pbCancel_clicked()
{
    reject();
}
