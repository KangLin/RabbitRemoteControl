// Author: Kang Lin <kl222@126.com>

#include "ParameterSftpServer.h"

CParameterSftpServer::CParameterSftpServer(QObject *parent, const QString &szPrefix)
    : CParameterServer{parent, szPrefix}
{
}

int CParameterSftpServer::OnLoad(QSettings &set)
{
    int nRet = 0;
    nRet = CParameterServer::OnLoad(set);
    if(nRet) return nRet;

    return nRet;
}

int CParameterSftpServer::OnSave(QSettings &set)
{
    int nRet = 0;
    nRet = CParameterServer::OnSave(set);
    if(nRet) return nRet;

    return nRet;
}
