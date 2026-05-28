// Author: Kang Lin <kl222@126.com>

#include "ParameterTemplateServer.h"

CParameterTemplateServer::CParameterTemplateServer(QObject *parent, const QString &szPrefix)
    : CParameterServer{parent, szPrefix}
{}

int CParameterTemplateServer::OnLoad(QSettings &set)
{
    int nRet = 0;
    nRet = CParameterServer::OnLoad(set);
    if(nRet) return nRet;
    // TODO: Load parameter

    return nRet;
}

int CParameterTemplateServer::OnSave(QSettings &set)
{
    int nRet = 0;
    nRet = CParameterServer::OnSave(set);
    if(nRet) return nRet;
    // TODO: Save parameter

    return nRet;
}
