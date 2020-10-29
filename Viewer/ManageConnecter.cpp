#include "ManageConnecter.h"
#include "RabbitCommonDir.h"

#include <QPluginLoader>
#include <QDebug>
#include <QtPlugin>

CManageConnecter::CManageConnecter(QObject *parent) : QObject(parent)
{}

CManageConnecter::~CManageConnecter()
{
    qDebug() << "CManageConnecter::~CManageConnecter()";
}

QString CManageConnecter::Name()
{
    return this->metaObject()->className();
}

QString CManageConnecter::Description()
{
    return Name();
}

int CManageConnecter::LoadPlugins()
{
    foreach (QObject *plugin, QPluginLoader::staticInstances())
    {
        CConnecter* p = qobject_cast<CConnecter*>(plugin);
        if(!p)
            break;
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
            CManageConnecter* p = qobject_cast<CManageConnecter*>(plugin);
            if(!p)
            {
                return -1;
            }
            m_Plugins.insert(p->Name(), p);
        }else{
            qDebug() << "load plugin error:" <<
                        loader.errorString().toStdString().c_str();
        }
    }

    return 0;
}

CConnecter* CManageConnecter::CreateConnecter(const QString &szName)
{
    auto it = m_Plugins.find(szName);
    if(m_Plugins.end() != it)
        return it.value()->CreateConnecter(szName);
    
    return nullptr;
}
