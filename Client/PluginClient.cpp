// Author: Kang Lin <kl222@126.com>

#include "PluginClient.h"
#include "RabbitCommonTools.h"

#include <QLocale>
#include <QDebug>
#include <QApplication>
#include <QDir>
#include <QLoggingCategory>

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
