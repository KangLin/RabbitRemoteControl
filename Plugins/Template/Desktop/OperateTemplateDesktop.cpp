// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>

#include "DlgSettingsTemplateDesktop.h"
#include "OperateTemplateDesktop.h"
#include "ParameterTemplateDesktop.h"
#include "BackendTemplateDesktop.h"

static Q_LOGGING_CATEGORY(log, "Operate.TemplateDesktop")
COperateTemplateDesktop::COperateTemplateDesktop(CPlugin *plugin)
    : COperateDesktop(plugin)
{
    qDebug(log) << Q_FUNC_INFO;
    m_pPara = new CParameterTemplateDesktop();
    SetParameter(m_pPara);
}

COperateTemplateDesktop::~COperateTemplateDesktop()
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_pPara)
        delete m_pPara;
}

QDialog *COperateTemplateDesktop::OnOpenDialogSettings(QWidget *parent)
{
    return new CDlgSettingsTemplateDesktop(m_pPara, parent);
}

CBackend *COperateTemplateDesktop::InstanceBackend()
{
    return new CBackendTemplateDesktop(this);
}