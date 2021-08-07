#include "Service.h"
#include <QTimer>
#include <QSettings>
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

bool CService::Enable()
{
    return true;
}

int CService::Init()
{
    int nRet = 0;
    if(GetParameters())
    {
        QString szFile = RabbitCommon::CDir::Instance()->GetFileUserConfigure();
        QSettings set(szFile, QSettings::IniFormat);
        szFile = set.value("Configure/File" + m_pPlugin->Id(),
                           RabbitCommon::CDir::Instance()->GetDirUserConfig()
                           + QDir::separator()
                           + m_pPlugin->Id()
                           + ".rrs").toString();
        LOG_MODEL_DEBUG("Service", "Configure file: %s", szFile.toStdString().c_str());
        nRet = GetParameters()->OnLoad(szFile);
    }
    OnInit();
    QTimer::singleShot(0, this, SLOT(slotProcess()));
    return nRet;
}

int CService::OnInit()
{
    return 0;
}

int CService::Clean()
{
    int nRet = 0;

    //TODO: save parameter to file
//    if(GetParameters())
//    {
//        QString szFile = RabbitCommon::CDir::Instance()->GetFileUserConfigure();
//        QSettings set(szFile, QSettings::IniFormat);
//        szFile = set.value("Configure/File" + m_pPlugin->Id(),
//                           RabbitCommon::CDir::Instance()->GetDirUserConfig()
//                           + QDir::separator()
//                           + m_pPlugin->Id()
//                           + ".rrs").toString();
//        nRet = GetParameters()->OnSave(szFile);
//    }
    
    OnClean();

    if(m_pPara)
    {
        m_pPara->deleteLater();
        m_pPara = nullptr;
    }
    return nRet;
}

int CService::OnClean()
{
    return 0;
}

void CService::slotProcess()
{
    int nRet = OnProcess();
    if(nRet < 0) return;
    if(0 == nRet)
        QTimer::singleShot(0, this, SLOT(slotProcess()));
}

int CService::OnProcess()
{
    return 0;
}

CParameterService* CService::GetParameters()
{
    return m_pPara;
}
