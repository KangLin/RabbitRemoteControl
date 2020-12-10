#include "ManageConnecter.h"
//! @author: Kang Lin(kl222@126.com)

#include "RabbitCommonDir.h"
#include "RabbitCommonLog.h"
#include <QPluginLoader>
#include <QDebug>
#include <QtPlugin>
#include <QDataStream>
#include <QFile>

CManageConnecter::CManageConnecter(QObject *parent) : QObject(parent),
    m_FileVersion(1)  //TODO: update it if update data
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
            m_Plugins.insert(p->Protol(), p);
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
    QStringList files = dir.entryList(filters, QDir::Files);
    foreach (fileName, files) {
        QString szPlugins = dir.absoluteFilePath(fileName);
        QPluginLoader loader(szPlugins);
        QObject *plugin = loader.instance();
        if (plugin) {
            CPluginFactory* p = qobject_cast<CPluginFactory*>(plugin);
            if(p)
            {
                m_Plugins.insert(p->Protol(), p);
            }
        }else{
            qDebug() << "load plugin error:" <<
                        loader.errorString().toStdString().c_str();
        }
    }

    return 0;
}

QList<CPluginFactory *> CManageConnecter::GetManageConnecter()
{
    QList<CPluginFactory*> connect;
    foreach(auto m, m_Plugins)
    {
        connect.push_back(m);
    }
    return connect;
}

CConnecter* CManageConnecter::CreateConnecter(const QString &szProtol)
{
    auto it = m_Plugins.find(szProtol);
    if(m_Plugins.end() != it)
        return it.value()->CreateConnecter(szProtol);
    
    return nullptr;
}

CConnecter* CManageConnecter::LoadConnecter(const QString &szFile)
{
    QFile f(szFile);
    if(!f.open(QFile::ReadOnly))
    {
        qCritical() << "Load connecter: Open file fail:" << szFile;
        return nullptr;
    }
    
    QDataStream d(&f);
    d >> m_FileVersion;
    QString protol;
    d >> protol;
    QString name;
    d >> name;
    LOG_MODEL_INFO("ManageConnecter", "protol: %s  name: %s",
                   protol.toStdString().c_str(),
                   name.toStdString().c_str());
    CConnecter* pConnecter = CreateConnecter(protol);
    if(pConnecter)
        pConnecter->Load(d);
    else
        LOG_MODEL_ERROR("ManageConnecter", "Don't create connecter: %s",
                        protol.toStdString().c_str());
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
    CPluginFactory* pFactory = dynamic_cast<CPluginFactory*>(pConnecter->parent());
    // In the CManageConnecter derived class,
    // the CreateConnecter function constructs the derived class of CConnecter,
    // and its parent pointer must be specified as the corresponding CManageConnecter derived class
    Q_ASSERT(pFactory);
    d << pFactory->Protol();
    d << pFactory->Name();
    pConnecter->Save(d);

    f.close();
    return 0;
}
