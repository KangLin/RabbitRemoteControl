// Author: Kang Lin <kl222@126.com>

#include <QLocale>
#include <QDebug>
#include <QApplication>
#include <QDir>
#include <QLoggingCategory>

#include "PluginClient.h"
#include "RabbitCommonTools.h"

static Q_LOGGING_CATEGORY(log, "Client.Plugin")

CPluginClient::CPluginClient(QObject *parent) : QObject(parent)
{}

CPluginClient::~CPluginClient()
{
    qDebug(log) << "CPluginClient::~CPluginClient()";
    if(m_Translator)
        RabbitCommon::CTools::Instance()->RemoveTranslator(m_Translator);
}

// 因为 Name() 是重载函数，需要子类化才有，所以此函数不能在构造函数中调用
int CPluginClient::InitTranslator()
{
    m_Translator = RabbitCommon::CTools::Instance()->InstallTranslator(
        Name(),
        RabbitCommon::CTools::TranslationType::Plugin,
        "plugins/Client");
    if(m_Translator)
        return 0;

    return -1;
}

const QString CPluginClient::Id() const
{
    return Protocol() + ":" + Name();
}

const QString CPluginClient::DisplayName() const
{
    return Name();
}

const QIcon CPluginClient::Icon() const
{
    return QIcon::fromTheme("network-wired");
}

const QString CPluginClient::Details() const
{
    return QString();
}

CConnecter* CPluginClient::CreateConnecter(
    const QString &szId, CParameterClient* para)
{
    CConnecter* p = OnCreateConnecter(szId);
    if(p) {
        int nRet = 0;
        //nRet = p->Initial();
        bool bRet = QMetaObject::invokeMethod(
            p,
            "Initial",
            Qt::DirectConnection,
            Q_RETURN_ARG(int, nRet));
        if(!bRet) {
            qCritical(log) << "Call p->Initial() fail.";
            return nullptr;
        }
        if(nRet) {
            qCritical(log) << "Connecter initial fail" << nRet;
            DeleteConnecter(p);
            return nullptr;
        }
        //nRet = p->SetParameterClient(para);
        bRet = QMetaObject::invokeMethod(
            p,
            "SetParameterClient",
            Qt::DirectConnection,
            Q_RETURN_ARG(int, nRet),
            Q_ARG(CParameterClient*, para));
        if(!bRet) {
            qCritical(log) << "Call p->SetParameterClient(para) fail.";
            return nullptr;
        }
        if(nRet) {
            qCritical(log) << "SetParameterClient fail" << nRet;
            DeleteConnecter(p);
            return nullptr;
        }
    }
    return p;
}

int CPluginClient::DeleteConnecter(CConnecter *p)
{
    qDebug(log) << Q_FUNC_INFO;
    if(!p) return 0;
    int nRet = 0;
    bool bRet = false;
    //nRet = p->Clean();
    bRet = QMetaObject::invokeMethod(
        p,
        "Clean",
        Qt::DirectConnection,
        Q_RETURN_ARG(int, nRet));
    if(!bRet) {
        qCritical(log) << "Call p->Clean() fail.";
        return -1;
    }
    if(nRet)
        qWarning(log) << "Connecter clean fail" << nRet;
    p->deleteLater();
    return 0;
}
