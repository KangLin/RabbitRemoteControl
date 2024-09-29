#include "DlgLibVNCServerPassword.h"
#include "ui_DlgLibVNCServerPassword.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "LibVNCServer.Dialog.GetPassword")

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
    if(!m_pConnecter) {
        qCritical(log) << "The connecter is nullptr";
        return;
    }

    m_pParameter = qobject_cast<CParameterLibVNCServer*>(m_pConnecter->GetParameter());
    if(!m_pParameter) {
        qCritical(log) << "The m_pParameter is nullptr";
        return;
    }

    ui->lbText->setText(tr("Set password for %1").arg(m_pConnecter->Name()));
    ui->wUser->SetParameter(&m_pParameter->m_Net.m_User);
}

void CDlgLibVNCServerPassword::on_pbOK_clicked()
{
    if(!m_pParameter) {
        qCritical(log) << "The m_pParameter is nullptr";
        return;
    }
    ui->wUser->Accept();
    emit m_pParameter->sigChanged();
    accept();
}

void CDlgLibVNCServerPassword::on_pbCancel_clicked()
{
    reject();
}
