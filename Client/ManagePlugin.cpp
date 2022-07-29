// Author: Kang Lin <kl222@126.com>

#include "ManagePlugin.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonLog.h"
#include "FrmParameterClient.h"

#include <QPluginLoader>
#include <QDebug>
#include <QtPlugin>
#include <QDataStream>
#include <QFile>
#include <QCoreApplication>
#include <QSettings>

CManagePlugin::CManagePlugin(QObject *parent) : QObject(parent),
    m_FileVersion(1)  //TODO: update version it if update data
{
    bool check = false;
//#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
//    Q_INIT_RESOURCE(translations_Client);
//#endif

    QString szTranslatorFile = RabbitCommon::CDir::Instance()->GetDirTranslations()
            + "/Client_" + QLocale::system().name() + ".qm";
    if(!m_Translator.load(szTranslatorFile))
        qCritical() << "Open translator file fail:" << szTranslatorFile;
    qApp->installTranslator(&m_Translator);

    LoadPlugins();

    check = connect(&m_ParameterClient, SIGNAL(sigHookKeyboardChanged()),
                    this, SLOT(slotHookKeyboardChanged()));
    Q_ASSERT(check);
    if(m_ParameterClient.GetHookKeyboard())
        m_Hook = QSharedPointer<CHook>(CHook::GetHook());
}

CManagePlugin::~CManagePlugin()
{
    qDebug() << "CManageConnecter::~CManageConnecter()";
    qApp->removeTranslator(&m_Translator);
    qDebug() << "CManagePlugin::~CManagePlugin()";
//#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
//    Q_CLEANUP_RESOURCE(translations_Client);
//#endif
}

int CManagePlugin::LoadPlugins()
{
    foreach (QObject *plugin, QPluginLoader::staticInstances())
    {
        CPluginClient* p = qobject_cast<CPluginClient*>(plugin);
        if(p)
        {
            if(m_Plugins.find(p->Id()) == m_Plugins.end())
            {
                m_Plugins.insert(p->Id(), p);
                p->InitTranslator();
            }
            else
                LOG_MODEL_ERROR("ManageConnecter", "The plugin [%s] is exist.",
                                p->Name().toStdString().c_str());
        }
    }

    QString szPath = RabbitCommon::CDir::Instance()->GetDirPlugins();
#if !defined (Q_OS_ANDROID)
    szPath = szPath + QDir::separator() + "Client";
#endif

    QStringList filters;
#if defined (Q_OS_WINDOWS)
        filters << "*PluginClient*.dll";
#else
        filters << "*PluginClient*.so";
#endif
    return FindPlugins(szPath, filters);
}

int CManagePlugin::FindPlugins(QDir dir, QStringList filters)
{
    QString szPath = dir.path();
    QString fileName;
    if(filters.isEmpty())
    {
#if defined (Q_OS_WINDOWS)
        filters << "*PluginClient*.dll";
#else
        filters << "*PluginClient*.so";
#endif
    }
    
    QString szCurrentPath = QDir::currentPath();
    QStringList files = dir.entryList(filters, QDir::Files);
    if(!files.isEmpty())
    {
        //This method is invalid
        //QCoreApplication::addLibraryPath(QDir::cleanPath(dir.absolutePath()));

        QDir::setCurrent(QDir::cleanPath(QDir::cleanPath(dir.absolutePath())));
        
        // This method is valid
//#if defined(Q_OS_WINDOWS)
//        QString szPath = QString::fromLocal8Bit(qgetenv("PATH"));
//        szPath += ";";
//        szPath += QDir::cleanPath(QDir::cleanPath(dir.absolutePath()));
//        qputenv("PATH", szPath.toLatin1());
//#endif
    }

    foreach (fileName, files) {
        QString szPlugins = dir.absoluteFilePath(fileName);
        QPluginLoader loader(szPlugins);
        QObject *plugin = loader.instance();
        if (plugin) {
            CPluginClient* p = qobject_cast<CPluginClient*>(plugin);
            if(p)
            {
                if(m_Plugins.find(p->Id()) == m_Plugins.end())
                {   
                    m_Plugins.insert(p->Id(), p);
                    p->InitTranslator();
                }
                else
                    LOG_MODEL_ERROR("ManageConnecter", "The plugin [%s] is exist.",
                                    p->Name().toStdString().c_str());
            }
        }else{
            QString szMsg;
            szMsg = "load plugin error: " + loader.errorString();
            LOG_MODEL_ERROR("ManageConnecter", szMsg.toStdString().c_str());
        }
        
        foreach (fileName, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            QDir pluginDir = dir;
            if(pluginDir.cd(fileName))
                FindPlugins(pluginDir, filters);
        }
    }
    
    QDir::setCurrent(szCurrentPath);

    return 0;
}

