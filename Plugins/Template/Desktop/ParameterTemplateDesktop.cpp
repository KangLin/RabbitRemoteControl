// Author: Kang Lin <kl222@126.com>

#include "ParameterTemplateDesktop.h"

CParameterTemplateDesktop::CParameterTemplateDesktop(
    CParameterOperate *parent, const QString &szPrefix)
    : CParameterDesktop{parent, szPrefix}
{}

int CParameterTemplateDesktop::OnLoad(QSettings &set)
{
    int nRet = 0;
    // TODO: Load parameter

    return nRet;
}

int CParameterTemplateDesktop::OnSave(QSettings &set)
{
    int nRet = 0;
    // TODO: Save parameter

    return nRet;
}
