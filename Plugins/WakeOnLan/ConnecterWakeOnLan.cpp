// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QHBoxLayout>
#include <QRegularExpression>
#include "ConnecterWakeOnLan.h"
#include "PluginClient.h"

static Q_LOGGING_CATEGORY(log, "WakeOnLan.Connecter")
CConnecterWakeOnLan::CConnecterWakeOnLan(CPluginClient *plugin)
    : CConnecterConnect(plugin)
    , m_pView(nullptr)
    , m_pConnect(nullptr)
{
    qDebug(log) << __FUNCTION__;
    m_Parameter.m_WakeOnLan.SetEnable(true);
}

CConnecterWakeOnLan::~CConnecterWakeOnLan()
{
    qDebug(log) << __FUNCTION__;
}

qint16 CConnecterWakeOnLan::Version()
{
    return 0;
}

int CConnecterWakeOnLan::OnInitial()
{
    qDebug(log) << __FUNCTION__;
    SetParameter(&m_Parameter);
    CPluginClient* plugin = GetPlugClient();
    m_pView = new CFrmWakeOnLan();
    if(m_pView)
        m_pView->setWindowTitle(plugin->Name());
    m_Menu.addAction(m_pSettings);
    return 0;
}

int CConnecterWakeOnLan::OnClean()
{
    qDebug(log) << __FUNCTION__;
    if(m_pView)
        delete m_pView;
    return 0;
}

QWidget *CConnecterWakeOnLan::GetViewer()
{
    return m_pView;
}

QDialog *CConnecterWakeOnLan::OnOpenDialogSettings(QWidget *parent)
{
    QDialog* pDlg = new QDialog(parent);
    QHBoxLayout* pLayout = new QHBoxLayout(pDlg);
    pDlg->setLayout(pLayout);

    CFrmWakeOnLan *pView = new CFrmWakeOnLan();
    if(pView) {
        pView->SetParameter(&m_Parameter.m_WakeOnLan);
        pLayout->addWidget(pView);
        bool check = connect(pView, SIGNAL(sigOk()), pDlg, SLOT(accept()));
        Q_ASSERT(check);
        check = connect(pView, SIGNAL(sigCancel()), pDlg, SLOT(reject()));
        Q_ASSERT(check);
    }

    return pDlg;
}

const QString CConnecterWakeOnLan::Id()
{
    QString szId = Protocol() + "_" + GetPlugClient()->Name();
    if(GetParameter())
    {
        if(!GetParameter()->GetName().isEmpty())
            szId += "_" + GetParameter()->GetName();
        if(!GetParameter()->m_WakeOnLan.GetMac().isEmpty())
            szId += "_" + GetParameter()->m_WakeOnLan.GetMac();
    }
    QRegularExpression exp("[-@:/#%!^&* \\.]");
    szId = szId.replace(exp, "_");
    return szId;
}

const QString CConnecterWakeOnLan::Name()
{
    QString szName;
    if(GetParameter() && GetParameter()->GetParameterClient()
        && GetParameter()->GetParameterClient()->GetShowProtocolPrefix())
        szName = Protocol() + ":";
    if(GetParameter()->m_WakeOnLan.GetMac().isEmpty())
        return szName + GetPlugClient()->Name();
    return szName + GetParameter()->m_WakeOnLan.GetMac();
}

int CConnecterWakeOnLan::Connect()
{
    m_pConnect = new CConnectWakeOnLan(this);
    bool check = false;
    if(m_pView) {
        m_pView->SetParameter(&m_Parameter.m_WakeOnLan);
        if(m_pConnect) {
            check = connect(m_pView, SIGNAL(sigOk()), m_pConnect, SLOT(Connect()));
            Q_ASSERT(check);
        }
        check = connect(m_pView, SIGNAL(sigCancel()), this, SIGNAL(sigDisconnect()));
        Q_ASSERT(check);
    }
    emit sigConnected();
    return 0;
}

int CConnecterWakeOnLan::DisConnect()
{
    if(m_pConnect) {
        m_pConnect->Disconnect();
        m_pConnect->deleteLater();
        emit sigDisconnected();
    }
    return 0;
}

CConnect *CConnecterWakeOnLan::InstanceConnect()
{
    return nullptr;
}
