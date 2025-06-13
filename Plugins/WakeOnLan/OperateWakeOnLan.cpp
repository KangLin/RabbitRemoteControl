// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRegularExpression>
#include <QPushButton>
#include <QCheckBox>
#include <QtGlobal>
#include <QCoreApplication>
#include "RabbitCommonTools.h"
#include "Plugin.h"

#include "ParameterWakeOnLanUI.h"
#include "OperateWakeOnLan.h"

static Q_LOGGING_CATEGORY(log, "WakeOnLan")
COperateWakeOnLan::COperateWakeOnLan(CPlugin *plugin)
    : COperate(plugin)
    , m_pView(nullptr)
    , m_pModel(nullptr)
    , m_pParameterClient(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
}

COperateWakeOnLan::~COperateWakeOnLan()
{
    qDebug(log) << Q_FUNC_INFO;
}

const qint16 COperateWakeOnLan::Version() const
{
    return 0;
}

int COperateWakeOnLan::Initial()
{
    int nRet = COperate::Initial();
    if(nRet) return nRet;
    qDebug(log) << Q_FUNC_INFO;
    bool check = false;
    CPlugin* plugin = GetPlugin();
    m_pModel = new CWakeOnLanModel(this);
    if(!m_pModel)
        return -1;
    m_pView = new CFrmWakeOnLan(m_pModel);
    if(!m_pView) return -2;
    m_pView->setWindowTitle(plugin->Name());
    check = connect(m_pView, SIGNAL(sigViewerFocusIn(QWidget*)),
                    this, SIGNAL(sigViewerFocusIn(QWidget*)));
    Q_ASSERT(check);
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

int COperateWakeOnLan::Clean()
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_pView)
        delete m_pView;
    if(m_pModel)
        delete m_pModel;
    return 0;
}

QWidget *COperateWakeOnLan::GetViewer()
{
    return m_pView;
}

QDialog *COperateWakeOnLan::OnOpenDialogSettings(QWidget *parent)
{
    return nullptr;
}

const QString COperateWakeOnLan::Id()
{
    QString szId = Protocol() + "_" + GetPlugin()->Name();
    return szId;
}

const QString COperateWakeOnLan::Name()
{
    QString szName;
    if(m_pParameterClient && m_pParameterClient->GetShowProtocolPrefix())
        szName = Protocol() + ":";
    szName += GetPlugin()->DisplayName();
    return szName;
}

int COperateWakeOnLan::Start()
{
#if defined(Q_OS_UNIX)
    if(!RabbitCommon::CTools::HasAdministratorPrivilege()
        && m_pParameterClient->GetPromptAdministratorPrivilege())
    {
        static bool bShow = false;
        if(!bShow) {
            bShow = true;
            int nRet = 0;
            QMessageBox msg(
                QMessageBox::Warning, tr("Warning"),
                tr("There are no administrator privileges, "
                   "and some functions(Get mac address) are restricted. "
                   "Please restart the program with administrative privileges."),
                QMessageBox::Yes | QMessageBox::No);
            msg.setCheckBox(new QCheckBox(tr("Always shown"), &msg));
            msg.checkBox()->setCheckable(true);
            nRet = msg.exec();
            msg.checkBox()->setChecked(
                m_pParameterClient->GetPromptAdministratorPrivilege());
            if(QMessageBox::Yes == nRet) {
                RabbitCommon::CTools::Instance()->StartWithAdministratorPrivilege(true);
            }
            if(m_pParameterClient->GetPromptAdministratorPrivilege()
                != msg.checkBox()->isChecked()) {
                m_pParameterClient->SetPromptAdministratorPrivilege(
                    msg.checkBox()->isChecked());
                // TODO: save settings
                //SaveSettings();
            }
        }
    }
#endif
    emit sigRunning();
    return 0;
}

int COperateWakeOnLan::Stop()
{
    emit sigFinished();
    return 0;
}

int COperateWakeOnLan::SetParameterPlugin(CParameterPlugin *pPara)
{
    m_pParameterClient = &pPara->m_Client;
    return 0;
}

int COperateWakeOnLan::Load(QSettings &set)
{
    if(!m_pModel) return -1;
    return m_pModel->Load(set, m_pParameterClient);
}

int COperateWakeOnLan::Save(QSettings &set)
{
    if(!m_pModel) return -1;
    return m_pModel->Save(set);
}

void COperateWakeOnLan::slotAdd()
{
    QSharedPointer<CParameterWakeOnLan> para(new CParameterWakeOnLan());
    para->SetParameterClient(m_pParameterClient);
    CParameterWakeOnLanUI dlg;
    dlg.SetParameter(para.data());
    int nRet = RC_SHOW_WINDOW(&dlg);
    if(QDialog::Accepted == nRet)
        m_pModel->AddItem(para);
}
