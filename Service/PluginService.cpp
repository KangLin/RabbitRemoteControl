// Author: Kang Lin <kl222@126.com>

#include "PluginService.h"

#include "ServiceThread.h"
#include "RabbitCommonLog.h"
#include "RabbitCommonDir.h"

#include <QIcon>
#include <QLocale>
#include <QDebug>
#include <QApplication>

CPluginService::CPluginService(QObject *parent)
    : QObject(parent),
    m_pThread(nullptr)
{
}

CPluginService::~CPluginService()
{
    LOG_MODEL_DEBUG("CPluginService", "CPluginService::~CPluginService");
    qApp->removeTranslator(&m_Translator);
}

int CPluginService::InitTranslator()
{
    QString szTranslatorFile = RabbitCommon::CDir::Instance()->GetDirPluginsTranslation("plugins/Service")
            + "/" + Name() + "_" + QLocale::system().name() + ".qm";
    if(!m_Translator.load(szTranslatorFile))
    {
        LOG_MODEL_ERROR("CPluginViewer", "Open translator file fail:",
                        szTranslatorFile.toStdString().c_str());
        return -1;
    }
    qApp->installTranslator(&m_Translator);
    return 0;
}

const QString CPluginService::Id() const
{
    return Protol() + "_" + Name();
}

const QIcon CPluginService::Icon() const
{
    return QIcon(":/image/Connect");
}

void CPluginService::Start()
{
    if(!m_pThread)
    {
        m_pThread = new CServiceThread(this, this);    
    }

    if(m_pThread)
        m_pThread->start();
}

void CPluginService::Stop()
{
    if(m_pThread)
    {
        m_pThread->quit();
        m_pThread = nullptr;
    }
}
