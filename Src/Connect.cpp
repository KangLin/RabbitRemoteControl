#include "Connect.h"

CConnect::CConnect(CFrmViewer *pView, QObject *parent) : QObject(parent)
{
    m_pView = pView;
    m_bExit = false;
}

CConnect::~CConnect()
{}

int CConnect::SetServer(const QString &szIp, const int port)
{
    m_szIp = szIp;
    m_nPort = port;
    return 0;
}

int CConnect::SetUser(const QString &szUser, const QString &szPassword)
{
    m_szUser = szUser;
    m_szPassword = szPassword;
    return 0;
}

int CConnect::SetParamter(void *pPara)
{
    Q_UNUSED(pPara)
    return 0;
}

int CConnect::Initialize()
{
    return 0;
}

int CConnect::Connect()
{
    return 0;
}

int CConnect::Disconnect()
{
    m_bExit = true;
    return 0;
}

int CConnect::Exec()
{
    return 0;
}

QString CConnect::GetServerName()
{
    return QString();
}
