// Author: Kang Lin <kl222@126.com>

#include "DlgSettingsTemplateBase.h"
#include "OperateTemplateBase.h"
#include "ParameterTemplateBase.h"

COperateTemplateBase::COperateTemplateBase(CPlugin *plugin)
    : COperate(plugin)
{
    m_pPara = new CParameterTemplateBase();
}

COperateTemplateBase::~COperateTemplateBase()
{
    if(m_pPara)
        delete m_pPara;
}

const qint16 COperateTemplateBase::Version() const
{
    // TODO: add version
    return 0;
}

QWidget *COperateTemplateBase::GetViewer()
{
    // TODO: add viewr
    return nullptr;
}

int COperateTemplateBase::Start()
{
    int nRet = 0;
    // TODO: add operate start

    return nRet;
}

int COperateTemplateBase::Stop()
{
    int nRet = 0;
    // TODO: add operate stop

    return nRet;
}

int COperateTemplateBase::SetPluginParameters(CParameterPlugin *pPara)
{
    int nRet = 0;
    // TODO: add apply plugin parameters

    return nRet;
}

QDialog *COperateTemplateBase::OnOpenDialogSettings(QWidget *parent)
{
    // TODO: add dialog settings
    return new CDlgSettingsTemplateBase(m_pPara, parent);
}

int COperateTemplateBase::Load(QSettings &set)
{
    int nRet = 0;
    if(m_pPara)
        m_pPara->Load(set);
    return nRet;
}

int COperateTemplateBase::Save(QSettings &set)
{
    int nRet = 0;
    if(m_pPara)
        m_pPara->Save(set);
    return nRet;
}
