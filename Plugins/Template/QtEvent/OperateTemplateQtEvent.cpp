// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>

#include "BackendThread.h"
#include "BackendTemplateQtEvent.h"
#include "ParameterTemplateQtEvent.h"
#include "FrmViewerTemplateQtEvent.h"
#include "DlgSettingsTemplateQtEvent.h"
#include "OperateTemplateQtEvent.h"

static Q_LOGGING_CATEGORY(log, "TemplateQtEvent.Operate")
COperateTemplateQtEvent::COperateTemplateQtEvent(CPlugin *plugin)
    : COperate(plugin)
    , m_pPara(nullptr)
    , m_pViewer(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
}

COperateTemplateQtEvent::~COperateTemplateQtEvent()
{
    qDebug(log) << Q_FUNC_INFO;
    QString szClass = this->metaObject()->className();
    QString szWhat;
    szWhat += "Please call ";
    szWhat += szClass;
    szWhat += "::Stop() first, then call " + szClass + "::Clean()";
    Q_ASSERT_X(!m_pPara && !m_pViewer,
               szClass.toStdString().c_str(), szWhat.toStdString().c_str());
}

CParameterTemplateQtEvent* COperateTemplateQtEvent::GetParameter() const
{
    return m_pPara;
}

const qint16 COperateTemplateQtEvent::Version() const
{
    // TODO: Add version
    return 0;
}

QWidget *COperateTemplateQtEvent::GetViewer()
{
    return m_pViewer;
}

int COperateTemplateQtEvent::Start()
{
    int nRet = 0;
    emit sigNewBackend(this);
    return nRet;
}

int COperateTemplateQtEvent::Stop()
{
    int nRet = 0;
    emit sigDeleteBackend(this);
    return nRet;
}

CBackend* COperateTemplateQtEvent::InstanceBackend()
{
    return new CBackendTemplateQtEvent(this);
}

int COperateTemplateQtEvent::SetPluginParameters(CParameterPlugin *pPara)
{
    int nRet = 0;
    // TODO: Modify apply plugin parameters
    if(m_pPara) {
        m_pPara->SetPluginParameters(pPara);
    }
    return nRet;
}

QDialog *COperateTemplateQtEvent::OnOpenDialogSettings(QWidget *parent)
{
    return new CDlgSettingsTemplateQtEvent(m_pPara, parent);
}

int COperateTemplateQtEvent::Load(QSettings &set)
{
    int nRet = 0;

    if(m_pPara)
        nRet = m_pPara->Load(set);

    return nRet;
}

int COperateTemplateQtEvent::Save(QSettings &set)
{
    int nRet = 0;
    if(m_pPara)
        nRet = m_pPara->Save(set);
    return nRet;
}

int COperateTemplateQtEvent::Initial()
{
    int nRet = 0;
    nRet = COperate::Initial();
    m_pPara = new CParameterTemplateQtEvent();
    m_pViewer = new CFrmViewerTemplateQtEvent();

    // TODO: add initial

    if(m_pActionSettings)
        m_Menu.addAction(m_pActionSettings);
    return nRet;
}

int COperateTemplateQtEvent::Clean()
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
