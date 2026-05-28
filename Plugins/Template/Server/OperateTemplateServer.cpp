// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>

#include "BackendThread.h"
#include "BackendTemplateServer.h"
#include "ParameterTemplateServer.h"
#include "FrmViewerTemplateServer.h"
#include "DlgSettingsTemplateServer.h"
#include "OperateTemplateServer.h"

static Q_LOGGING_CATEGORY(log, "Operate.TemplateServer")
COperateTemplateServer::COperateTemplateServer(CPlugin *plugin)
    : COperate(plugin)
    , m_pPara(nullptr)
    , m_pViewer(nullptr)
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

QWidget *COperateTemplateServer::GetViewer()
{
    return m_pViewer;
}

int COperateTemplateServer::Start()
{
    int nRet = 0;
    // TODO: If backend threads are not needed, modifications are required.
    m_pThread = new CBackendThread(this);
    if(!m_pThread) {
        qCritical(log) << "new CBackendThread fail";
        return -1;
    }

    m_pThread->start();
    return nRet;
}

int COperateTemplateServer::Stop()
{
    int nRet = 0;
    // TODO: If backend threads are not needed, modifications are required.
    if(m_pThread)
    {
        m_pThread->quit();
        //Don't delete m_pThread, See CBackendThread
        m_pThread = nullptr;
    }
    return nRet;
}

CBackend* COperateTemplateServer::InstanceBackend()
{
    return new CBackendTemplateServer(this);
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
    nRet = COperate::Initial();
    m_pPara = new CParameterTemplateServer();
    m_pViewer = new CFrmViewerTemplateServer();

    // TODO: add initial

    m_Menu.addAction(m_pActionSettings);
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
    if(m_pViewer) {
        delete m_pViewer;
        m_pViewer = nullptr;
    }
    nRet = COperate::Clean();
    return nRet;
}
