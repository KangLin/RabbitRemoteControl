#include "DlgUserPassword.h"
#include "ui_DlgUserPassword.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Channel.SSH.Tunnel.DlgUserPassword")

static int g_CDlgUserPassword = qRegisterMetaType<CDlgUserPassword>();

CDlgUserPassword::CDlgUserPassword(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgUserPassword)
{
    ui->setupUi(this);
}

CDlgUserPassword::~CDlgUserPassword()
{
    delete ui;
}

CDlgUserPassword::CDlgUserPassword(const CDlgUserPassword &other)
{
    m_pPara = other.m_pPara;
}

void CDlgUserPassword::SetContext(void *pContext)
{
    m_pPara = (CParameterChannelSSH*)pContext;
    if(!m_pPara) {
        qCritical(log) << "The pContext is null";
        return;
    }

    if(m_pPara->GetAuthenticationMethod() == SSH_AUTH_METHOD_PASSWORD) {
        setWindowTitle(tr("Set SSH user and password"));
        ui->leUser->setText(m_pPara->GetUser());
    }

    if(m_pPara->GetAuthenticationMethod() == SSH_AUTH_METHOD_PUBLICKEY) {
        setWindowTitle(tr("Set SSH passphrase"));
        ui->leUser->setEnabled(false);
    }

    ui->lbText->setText(tr("SSH server: ")
                        + m_pPara->GetServer()
                        + ":" + QString::number(m_pPara->GetPort()));

    ui->lePassowrd->setText(m_pPara->GetPassword());
}

void CDlgUserPassword::accept()
{
    if(!m_pPara) {
        qCritical(log) << "The pContext is null";
        return;
    }

    m_pPara->SetUser(ui->leUser->text());
    if(m_pPara->GetAuthenticationMethod() == SSH_AUTH_METHOD_PASSWORD)
        m_pPara->SetPassword(ui->lePassowrd->text());
    if(m_pPara->GetAuthenticationMethod() == SSH_AUTH_METHOD_PUBLICKEY)
        m_pPara->SetPassphrase(ui->lePassowrd->text());
    
    QDialog::accept();
}
