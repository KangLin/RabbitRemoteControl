// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>

#include "BackendThread.h"
#include "BackendTemplateBase.h"
#include "ParameterTemplateBase.h"
#include "FrmViewerTemplateBase.h"
#include "DlgSettingsTemplateBase.h"
#include "OperateTemplateBase.h"

static Q_LOGGING_CATEGORY(log, "Operate.TemplateBase")
COperateTemplateBase::COperateTemplateBase(CPlugin *plugin)
    : COperate(plugin)
    , m_pPara(nullptr)
    , m_pViewer(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
}

COperateTemplateBase::~COperateTemplateBase()
{
    qDebug(log) << Q_FUNC_INFO;
}

CParameterTemplateBase* COperateTemplateBase::GetParameter() const
{
    return m_pPara;
}

const qint16 COperateTemplateBase::Version() const
{
    // TODO: Add version
    return 0;
}

QWidget *COperateTemplateBase::GetViewer()
{
    return m_pViewer;
}

int COperateTemplateBase::Start()
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

int COperateTemplateBase::Stop()
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

CBackend* COperateTemplateBase::InstanceBackend()
{
    return new CBackendTemplateBase(this);
}

int COperateTemplateBase::SetPluginParameters(CParameterPlugin *pPara)
{
    int nRet = 0;
    // TODO: Modify apply plugin parameters
    m_pPara->SetPluginParameters(pPara);
    return nRet;
}

QDialog *COperateTemplateBase::OnOpenDialogSettings(QWidget *parent)
{
    return new CDlgSettingsTemplateBase(m_pPara, parent);
}

int COperateTemplateBase::Load(QSettings &set)
{
    int nRet = 0;

    if(m_pPara)
        nRet = m_pPara->Load(set);

    return nRet;
}

int COperateTemplateBase::Save(QSettings &set)
{
    int nRet = 0;
    if(m_pPara)
        nRet = m_pPara->Save(set);
    return nRet;
}

int COperateTemplateBase::Initial()
{
    int nRet = 0;
    nRet = COperate::Initial();
    m_pPara = new CParameterTemplateBase();
    m_pViewer = new CFrmViewerTemplateBase();

    // TODO: add initial

    m_Menu.addAction(m_pActionSettings);
    return nRet;
}

int COperateTemplateBase::Clean()
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
