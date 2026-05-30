// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>

#include "DlgSettingsTemplateDesktop.h"
#include "ParameterTemplateDesktop.h"
#include "BackendTemplateDesktop.h"
#include "OperateTemplateDesktop.h"

static Q_LOGGING_CATEGORY(log, "Operate.TemplateDesktop")
COperateTemplateDesktop::COperateTemplateDesktop(CPlugin *plugin)
    : COperateDesktop(plugin)
    , m_pPara(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;

}

COperateTemplateDesktop::~COperateTemplateDesktop()
{
    qDebug(log) << Q_FUNC_INFO;
    QString szClass = this->metaObject()->className();
    QString szWhat;
    szWhat += "Please call ";
    szWhat += szClass;
    szWhat += "::Stop() first";
    Q_ASSERT_X(!m_pPara, szClass.toStdString().c_str(), szWhat.toStdString().c_str());
}

QDialog *COperateTemplateDesktop::OnOpenDialogSettings(QWidget *parent)
{
    return new CDlgSettingsTemplateDesktop(m_pPara, parent);
}

CBackend *COperateTemplateDesktop::InstanceBackend()
{
    return new CBackendTemplateDesktop(this);
}

int COperateTemplateDesktop::Initial()
{
    int nRet = 0;
    nRet = COperateDesktop::Initial();
    if(nRet) return nRet;
    m_pPara = new CParameterTemplateDesktop();
    if(m_pPara)
        SetParameter(m_pPara);
    return nRet;
}

int COperateTemplateDesktop::Clean()
{
    int nRet = 0;
    // TODO: add Clean

    if(m_pPara) {
        delete m_pPara;
        m_pPara = nullptr;
    }
    nRet = COperateDesktop::Clean();
    return nRet;
}
