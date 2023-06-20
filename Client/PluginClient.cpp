// Author: Kang Lin <kl222@126.com>

#include "PluginClient.h"
#include "RabbitCommonDir.h"

#include <QLocale>
#include <QDebug>
#include <QApplication>
#include <QDir>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(Client)

CPluginClient::CPluginClient(QObject *parent) : QObject(parent)
{
}

CPluginClient::~CPluginClient()
{
    qApp->removeTranslator(&m_Translator);
}

int CPluginClient::InitTranslator()
{
    QString szTranslatorFile = RabbitCommon::CDir::Instance()->GetDirPluginsTranslation("plugins/Client")
            + QDir::separator() + Name() + "_" + QLocale::system().name() + ".qm";
    if(!m_Translator.load(szTranslatorFile))
    {
        qDebug(Client) << "Open translator file fail:" << szTranslatorFile;
        return -1;
    }
    qApp->installTranslator(&m_Translator);
    return 0;
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
