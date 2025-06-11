// Author: Kang Lin <kl222@126.com>

#include <QDir>
#include <QLoggingCategory>
#include "RabbitCommonTools.h"

#include "Plugin.h"

static Q_LOGGING_CATEGORY(log, "Plugin")

CPlugin::CPlugin(QObject *parent)
    : QObject{parent}
{}

CPlugin::~CPlugin()
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_Translator)
        RabbitCommon::CTools::Instance()->RemoveTranslator(m_Translator);
}

// 因为 Name() 是重载函数，需要子类化才有，所以此函数不能在构造函数中调用
int CPlugin::InitTranslator()
{
    m_Translator = RabbitCommon::CTools::Instance()->InstallTranslator(
        Name(),
        RabbitCommon::CTools::TranslationType::Plugin,
        "plugins");
    if(m_Translator)
        return 0;
    
    return -1;
}

const QString CPlugin::GetType(const TYPE t)
{
    switch(t)
    {
    case TYPE::Client:
        return "Client";
    case TYPE::Service:
        return "Service";
    case TYPE::Tool:
        return "Tool";
    }
    return QString();
}

const QString CPlugin::Id()
{
    return GetType(Type()) + ":" + Protocol() + ":" + Name();
}

const QString CPlugin::DisplayName() const
{
    return Name();
}

const QString CPlugin::Details() const
{
    return QString();
}

COperate* CPlugin::CreateOperate(
    const QString &szId, CParameterPlugin* para)
{
    COperate* p = OnCreateOperate(szId);
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
            qCritical(log) << "Operate initial fail" << nRet;
            DeleteOperate(p);
            return nullptr;
        }
        //nRet = p->SetParameterPlugin(para);
        bRet = QMetaObject::invokeMethod(
            p,
            "SetParameterPlugin",
            Qt::DirectConnection,
            Q_RETURN_ARG(int, nRet),
            Q_ARG(CParameterPlugin*, para));
        if(!bRet) {
            qCritical(log) << "Call p->SetParameterClient(para) fail.";
            return nullptr;
        }
        if(nRet) {
            qCritical(log) << "SetParameterClient fail" << nRet;
            DeleteOperate(p);
            return nullptr;
        }
    }
    return p;
}

int CPlugin::DeleteOperate(COperate *p)
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
        qWarning(log) << "Operate clean fail" << nRet;
    p->deleteLater();
    return 0;
}
