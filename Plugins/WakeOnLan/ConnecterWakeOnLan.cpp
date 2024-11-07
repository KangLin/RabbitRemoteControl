// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRegularExpression>
#include <QPushButton>
#include <QtGlobal>

#include "RabbitCommonTools.h"
#include "PluginClient.h"
#include "ConnectWakeOnLan.h"
#include "ConnecterWakeOnLan.h"
#include "ParameterWakeOnLanUI.h"

static Q_LOGGING_CATEGORY(log, "WakeOnLan.Connecter")
CConnecterWakeOnLan::CConnecterWakeOnLan(CPluginClient *plugin)
    : CConnecterConnect(plugin)
    , m_pView(nullptr)
    , m_pModel(nullptr)
    , m_pParameterClient(nullptr)
    , m_pConnect(nullptr)
{
    qDebug(log) << __FUNCTION__;
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
    bool check = false;
    CPluginClient* plugin = GetPlugClient();
    m_pModel = new CWakeOnLanModel(this);
    if(!m_pModel)
        return -1;
    m_pView = new CFrmWakeOnLan(m_pModel);
    if(!m_pView) return -2;
    m_pView->setWindowTitle(plugin->Name());
    check = connect(m_pView, &CFrmWakeOnLan::customContextMenuRequested,
                    this, [&](const QPoint &pos){
                        m_Menu.exec(m_pView->mapToGlobal(pos));
                    });
    Q_ASSERT(check);
    m_Menu.addAction(QIcon::fromTheme("list-add"), tr("Add"),
                     this, SLOT(slotAdd()));
    m_Menu.addAction(QIcon::fromTheme("list-remove"), tr("Remove"),
                     m_pView, SLOT(slotRemoveRow()));
    m_Menu.addAction(
        QIcon::fromTheme("mac"), tr("Get mac address"),
        this, [&](){
            if(!m_pModel || !m_pView)
                return;
            CParameterWakeOnLan* p = m_pModel->GetData(m_pView->GetCurrentIndex());
            p->SetHostState(CParameterWakeOnLan::HostState::GetMac);

            CConnectWakeOnLan c(this);
            QString szMac = c.GetMac(p->m_Net.GetHost(),
                                     p->GetNetworkInterface(),
                                     p->GetDelay());
            if(szMac.isEmpty()) {
                p->SetHostState(CParameterWakeOnLan::HostState::Offline);
            } else {
                p->SetMac(szMac);
                p->SetHostState(CParameterWakeOnLan::HostState::Online);
            }
            emit sigGetMac(p);
        });
    m_Menu.addAction(
        QIcon::fromTheme("lan"), tr("Wake on lan"),
        this, [&](){
            if(!m_pModel || !m_pView)
                return;
            CParameterWakeOnLan* p = m_pModel->GetData(m_pView->GetCurrentIndex());
            p->SetHostState(CParameterWakeOnLan::HostState::WakeOnLan);
            emit sigWakeOnLan(p);
        });
    return 0;
}

int CConnecterWakeOnLan::OnClean()
{
    qDebug(log) << __FUNCTION__;
    if(m_pView)
        delete m_pView;
    if(m_pModel)
        delete m_pModel;
    return 0;
}

QWidget *CConnecterWakeOnLan::GetViewer()
{
    return m_pView;
}

QDialog *CConnecterWakeOnLan::OnOpenDialogSettings(QWidget *parent)
{
    return nullptr;
}

const QString CConnecterWakeOnLan::Id()
{
    QString szId = Protocol() + "_" + GetPlugClient()->Name();
    return szId;
}

const QString CConnecterWakeOnLan::Name()
{
    QString szName;
    if(GetParameter() && GetParameter()->GetParameterClient()
        && GetParameter()->GetParameterClient()->GetShowProtocolPrefix())
        szName = Protocol() + ":";
    szName += GetPlugClient()->Name();
    return szName;
}

int CConnecterWakeOnLan::Connect()
{
    emit sigConnected();
    return 0;
}

int CConnecterWakeOnLan::DisConnect()
{
    emit sigDisconnected();
    return 0;
}

CConnect *CConnecterWakeOnLan::InstanceConnect()
{
    return nullptr;
}

int CConnecterWakeOnLan::SetParameterClient(CParameterClient *pPara)
{
    m_pParameterClient = pPara;
    return 0;
}

int CConnecterWakeOnLan::Load(QSettings &set)
{
    if(!m_pModel) return -1;
    return m_pModel->Load(set);
}

int CConnecterWakeOnLan::Save(QSettings &set)
{
    if(!m_pModel) return -1;
    return m_pModel->Save(set);
}

void CConnecterWakeOnLan::slotAdd()
{
    QSharedPointer<CParameterWakeOnLan> para(new CParameterWakeOnLan());
    para->SetParameterClient(m_pParameterClient);
    CParameterWakeOnLanUI dlg;
    dlg.SetParameter(para.data());
    int nRet = RC_SHOW_WINDOW(&dlg);
    if(QDialog::Accepted == nRet)
        m_pModel->AddItem(para);
}
