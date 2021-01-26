//! @author: Kang Lin (kl222@126.com)

#include "ManageConnecter.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonLog.h"
#include <QPluginLoader>
#include <QDebug>
#include <QtPlugin>
#include <QDataStream>
#include <QFile>
#include <QCoreApplication>

CManageConnecter::CManageConnecter(QObject *parent) : QObject(parent),
    m_FileVersion(0)  //TODO: update it if update data
{
    LoadPlugins();
}

CManageConnecter::~CManageConnecter()
{
    qDebug() << "CManageConnecter::~CManageConnecter()";
}

int CManageConnecter::LoadPlugins()
{
    foreach (QObject *plugin, QPluginLoader::staticInstances())
    {
        CPluginFactory* p = qobject_cast<CPluginFactory*>(plugin);
        if(p)
        {
            m_Plugins.insert(p->Id(), p);
        }
    }

    QString szPath = RabbitCommon::CDir::Instance()->GetDirPlugins();
    QStringList filters;
#if defined (Q_OS_WINDOWS)
        filters << "*.dll";
#else
        filters << "*.so";
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
        filters << "*.dll";
#else
        filters << "*.so";
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
            CPluginFactory* p = qobject_cast<CPluginFactory*>(plugin);
            if(p)
            {
                m_Plugins.insert(p->Id(), p);
            }
        }else{
            QString szMsg;
            szMsg = "load plugin error: " + loader.errorString();
            LOG_MODEL_ERROR("ManageConnecter", szMsg.toStdString().c_str());
        }
    }
    
    QDir::setCurrent(szCurrentPath);

    return 0;
}

CConnecter* CManageConnecter::CreateConnecter(const QString& id)
{
    auto it = m_Plugins.find(id);
    if(m_Plugins.end() != it)
        return it.value()->CreateConnecter(id);
    return nullptr;
}

CConnecter* CManageConnecter::LoadConnecter(const QString &szFile)
{
    CConnecter* pConnecter = nullptr;
    QFile f(szFile);
    if(!f.open(QFile::ReadOnly))
    {
        LOG_MODEL_ERROR("ManageConnecter", "Load connecter: Open file fail: %s",
                        szFile.toStdString().c_str());
        return nullptr;
    }
    
    try{
        QDataStream d(&f);
        d >> m_FileVersion;
        QString id;
        d >> id;
        QString protol;
        d >> protol;
        QString name;
        d >> name;
        LOG_MODEL_INFO("ManageConnecter", "protol: %s  name: %s",
                       protol.toStdString().c_str(),
                       name.toStdString().c_str());
        pConnecter = CreateConnecter(id);
        if(pConnecter)
            pConnecter->Load(d);
        else
            LOG_MODEL_ERROR("ManageConnecter", "Don't create connecter: %s",
                            protol.toStdString().c_str());
    } catch(...) {
        LOG_MODEL_ERROR("ManageConnecter", "Load connecter exception");
    }

    f.close();

    return pConnecter;
}

int CManageConnecter::SaveConnecter(const QString &szFile, CConnecter *pConnecter)
{
    if(!pConnecter) return -1;

    QFile f(szFile);
    if(!f.open(QFile::WriteOnly))
    {
        LOG_MODEL_ERROR("ManageConnecter",
                        "Save connecter: Open file fail: %s",
                        szFile.toStdString().c_str());
        return -1;
    }

    QDataStream d(&f);
    d << m_FileVersion;
    const CPluginFactory* pFactory = pConnecter->GetPluginFactory();
    // In the CManageConnecter derived class,
    // the CreateConnecter function constructs the derived class of CConnecter,
    // and its parent pointer must be specified as the corresponding CPluginFactory derived class
    Q_ASSERT(pFactory);
    d << pFactory->Id()
      << pFactory->Protol()
      << pFactory->Name();
    pConnecter->Save(d);

    f.close();
    return 0;
}

int CManageConnecter::EnumPlugins(Handle *handle)
{
    int nRet = 0;
    QMap<QString, CPluginFactory*>::iterator it;
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
