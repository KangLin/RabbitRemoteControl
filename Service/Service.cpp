// Author: Kang Lin <kl222@126.com>

#include "Service.h"
#include <QTimer>
#include <QSettings>
#include <QDir>
#include "RabbitCommonDir.h"
#include "PluginService.h"
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(Service)

CService::CService(CPluginService *plugin) : QObject(),
    m_pPara(nullptr),
    m_pPlugin(plugin)
{
}

CService::~CService()
{
    qDebug(Service) << "CService::~CService()";
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
        qInfo(Service) << "The service" << m_pPlugin->Name() << "is disable";
        return -2;
    } else
        qInfo(Service) << "The service" << m_pPlugin->Name() << "is start";

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
        qCritical(Service) << "OnProcess excption:" << e.what();
    } catch (...) {
        qCritical(Service) << "OnProcess excption";
    }

    // TODO: Ignore error and stop
    if(nRet < 0)
    {
        qCritical(Service) << "OnProcess fail:", nRet;
        return;
    }
    QTimer::singleShot(nRet, this, SLOT(slotProcess()));
}

int CService::OnProcess()
{
    qWarning(Service) << "Need to implement CService::OnProcess()";
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
    qInfo(Service) << "Configure file:" << szFile;
    QDir d;
    if(d.exists(szFile)){
        nRet = GetParameters()->Load(szFile);
        if(nRet)
        {
            qInfo(Service) << "Load configure file fail:" << nRet << ":" << szFile;
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
