// Author: Kang Lin <kl222@126.com>

#include "ParameterTemplateBase.h"

CParameterTemplateBase::CParameterTemplateBase(QObject *parent, const QString &szPrefix)
    : CParameterOperate{parent, szPrefix}
{}

int CParameterTemplateBase::OnLoad(QSettings &set)
{
    int nRet = 0;
    // TODO: Load parameter

    return nRet;
}

int CParameterTemplateBase::OnSave(QSettings &set)
{
    int nRet = 0;
    // TODO: Save parameter

    return nRet;
}
