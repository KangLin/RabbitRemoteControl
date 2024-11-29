#include <QLoggingCategory>
#include <QRegularExpression>
#include "ConnecterConnect.h"
#include "PluginClientThread.h"

static Q_LOGGING_CATEGORY(log, "Client.Connecter.Connect")
CConnecterConnect::CConnecterConnect(CPluginClient *plugin)
    : CConnecter(plugin)
    , m_pConnect(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
}

CConnecterConnect::~CConnecterConnect()
{
    qDebug(log) << Q_FUNC_INFO;
}

const QString CConnecterConnect::Id()
{
    QString szId = Protocol() + "_" + GetPlugClient()->Name();
    if(GetParameter())
    {
        if(!GetParameter()->GetName().isEmpty())
            szId += "_" + GetParameter()->GetName();
        if(!GetParameter()->m_Net.GetHost().isEmpty())
            szId += "_" + GetParameter()->m_Net.GetHost()
                    + "_" + QString::number(GetParameter()->m_Net.GetPort());
    }
    static QRegularExpression exp("[-@:/#%!^&* \\.]");
    szId = szId.replace(exp, "_");
    return szId;
}

/*!
 * \~chinese
 * \brief 显示顺序：
 *        - 用户参数设置的名称
 *        - 如果允许，远程服务名。
 *        - 远程地址
 *
 * \~english
 *  Display order:
 *  - User parameter Name()
 *  - if enable, Server name
 *  - Host and port
 *
 * \~
 * \see ServerName()
 */
const QString CConnecterConnect::Name()
{
    QString szName;
    if(GetParameter() && GetParameter()->GetParameterClient()
        && GetParameter()->GetParameterClient()->GetShowProtocolPrefix())
        szName = Protocol() + ":";

    if(GetParameter() && !(GetParameter()->GetName().isEmpty()))
        szName += GetParameter()->GetName();
    else
        szName += ServerName();
    return szName;
}

const QString CConnecterConnect::Description()
{
    return tr("Name: ") + Name() + "\n"
           + tr("Protocol: ") + Protocol()
#ifdef DEBUG
           + " - " + GetPlugClient()->DisplayName()
#endif
           + "\n"
           + tr("Server name: ") + ServerName() + "\n"
           + tr("Description: ") + GetPlugClient()->Description();
}

CParameterBase *CConnecterConnect::GetParameter()
{
    return qobject_cast<CParameterBase*>(CConnecter::GetParameter());
}

int CConnecterConnect::SetParameter(CParameterBase *p)
{
    int nRet = CConnecter::SetParameter(p);
    if(nRet) return nRet;
    if(GetParameter())
    {
        bool check = false;
        check = connect(GetParameter(), SIGNAL(sigNameChanged()),
                        this, SLOT(slotUpdateName()));
        Q_ASSERT(check);
        check = connect(GetParameter(), SIGNAL(sigShowServerNameChanged()),
                        this, SLOT(slotShowServerName()));
        Q_ASSERT(check);
        check = connect(GetParameter(), &CParameter::sigChanged,
                        this, [&](){
                            emit this->sigUpdateParameters(this);
                        });
        Q_ASSERT(check);
        CFrmViewer* pViewer = qobject_cast<CFrmViewer*>(GetViewer());
        if(pViewer) {
            check = connect(GetParameter(), SIGNAL(sigZoomFactorChanged(double)),
                            pViewer, SLOT(slotSetZoomFactor(double)));
            Q_ASSERT(check);
            check = connect(
                GetParameter(),
                SIGNAL(sigAdaptWindowsChanged(CFrmViewer::ADAPT_WINDOWS)),
                pViewer, SLOT(slotSetAdaptWindows(CFrmViewer::ADAPT_WINDOWS)));
            Q_ASSERT(check);
        }
    }
    return 0;
}

int CConnecterConnect::SetParameterClient(CParameterClient* pPara)
{
    if(GetParameter())
    {
        GetParameter()->SetParameterClient(pPara);
        if(pPara)
        {
            bool check = connect(pPara, SIGNAL(sigShowProtocolPrefixChanged()),
                                 this, SLOT(slotUpdateName()));
            Q_ASSERT(check);
            check = connect(pPara, SIGNAL(sigSHowIpPortInNameChanged()),
                            this, SLOT(slotUpdateName()));
            Q_ASSERT(check);
        }
        return 0;
    } else {
        QString szMsg = "The CConnecter is not parameters! "
                        "please first create parameters, "
                        "then call SetParameter in the ";
        szMsg += metaObject()->className() + QString("::")
                 + metaObject()->className();
        szMsg += QString(" or ") + metaObject()->className()
                 + QString("::") + "Initial()";
        szMsg += " to set the parameters pointer. "
                 "Default set CParameterClient for the parameters of connecter "
                 "(CParameterConnecter or its derived classes) "
                 "See: CClient::CreateConnecter. "
                 "If you are sure the parameter of connecter "
                 "does not need CParameterClient. "
                 "Please overload the SetParameterClient in the ";
        szMsg += QString(metaObject()->className()) + " . don't set it";
        qCritical(log) << szMsg.toStdString().c_str();
        Q_ASSERT(false);
    }
    return -1;
}

int CConnecterConnect::Connect()
{
    qDebug(log) << Q_FUNC_INFO;
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
    qDebug(log) << Q_FUNC_INFO;
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

QString CConnecterConnect::ServerName()
{
    if(GetParameter() && GetParameter()->GetParameterClient()
        && GetParameter()->GetParameterClient()->GetShowIpPortInName())
    {
        return GetParameter()->m_Net.GetHost()
        + ":" + QString::number(GetParameter()->m_Net.GetPort());
    }

    if(m_szServerName.isEmpty() && GetParameter())
        return GetParameter()->GetServerName();
    return m_szServerName;
}

void CConnecterConnect::slotSetServerName(const QString& szName)
{
    if(m_szServerName == szName)
        return;

    m_szServerName = szName;
    if(GetParameter())
    {
        if(GetParameter()->GetServerName() == szName)
            return;
        GetParameter()->SetServerName(szName);
    }

    emit sigUpdateName(Name());
}
