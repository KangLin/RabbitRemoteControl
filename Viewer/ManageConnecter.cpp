// Author: Kang Lin <kl222@126.com>

#include "ManageConnecter.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonLog.h"
#include <QPluginLoader>
#include <QDebug>
#include <QtPlugin>
#include <QDataStream>
#include <QFile>
#include <QCoreApplication>
#include <QSettings>

CManageConnecter::CManageConnecter(QObject *parent) : QObject(parent),
    m_FileVersion(1)  //TODO: update version it if update data
{
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_INIT_RESOURCE(translations_Viewer);
#endif

    QString szTranslatorFile = RabbitCommon::CDir::Instance()->GetDirTranslations()
            + "/Viewer_" + QLocale::system().name() + ".qm";
    if(!m_Translator.load(szTranslatorFile))
        qCritical() << "Open translator file fail:" << szTranslatorFile;
    qApp->installTranslator(&m_Translator);
    
    LoadPlugins();
}

CManageConnecter::~CManageConnecter()
{
    qDebug() << "CManageConnecter::~CManageConnecter()";
    qApp->removeTranslator(&m_Translator);
    qDebug() << "CPluginFactoryFreeRdp::~CPluginFactoryFreeRdp()";
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_CLEANUP_RESOURCE(translations_Viewer);
#endif
}

int CManageConnecter::LoadPlugins()
{
    foreach (QObject *plugin, QPluginLoader::staticInstances())
    {
        CPluginViewer* p = qobject_cast<CPluginViewer*>(plugin);
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
    szPath = szPath + QDir::separator() + "Viewer";
#endif

    QStringList filters;
#if defined (Q_OS_WINDOWS)
        filters << "*PluginViewer*.dll";
#else
        filters << "*PluginViewer*.so";
#endif
    return FindPlugins(szPath, filters);
}

int CManageConnecter::FindPlugins(QDir dir, QStringList filters)
{
    QString szPath = dir.path();
    QString fileName;
    if(filters.isEmpty())
    {
#if defined (Q_OS_WINDOWS)
        filters << "*PluginViewer*.dll";
#else
        filters << "*PluginViewer*.so";
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
            CPluginViewer* p = qobject_cast<CPluginViewer*>(plugin);
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

CConnecter* CManageConnecter::CreateConnecter(const QString& id)
{
    auto it = m_Plugins.find(id);
    if(m_Plugins.end() != it)
    {
        return it.value()->CreateConnecter(id);
    }
    return nullptr;
}

CConnecter* CManageConnecter::LoadConnecter(const QString &szFile)
{
    CConnecter* pConnecter = nullptr;
   
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

int CManageConnecter::SaveConnecter(QString szFile, CConnecter *pConnecter)
{
    if(!pConnecter) return -1;

    if(szFile.isEmpty())
        szFile = RabbitCommon::CDir::Instance()->GetDirUserData()
                + QDir::separator()
                + pConnecter->Id()
                + ".rrc";

    QSettings set(szFile, QSettings::IniFormat);
    
    const CPluginViewer* pPluginViewer = pConnecter->m_pPluginViewer;
    Q_ASSERT(pPluginViewer);
    
    set.setValue("Manage/FileVersion", m_FileVersion);
    set.setValue("Plugin/ID", pPluginViewer->Id());
    set.setValue("Plugin/Protol", pPluginViewer->Protol());
    set.setValue("Plugin/Name", pPluginViewer->Name());
    pConnecter->Save(szFile);

    return 0;
}

int CManageConnecter::EnumPlugins(Handle *handle)
{
    int nRet = 0;
    QMap<QString, CPluginViewer*>::iterator it;
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
