#include "DlgUserPassword.h"
#include "ui_DlgUserPassword.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Client.DlgUserPassword")

static int g_CDlgUserPassword = qRegisterMetaType<CDlgUserPassword>();

CDlgUserPassword::CDlgUserPassword(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgUserPassword)
{
    ui->setupUi(this);
    setWindowTitle(tr("Set user and password"));
}

CDlgUserPassword::~CDlgUserPassword()
{
    delete ui;
}

CDlgUserPassword::CDlgUserPassword(const CDlgUserPassword &other)
{
    m_pNet = other.m_pNet;
}

void CDlgUserPassword::SetContext(void *pContext)
{
    m_pNet = (CParameterNet*)pContext;
    if(!m_pNet) {
        qCritical(log) << "The pContext is null";
        return;
    }
    auto &user = m_pNet->m_User;
    ui->lbText->setText(windowTitle() + "\n" + tr("Server: ")
                        + m_pNet->GetHost()
                        + ":" + QString::number(m_pNet->GetPort()));
    ui->wUser->SetParameter(&user);
}

void CDlgUserPassword::SetConnecter(CConnecter *pConnecter)
{
    m_pConnecter = pConnecter;
}

void CDlgUserPassword::accept()
{
    if(!m_pNet) {
        qCritical(log) << "The pContext is null";
        return;
    }

    ui->wUser->Accept();
    emit m_pNet->sigChanged();
    QDialog::accept();
}
