// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRegularExpression>
#include <QPushButton>
#include <QtGlobal>
#include <QCoreApplication>
#include "RabbitCommonTools.h"
#include "PluginClient.h"

#include "ParameterWakeOnLanUI.h"
#include "ConnecterWakeOnLan.h"

static Q_LOGGING_CATEGORY(log, "WakeOnLan.Connecter")
CConnecterWakeOnLan::CConnecterWakeOnLan(CPluginClient *plugin)
    : CConnecterConnect(plugin)
    , m_pView(nullptr)
    , m_pModel(nullptr)
    , m_pParameterClient(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
}

CConnecterWakeOnLan::~CConnecterWakeOnLan()
{
    qDebug(log) << Q_FUNC_INFO;
}

qint16 CConnecterWakeOnLan::Version()
{
    return 0;
}

int CConnecterWakeOnLan::Initial()
{
    qDebug(log) << Q_FUNC_INFO;
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
    m_Menu.addAction(QIcon::fromTheme("document-edit"), tr("Edit"),
                     this, [&](){
                         QSharedPointer<CParameterWakeOnLan> para
                             = m_pModel->GetData(m_pView->GetCurrentIndex());
                         if(!para) {
                             QMessageBox::information(
                                 nullptr,
                                 tr("Information"),
                                 tr("Please select a item"));
                             return;
                         }
                         CParameterWakeOnLanUI dlg;
                         dlg.SetParameter(para.data());
                         RC_SHOW_WINDOW(&dlg);
                     });
    m_Menu.addAction(QIcon::fromTheme("list-remove"), tr("Remove"),
                     m_pView, SLOT(slotRemoveRow()));
#if defined(Q_OS_UNIX)
    if(RabbitCommon::CTools::HasAdministratorPrivilege())
    {
        m_Menu.addAction(QIcon::fromTheme("view-refresh"), tr("Refresh"),
                         this, [&](){
                             foreach(auto p, m_pModel->m_Data)
                             m_Arp.GetMac(p);
                         });
        m_Menu.addAction(
            QIcon::fromTheme("mac"), tr("Get mac address"),
            this, [&](){
                if(!m_pModel || !m_pView)
                    return;
                foreach(auto index, m_pView->GetSelect()) {
                    QSharedPointer<CParameterWakeOnLan> p
                        = m_pModel->GetData(index);
                    if(!p) continue;
                    if(m_Arp.GetMac(p))
                        p->SetHostState(CParameterWakeOnLan::HostState::GetMac);
                }
            });
    }
#else
    m_Menu.addAction(QIcon::fromTheme("view-refresh"), tr("Refresh"),
                     this, [&](){
                         foreach(auto p, m_pModel->m_Data)
                             m_Arp.GetMac(p);
                     });
    m_Menu.addAction(
        QIcon::fromTheme("mac"), tr("Get mac address"),
        this, [&](){
            if(!m_pModel || !m_pView)
                return;
            foreach(auto index, m_pView->GetSelect()) {
                QSharedPointer<CParameterWakeOnLan> p = m_pModel->GetData(index);
                if(!p) continue;
                if(m_Arp.GetMac(p))
                    p->SetHostState(CParameterWakeOnLan::HostState::GetMac);
            }
        });
#endif
    m_Menu.addAction(
        QIcon::fromTheme("lan"), tr("Wake on lan"),
        this, [&](){
            if(!m_pModel || !m_pView)
                return;
            foreach(auto index, m_pView->GetSelect()) {
                QSharedPointer<CParameterWakeOnLan> p = m_pModel->GetData(index);
                if(!p) continue;
                if(!m_Arp.WakeOnLan(p))
                    p->SetHostState(CParameterWakeOnLan::HostState::WakeOnLan);
            }
        });

    return 0;
}

int CConnecterWakeOnLan::Clean()
{
    qDebug(log) << Q_FUNC_INFO;
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
#if defined(Q_OS_UNIX)
    if(!RabbitCommon::CTools::HasAdministratorPrivilege())
    {
        static bool bShow = false;
        if(!bShow) {
            bShow = true;
            int nRet = QMessageBox::warning(
                nullptr, tr("Warning"),
                tr("There are no administrator privileges, "
                   "and some functions(Get mac address) are restricted. "
                   "Please restart the program with administrative privileges."),
                QMessageBox::Yes | QMessageBox::No);
            if(QMessageBox::Yes == nRet) {
                bool bRet = RabbitCommon::CTools::executeByRoot(
                    QCoreApplication::applicationFilePath());
                if(bRet)
                    QCoreApplication::quit();
            }
        }
    }
#endif
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
    return m_pModel->Load(set, m_pParameterClient);
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
