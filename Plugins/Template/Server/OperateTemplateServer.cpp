// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>

#include "FrmViewServer.h"
#include "BackendTemplateServer.h"
#include "ParameterTemplateServer.h"
#include "DlgSettingsTemplateServer.h"
#include "OperateTemplateServer.h"

static Q_LOGGING_CATEGORY(log, "Operate.TemplateServer")
COperateTemplateServer::COperateTemplateServer(CPlugin *plugin)
    : COperateServer(plugin)
    , m_pPara(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
}

COperateTemplateServer::~COperateTemplateServer()
{
    qDebug(log) << Q_FUNC_INFO;
}

CParameterTemplateServer* COperateTemplateServer::GetParameter() const
{
    return m_pPara;
}

const qint16 COperateTemplateServer::Version() const
{
    // TODO: Add version
    return 0;
}

CBackend* COperateTemplateServer::InstanceBackend()
{
    CFrmViewServer* pView = qobject_cast<CFrmViewServer*>(GetViewer());
    auto *p = new CBackendTemplateServer(this);
    if(!pView || !p)
        return nullptr;
    bool check = connect(p, SIGNAL(sigConnectCount(int,int,int)),
                         pView, SLOT(slotConnectCount(int,int,int)));
    Q_ASSERT(check);

    check = connect(p, SIGNAL(sigConnected(QString,quint16)),
                    pView, SLOT(slotConnected(QString,quint16)));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigDisconnected(QString,quint16)),
                    pView, SLOT(slotDisconnected(QString,quint16)));
    Q_ASSERT(check);
    check = connect(pView, SIGNAL(sigDisconnect(QString,quint16)),
                    p, SLOT(slotDisconnect(QString,quint16)));
    Q_ASSERT(check);
    return p;
}

int COperateTemplateServer::SetPluginParameters(CParameterPlugin *pPara)
{
    int nRet = 0;
    // TODO: Modify apply plugin parameters
    if(m_pPara) {
        m_pPara->SetPluginParameters(pPara);
        m_pPara->m_Net.m_User.SetSavePassword(true);
        m_pPara->m_Net.m_User.SetSavePassphrase(true);
    }
    return nRet;
}

QDialog *COperateTemplateServer::OnOpenDialogSettings(QWidget *parent)
{
    return new CDlgSettingsTemplateServer(m_pPara, parent);
}

int COperateTemplateServer::Load(QSettings &set)
{
    int nRet = 0;

    if(m_pPara)
        nRet = m_pPara->Load(set);

    return nRet;
}

int COperateTemplateServer::Save(QSettings &set)
{
    int nRet = 0;
    if(m_pPara)
        nRet = m_pPara->Save(set);
    return nRet;
}

int COperateTemplateServer::Initial()
{
    int nRet = 0;
    nRet = COperateServer::Initial();
    if(nRet) return nRet;
    m_pPara = new CParameterTemplateServer();

    // TODO: add initial

    return nRet;
}

int COperateTemplateServer::Clean()
{
    int nRet = 0;
    // TODO: add Clean

    if(m_pPara) {
        delete m_pPara;
        m_pPara = nullptr;
    }
    nRet = COperateServer::Clean();
    return nRet;
}
