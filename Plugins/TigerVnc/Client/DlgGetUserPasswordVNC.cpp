#include "DlgGetUserPasswordVNC.h"
#include "ui_DlgGetUserPasswordVNC.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "VNC.Dialog.GetPassword")

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
    Q_ASSERT(pConnecter);
    qDebug(log) << "class name:" << pConnecter->metaObject()->className();
    m_pConnecter = qobject_cast<CConnecterVnc*>(pConnecter);
    Q_ASSERT(m_pConnecter);
    if(!m_pConnecter) {
        qCritical(log) << "The connecter is nullptr";
        return;
    }

    m_pParameter = qobject_cast<CParameterVnc*>(m_pConnecter->GetParameter());
    if(!m_pParameter) {
        qCritical(log) << "The m_pParameter is nullptr";
        return;
    }

    ui->lbText->setText(tr("Set password for %1").arg(m_pConnecter->Name()));
    ui->wUser->SetParameter(&m_pParameter->m_Net.m_User);
}

void CDlgGetPasswordVNC::on_pbOK_clicked()
{
    if(!m_pParameter) {
        qCritical(log) << "The m_pParameter is nullptr";
        return;
    }
    ui->wUser->slotAccept();
    emit m_pParameter->sigChanged();
    accept();
}

void CDlgGetPasswordVNC::on_pbCancel_clicked()
{
    reject();
}
