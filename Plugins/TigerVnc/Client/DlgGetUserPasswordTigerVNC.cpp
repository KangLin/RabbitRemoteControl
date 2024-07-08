#include "DlgGetUserPasswordTigerVNC.h"
#include "ui_DlgGetUserPasswordTigerVNC.h"

static int g_CDlgGetPasswordTigerVNC = qRegisterMetaType<CDlgGetPasswordTigerVNC>();

CDlgGetPasswordTigerVNC::CDlgGetPasswordTigerVNC(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgGetPasswordTigerVNC),
    m_pConnecter(nullptr),
    m_pParameter(nullptr)
{
    ui->setupUi(this);
}

CDlgGetPasswordTigerVNC::~CDlgGetPasswordTigerVNC()
{
    delete ui;
}

CDlgGetPasswordTigerVNC::CDlgGetPasswordTigerVNC(const CDlgGetPasswordTigerVNC& dlg)
{
    m_pConnecter = dlg.m_pConnecter;
    m_pParameter = dlg.m_pParameter;
}

void CDlgGetPasswordTigerVNC::SetContext(void *pContext)
{
}

void CDlgGetPasswordTigerVNC::SetConnecter(CConnecter *pConnecter)
{
    m_pConnecter = qobject_cast<CConnecterTigerVnc*>(pConnecter);
    if(!m_pConnecter) return;

    m_pParameter = qobject_cast<CParameterTigerVnc*>(m_pConnecter->GetParameter());

    ui->lbText->setText(tr("Set password for %1").arg(m_pConnecter->Name()));
    ui->wUser->SetParameter(&m_pParameter->m_Net.m_User);
}

void CDlgGetPasswordTigerVNC::showEvent(QShowEvent *event)
{
}

void CDlgGetPasswordTigerVNC::on_pbOK_clicked()
{
    ui->wUser->slotAccept();
    emit m_pConnecter->sigUpdateParameters(m_pConnecter);
    accept();
}

void CDlgGetPasswordTigerVNC::on_pbCancel_clicked()
{
    reject();
}
