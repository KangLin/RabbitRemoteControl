#include <QLoggingCategory>

#include "ConnecterConnect.h"
#include "PluginClientThread.h"

static Q_LOGGING_CATEGORY(log, "Client.Connecter.Connect")
CConnecterConnect::CConnecterConnect(CPluginClient *plugin)
    : CConnecter(plugin)
    , m_pConnect(nullptr)
{
    qDebug(log) << "CConnecterConnect::CConnecterConnect";
}

CConnecterConnect::~CConnecterConnect()
{
    qDebug(log) << "CConnecterConnect::~CConnecterConnect()";
}

CParameterBase *CConnecterConnect::GetParameter()
{
    return CConnecter::GetParameter();
}

int CConnecterConnect::Load(QSettings &set)
{
    int nRet = 0;
    Q_ASSERT(GetParameter());
    if(GetParameter())
        nRet = GetParameter()->Load(set);
    return nRet;
}

int CConnecterConnect::Save(QSettings &set)
{
    int nRet = 0;
    Q_ASSERT(GetParameter());
    if(GetParameter()) {
        GetParameter()->Save(set);
    }
    return nRet;
}

int CConnecterConnect::Connect()
{
    qDebug(log) << "CConnecterDesktop::Connect";
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
    qDebug(log) << "CConnecterDesktop::DisConnect";
    CPluginClientThread* pPlug
        = qobject_cast<CPluginClientThread*>(GetPlugClient());
    if(pPlug)
        emit sigCloseconnect(this);
    else {
        if(m_pConnect) {
            int nRet = m_pConnect->Disconnect();
            emit sigDisconnected();
            return nRet;
        }
    }
    return 0;
}