CConnecter* CManagePlugin::CreateConnecter(const QString& id)
{
    auto it = m_Plugins.find(id);
    if(m_Plugins.end() != it)
    {
        CConnecter* p = it.value()->CreateConnecter(id);
        if(p) p->SetParameterClient(&m_ParameterClient);
        return p;
    }
    return nullptr;
}

CConnecter* CManagePlugin::LoadConnecter(const QString &szFile)
{
    CConnecter* pConnecter = nullptr;
    if(szFile.isEmpty()) return nullptr;

    QSettings set(szFile, QSettings::IniFormat);
    m_FileVersion = set.value("Manage/FileVersion", m_FileVersion).toInt();
    QString id = set.value("Plugin/ID").toString();
    QString protol = set.value("Plugin/Protol").toString();
    QString name = set.value("Plugin/Name").toString();
    
    LOG_MODEL_INFO("ManageConnecter", "protol: %s  name: %s",
                   protol.toStdString().c_str(),
                   name.toStdString().c_str());
    pConnecter = CreateConnecter(id);
    if(pConnecter)
        pConnecter->Load(szFile);
    else
        LOG_MODEL_ERROR("ManageConnecter", "Don't create connecter: %s",
                        protol.toStdString().c_str());

    return pConnecter;
}

int CManagePlugin::SaveConnecter(QString szFile, CConnecter *pConnecter)
{
    if(!pConnecter) return -1;

    if(szFile.isEmpty())
        szFile = RabbitCommon::CDir::Instance()->GetDirUserData()
                + QDir::separator()
                + pConnecter->Id()
                + ".rrc";

    QSettings set(szFile, QSettings::IniFormat);
    
    const CPluginClient* pPluginClient = pConnecter->m_pPluginClient;
    Q_ASSERT(pPluginClient);
    
    set.setValue("Manage/FileVersion", m_FileVersion);
    set.setValue("Plugin/ID", pPluginClient->Id());
    set.setValue("Plugin/Protol", pPluginClient->Protol());
    set.setValue("Plugin/Name", pPluginClient->Name());
    pConnecter->Save(szFile);

    return 0;
}

int CManagePlugin::LoadSettings(QString szFile)
{
    return m_ParameterClient.CParameter::Load(szFile);
}

int CManagePlugin::SaveSettings(QString szFile)
{
    return m_ParameterClient.CParameter::Save(szFile);
}

QList<QWidget*> CManagePlugin::GetSettingsWidgets(QWidget* parent)
{
    QList<QWidget*> lstWidget;

    QWidget* p = new CFrmParameterClient(&m_ParameterClient, parent);
    if(p)
        lstWidget.push_back(p);
    return lstWidget;
}

int CManagePlugin::EnumPlugins(Handle *handle)
{
    int nRet = 0;
    QMap<QString, CPluginClient*>::iterator it;
    for(it = m_Plugins.begin(); it != m_Plugins.end(); it++)
    {
        if(handle->m_bIgnoreReturn)
            handle->onProcess(it.key(), it.value());
        else
        {
            nRet = handle->onProcess(it.key(), it.value());
            if(nRet)
                return nRet;
        }
    }
    return nRet;
}

void CManagePlugin::slotHookKeyboardChanged()
{
    if(m_ParameterClient.GetHookKeyboard())
        m_Hook = QSharedPointer<CHook>(CHook::GetHook());
    else
        m_Hook.reset();
}
