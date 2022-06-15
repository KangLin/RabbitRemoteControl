// Author: Kang Lin <kl222@126.com>

#include "Service.h"
#include <QTimer>
#include <QSettings>
#include <QDir>
#include "RabbitCommonLog.h"
#include "RabbitCommonDir.h"
#include "PluginService.h"

CService::CService(CPluginService *plugin) : QObject(),
    m_pPara(nullptr),
    m_pPlugin(plugin)
{
}

CService::~CService()
{
    LOG_MODEL_DEBUG("CService", "CService::~CService()");
}

int CService::Init()
{
    int nRet = 0;
    if(GetParameters())
    {
        nRet = LoadConfigure();
        if(nRet) return nRet;
    }

    if(!GetParameters() || !GetParameters()->getEnable())
    {
        LOG_MODEL_INFO("ServiceThread", "The service [%s] is disable",
                       m_pPlugin->Name().toStdString().c_str());
        return -2;
    } else
        LOG_MODEL_INFO("ServiceThread", "The service [%s] is start",
                       m_pPlugin->Name().toStdString().c_str());

    nRet = OnInit();
    if(0 == nRet)
        QTimer::singleShot(0, this, SLOT(slotProcess()));
    if(nRet > 0) return 0;
    return nRet;
}

int CService::Clean()
{
    int nRet = 0;

    OnClean();

    if(m_pPara)
    {
        m_pPara->deleteLater();
        m_pPara = nullptr;
    }
    return nRet;
}

void CService::slotProcess()
{
    int nRet = 0;
    try{
        nRet = OnProcess();
    } catch (std::exception& e) {
        LOG_MODEL_ERROR("CService", "OnProcess excption:%s", e.what());
    } catch (...) {
        LOG_MODEL_ERROR("CService", "OnProcess excption");
    }

    // TODO: Ignore error and stop
    if(nRet < 0)
    {
        LOG_MODEL_ERROR("CService", "OnProcess fail:%d", nRet);
        return;
    }
    QTimer::singleShot(nRet, this, SLOT(slotProcess()));
}

int CService::OnProcess()
{
    LOG_MODEL_WARNING("CService", "Need to implement CService::OnProcess()");
    return 0;
}

CParameterService* CService::GetParameters()
{
    return m_pPara;
}

int CService::LoadConfigure(const QString &szDir)
{
    int nRet = 0;
    QString szFolder = szDir;
    QString szFile;
    if(szFolder.isEmpty())
    {
        szFile = RabbitCommon::CDir::Instance()->GetFileUserConfigure();
        QSettings set(szFile, QSettings::IniFormat);
        szFolder = set.value("Configure/Folder",
                 RabbitCommon::CDir::Instance()->GetDirUserConfig()).toString();
        szFile = set.value("Configure/File/" + m_pPlugin->Id(),
                       szFolder
                       + QDir::separator()
                       + m_pPlugin->Id()
                       + ".rrs").toString();
    } else {
        szFile = szFolder + QDir::separator() + m_pPlugin->Id() + ".rrs";
    }
    LOG_MODEL_INFO("Service", "Configure file: %s", szFile.toStdString().c_str());
    QDir d;
    if(d.exists(szFile)){
        nRet = GetParameters()->Load(szFile);
        if(nRet)
        {
            LOG_MODEL_INFO("Service", "Load configure file fail:%d %s",
                           nRet, szFile.toStdString().c_str());
            return -1;
        }
    }
    return nRet;
}

int CService::SaveConfigure(const QString &szDir)
{
    int nRet = 0;
    if(GetParameters())
    {
        QString szFolder = szDir;
        QString szFile;
        if(szFolder.isEmpty())
        {
            szFile = RabbitCommon::CDir::Instance()->GetFileUserConfigure();
            QSettings set(szFile, QSettings::IniFormat);
            szFolder = set.value("Configure/Folder",
                     RabbitCommon::CDir::Instance()->GetDirUserConfig()).toString();
            szFile = set.value("Configure/File/" + m_pPlugin->Id(),
                           szFolder
                           + QDir::separator()
                           + m_pPlugin->Id()
                           + ".rrs").toString();
        } else {
            szFile = szFolder + QDir::separator() + m_pPlugin->Id() + ".rrs";
        }
        nRet = GetParameters()->Save(szFile);
    }
    return nRet;
}
