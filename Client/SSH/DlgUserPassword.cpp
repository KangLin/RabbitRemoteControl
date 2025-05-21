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
    m_pPara = (CParameterSSHTunnel*)pContext;
    if(!m_pPara) {
        qCritical(log) << "The pContext is null";
        return;
    }
    auto &net = m_pPara->m_Net;
    auto &user = net.m_User;
    if(user.GetUsedType() == CParameterUser::TYPE::UserPassword) {
        setWindowTitle(tr("Set SSH user and password"));
    }

    if(user.GetUsedType() == CParameterUser::TYPE::PublicKey) {
        setWindowTitle(tr("Set SSH passphrase"));
    }

    ui->lbText->setText(windowTitle() + "\n" + tr("SSH server: ")
                        + net.GetHost()
                        + ":" + QString::number(net.GetPort()));

    ui->wUser->SetParameter(&user);
}

void CDlgUserPassword::SetConnecter(CConnecter *pConnecter)
{
    m_pConnecter = pConnecter;
}

void CDlgUserPassword::accept()
{
    if(!m_pPara) {
        qCritical(log) << "The pContext is null";
        return;
    }

    ui->wUser->Accept();
    emit m_pPara->sigChanged();
    QDialog::accept();
}
