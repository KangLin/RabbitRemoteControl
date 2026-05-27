// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>

#include "BackendThread.h"
#include "BackendSftpServer.h"
#include "ParameterSftpServer.h"
#include "FrmViewerSftpServer.h"
#include "DlgSettingsSftpServer.h"
#include "OperateSftpServer.h"

static Q_LOGGING_CATEGORY(log, "Operate.SftpServer")
COperateSftpServer::COperateSftpServer(CPlugin *plugin)
    : COperate(plugin)
    , m_pPara(nullptr)
    , m_pViewer(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
}

COperateSftpServer::~COperateSftpServer()
{
    qDebug(log) << Q_FUNC_INFO;
}

CParameterSftpServer* COperateSftpServer::GetParameter() const
{
    return m_pPara;
}

const qint16 COperateSftpServer::Version() const
{
    return 0;
}

QWidget *COperateSftpServer::GetViewer()
{
    return m_pViewer;
}

int COperateSftpServer::Start()
{
    int nRet = 0;
    m_pThread = new CBackendThread(this);
    if(!m_pThread) {
        qCritical(log) << "new CBackendThread fail";
        return -1;
    }
    m_pThread->start();
    return nRet;
}

int COperateSftpServer::Stop()
{
    int nRet = 0;
    if(m_pThread)
    {
        m_pThread->quit();
        //Don't delete m_pThread, See CBackendThread
        m_pThread = nullptr;
    }
    return nRet;
}

CBackend* COperateSftpServer::InstanceBackend()
{
    return new CBackendSftpServer(this);
}

int COperateSftpServer::SetPluginParameters(CParameterPlugin *pPara)
{
    int nRet = 0;
    if(m_pPara) {
        m_pPara->SetPluginParameters(pPara);
        m_pPara->m_Net.m_User.SetSavePassword(true);
        m_pPara->m_Net.m_User.SetSavePassphrase(true);
    }
    return nRet;
}

QDialog *COperateSftpServer::OnOpenDialogSettings(QWidget *parent)
{
    return new CDlgSettingsSftpServer(m_pPara, parent);
}

int COperateSftpServer::Load(QSettings &set)
{
    int nRet = 0;
    if(m_pPara)
        nRet = m_pPara->Load(set);
    return nRet;
}

int COperateSftpServer::Save(QSettings &set)
{
    int nRet = 0;
    if(m_pPara)
        nRet = m_pPara->Save(set);
    return nRet;
}

int COperateSftpServer::Initial()
{
    int nRet = 0;
    nRet = COperate::Initial();
    if(nRet) return nRet;

    m_pPara = new CParameterSftpServer();
    m_pViewer = new CFrmViewerSftpServer();

    m_Menu.addAction(m_pActionSettings);
    return nRet;
}

int COperateSftpServer::Clean()
{
    int nRet = 0;
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
