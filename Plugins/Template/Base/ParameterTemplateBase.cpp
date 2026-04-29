// Author: Kang Lin <kl222@126.com>

#include "ParameterTemplateBase.h"

CParameterTemplateBase::CParameterTemplateBase(QObject *parent, const QString &szPrefix)
    : CParameterOperate{parent, szPrefix}
{}

int CParameterTemplateBase::OnLoad(QSettings &set)
{
    int nRet = 0;
    // TODO: load parameter

    return nRet;
}

int CParameterTemplateBase::OnSave(QSettings &set)
{
    int nRet = 0;
    // TODO: save parameter

    return nRet;
}
