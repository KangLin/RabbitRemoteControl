#include "DlgGetUserPasswordVNC.h"
#include "ui_DlgGetUserPasswordVNC.h"

static int g_CDlgGetPasswordVNC = qRegisterMetaType<CDlgGetPasswordVNC>();

CDlgGetPasswordVNC::CDlgGetPasswordVNC(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgGetPasswordVNC),
    m_pConnecter(nullptr),
    m_pParameter(nullptr)
{
    ui->setupUi(this);
}

CDlgGetPasswordVNC::~CDlgGetPasswordVNC()
{
    delete ui;
}

CDlgGetPasswordVNC::CDlgGetPasswordVNC(const CDlgGetPasswordVNC& dlg)
{
    m_pConnecter = dlg.m_pConnecter;
    m_pParameter = dlg.m_pParameter;
}

void CDlgGetPasswordVNC::SetContext(void *pContext)
{
}

void CDlgGetPasswordVNC::SetConnecter(CConnecter *pConnecter)
{
    m_pConnecter = qobject_cast<CConnecterVnc*>(pConnecter);
    if(!m_pConnecter) return;

    m_pParameter = qobject_cast<CParameterVnc*>(m_pConnecter->GetParameter());

    ui->lbText->setText(tr("Set password for %1").arg(m_pConnecter->Name()));
    ui->wUser->SetParameter(&m_pParameter->m_Net.m_User);
}

void CDlgGetPasswordVNC::showEvent(QShowEvent *event)
{
}

void CDlgGetPasswordVNC::on_pbOK_clicked()
{
    ui->wUser->slotAccept();
    emit m_pConnecter->sigUpdateParameters(m_pConnecter);
    accept();
}

void CDlgGetPasswordVNC::on_pbCancel_clicked()
{
    reject();
}
