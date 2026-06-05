// Author: Kang Lin <kl222@126.com>

#include "ParameterTemplateQtEvent.h"

CParameterTemplateQtEvent::CParameterTemplateQtEvent(QObject *parent, const QString &szPrefix)
    : CParameterOperate{parent, szPrefix}
{}

int CParameterTemplateQtEvent::OnLoad(QSettings &set)
{
    int nRet = 0;
    // TODO: Load parameter

    return nRet;
}

int CParameterTemplateQtEvent::OnSave(QSettings &set)
{
    int nRet = 0;
    // TODO: Save parameter

    return nRet;
}
