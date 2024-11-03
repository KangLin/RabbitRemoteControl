#include <QLoggingCategory>

#include "ConnecterConnect.h"
#include "PluginClientThread.h"

static Q_LOGGING_CATEGORY(log, "Client.Connecter.Connect")
CConnecterConnect::CConnecterConnect(CPluginClient *plugin)
    : CConnecter(plugin)
    , m_pConnect(nullptr)
{
    qDebug(log) << __FUNCTION__;
}

CConnecterConnect::~CConnecterConnect()
{
    qDebug(log) << __FUNCTION__;
}

CParameterBase *CConnecterConnect::GetParameter()
{
    return CConnecter::GetParameter();
}

int CConnecterConnect::Connect()
{
    qDebug(log) << __FUNCTION__;
    CPluginClientThread* pPlug
        = qobject_cast<CPluginClientThread*>(GetPlugClient());
    if(pPlug)
        emit sigOpenConnect(this);
    else {
        m_pConnect = InstanceConnect();
        if(m_pConnect) {
            int nRet = m_pConnect->Connect();
            emit sigConnected();
            return nRet;
        }
    }
    return 0;
}

int CConnecterConnect::DisConnect()
{
    qDebug(log) << __FUNCTION__;
    CPluginClientThread* pPlug
        = qobject_cast<CPluginClientThread*>(GetPlugClient());
    if(pPlug)
        emit sigCloseconnect(this);
    else {
        if(m_pConnect) {
            int nRet = m_pConnect->Disconnect();
            m_pConnect->deleteLater();
            emit sigDisconnected();
            return nRet;
        }
    }
    return 0;
}
