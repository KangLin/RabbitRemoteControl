// Author: Kang Lin <kl222@126.com>

#include "DlgUserPassword.h"
#include "ui_DlgUserPassword.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Client.DlgUserPassword")

static int g_CDlgUserPassword = qRegisterMetaType<CDlgUserPassword>();

CDlgUserPassword::CDlgUserPassword(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgUserPassword)
    , m_pUser(nullptr)
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
    m_pUser = other.m_pUser;
}

void CDlgUserPassword::SetContext(void *pContext)
{
    auto pNet = (CParameterNet*)pContext;
    if(!pNet) {
        qCritical(log) << "The pContext is null";
        return;
    }
    m_pUser = &pNet->m_User;
    ui->lbText->setText(windowTitle() + "\n" + tr("Server: ")
                        + pNet->GetHost()
                        + ":" + QString::number(pNet->GetPort()));
    ui->wUser->SetParameter(m_pUser);
}

int CDlgUserPassword::SetUser(const QString &szPrompt, CParameterUser* pUser)
{
    m_pUser = pUser;
    ui->lbText->setText(szPrompt);
    ui->wUser->SetParameter(m_pUser);
    return 0;
}

void CDlgUserPassword::accept()
{
    if(!m_pUser) {
        qCritical(log) << "The pContext is null";
        return;
    }
    ui->wUser->Accept();
    emit m_pUser->sigChanged();
    QDialog::accept();
}
