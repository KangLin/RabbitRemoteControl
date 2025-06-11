// Author: Kang Lin <kl222@126.com>

#include <QPluginLoader>
#include <QKeyEvent>
#include <QtPlugin>
#include <QFile>
#include <QApplication>
#include <QSettings>
#include <QLoggingCategory>
#include <QMessageBox>
#include <QCheckBox>

#include "RabbitCommonDir.h"
#include "RabbitCommonTools.h"
#include "FrmParameterClient.h"
#include "FrmViewer.h"
#include "Channel.h"
#include "ParameterRecordUI.h"
#include "Manager.h"

static Q_LOGGING_CATEGORY(log, "Manager")

CManager::CManager(QObject *parent, QString szFile) : QObject(parent)
    , m_FileVersion(1)  //TODO: update version it if update data
    , m_pHook(nullptr)
{
    bool check = false;
    //#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    //    Q_INIT_RESOURCE(translations_Client);
    //#endif

    m_Translator = RabbitCommon::CTools::Instance()->InstallTranslator(
        // TODO modify the name to Plugin
        "Client", RabbitCommon::CTools::TranslationType::Library);

    CChannel::InitTranslation();
    m_szSettingsFile = szFile;
    m_pParameter = new CParameterPlugin();
    if(m_pParameter) {
        CParameterClient* pParameterClient = &m_pParameter->m_Client;
        LoadSettings(m_szSettingsFile);
        check = connect(pParameterClient, &CParameterClient::sigNativeWindowRecieveKeyboard,
                        this, [&](){
                            if(pParameterClient->GetNativeWindowReceiveKeyboard()) {
                                if(m_pHook) {
                                    m_pHook->UnRegisterKeyboard();
                                    m_pHook->deleteLater();
                                    m_pHook = nullptr;
                                }
                            } else {
                                m_pHook = CHook::GetHook(pParameterClient, this);
                                if(m_pHook)
                                    m_pHook->RegisterKeyboard();
                            }
                        });
        m_pHook = CHook::GetHook(pParameterClient, this);
        if(m_pHook)
            m_pHook->RegisterKeyboard();
    } else {
        qCritical(log) << "new CParameterPlugin() fail";
        Q_ASSERT(m_pParameter);
    }

    LoadPlugins();
}

CManager::~CManager()
{
    qDebug(log) << "CManager::~CManager()";

    qApp->removeEventFilter(this);

    if(m_pHook) {
        m_pHook->UnRegisterKeyboard();
        m_pHook->deleteLater();
        m_pHook = nullptr;
    }

    if(m_pParameter) {
        m_pParameter->deleteLater();
        m_pParameter = nullptr;
    }

    if(m_Translator)
        RabbitCommon::CTools::Instance()->RemoveTranslator(m_Translator);

    CChannel::RemoveTranslation();

    //#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    //    Q_CLEANUP_RESOURCE(translations_Client);
    //#endif
}

int CManager::LoadPlugins()
{
    int nRet = 0;
    foreach (QObject *plugin, QPluginLoader::staticInstances())
    {
        CPlugin* p = qobject_cast<CPlugin*>(plugin);
        if(p)
        {
            if(m_Plugins.find(p->Id()) == m_Plugins.end())
            {
                qInfo(log) << "Success: Load plugin" << p->Name();
                AppendPlugin(p);
            }
            else
                qWarning(log) << "The plugin" << p->Name() << " is exist.";
        }
    }
    
    QString szPath = RabbitCommon::CDir::Instance()->GetDirPlugins();
    
    QStringList filters;
#if defined (Q_OS_WINDOWS) || defined(Q_OS_WIN)
    filters << "*.dll";
#else
    filters << "*.so";
#endif
    nRet = FindPlugins(szPath, filters);
    if(!m_szDetails.isEmpty())
        m_szDetails = tr("### Plugins") + "\n" + m_szDetails;
    
    qDebug(log) << ("Client details:\n" + Details()).toStdString().c_str();
    return nRet;
}

int CManager::FindPlugins(QDir dir, QStringList filters)
{
    QString fileName;
    if(filters.isEmpty())
    {
#if defined (Q_OS_WINDOWS) || defined(Q_OS_WIN)
        filters << "*.dll";
#elif defined(Q_OS_MACOS) || defined(Q_OS_MAC)
        filters << "*.dylib";
#else
        filters << "*.so";
#endif
    }

    QString szCurrentPath = QDir::currentPath();
    QStringList files = dir.entryList(filters, QDir::Files | QDir::NoDotAndDotDot);
    if(!files.isEmpty())
    {
        //This method is invalid
        //QCoreApplication::addLibraryPath(QDir::cleanPath(dir.absolutePath()));
        
        QDir::setCurrent(QDir::cleanPath(dir.absolutePath()));
        
        // This method is valid
        //#if defined(Q_OS_WINDOWS)
        //        QString szPath = QString::fromLocal8Bit(qgetenv("PATH"));
        //        szPath += ";";
        //        szPath += QDir::cleanPath(dir.absolutePath());
        //        qputenv("PATH", szPath.toLatin1());
        //#endif
    }
    
    foreach (fileName, files) {
        QString szPlugins = dir.absoluteFilePath(fileName);
        QPluginLoader loader(szPlugins);
        QObject *plugin = loader.instance();
        if (plugin) {
            CPlugin* p = qobject_cast<CPlugin*>(plugin);
            if(p)
            {
                if(m_Plugins.find(p->Id()) == m_Plugins.end())
                {
                    qInfo(log) << "Success: Load plugin"
                               << p->Name() << "from" << szPlugins;
                    AppendPlugin(p);
                }
                else
                    qWarning(log) << "The plugin [" << p->Name() << "] is exist.";
            }
        }else{
            QString szMsg;
            szMsg = "Error: Load plugin fail from " + szPlugins;
            if(!loader.errorString().isEmpty())
                szMsg += "; Error: " + loader.errorString();
            qCritical(log) << szMsg.toStdString().c_str();
        }
    }
    
    foreach (fileName, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QDir pluginDir = dir;
        if(pluginDir.cd(fileName))
            FindPlugins(pluginDir, filters);
    }
    
    QDir::setCurrent(szCurrentPath);
    
    return 0;
}

int CManager::AppendPlugin(CPlugin *p)
{
    if(!p) return -1;
    m_Plugins.insert(p->Id(), p);
    //p->InitTranslator();
    int val = 0;
    bool bRet = QMetaObject::invokeMethod(
        p,
        "InitTranslator",
        Qt::DirectConnection,
        Q_RETURN_ARG(int, val));
    if(!bRet || val)
    {
        qCritical(log) << "The plugin" <<  p->Name()
        << "initial translator fail" << bRet << val;
    }
    
    m_szDetails += "#### " + p->DisplayName() + "\n"
                   + tr("Version:") + " " + p->Version() + "  \n"
                   + p->Description() + "\n";
    if(!p->Details().isEmpty())
        m_szDetails += p->Details() + "\n";
    
    return 0;
}

//! [CManager CreateOperate]
COperate* CManager::CreateOperate(const QString& id)
{
    COperate* pOperate = nullptr;
    auto it = m_Plugins.find(id);
    if(m_Plugins.end() != it)
    {
        bool bRet = 0;
        qDebug(log) << "CreateOperate id:" << id;
        auto plugin = it.value();
        if(plugin) {
            //p = plugin->CreateOperate(id);
            bRet = QMetaObject::invokeMethod(
                plugin,
                "CreateOperate",
                Qt::DirectConnection,
                Q_RETURN_ARG(COperate*, pOperate),
                Q_ARG(QString, id),
                Q_ARG(CParameterPlugin*, m_pParameter));
            if(!bRet) {
                qCritical(log) << "Create COperate fail.";
                return nullptr;
            }
        }
    }
    return pOperate;
}
//! [CManager CreateOperate]

int CManager::DeleteOperate(COperate *p)
{
    qDebug(log) << Q_FUNC_INFO;
    if(!p) return 0;
    
    CPlugin* pPlugin = nullptr;
    //pPlugin->GetPlugin();
    bool bRet = QMetaObject::invokeMethod(
        p,
        "GetPlugin",
        Qt::DirectConnection,
        Q_RETURN_ARG(CPlugin*, pPlugin));
    
    if(bRet && pPlugin) {
        int nRet = 0;
        //pPlugin->DeleteOperate(p);
        bRet = QMetaObject::invokeMethod(
            pPlugin,
            "DeleteOperate",
            Qt::DirectConnection,
            Q_RETURN_ARG(int, nRet),
            Q_ARG(COperate*, p));
        if(!bRet) {
            nRet = -1;
            qCritical(log) << "Call pPlugin->DeleteOperate(p) fail";
        }
        return nRet;
    }
    
    qCritical(log) << "Get CManager fail.";
    return -1;
}

COperate* CManager::LoadOperate(const QString &szFile)
{
    COperate* pOperate = nullptr;
    if(szFile.isEmpty()) return nullptr;
    
    QSettings set(szFile, QSettings::IniFormat);
    m_FileVersion = set.value("Manage/FileVersion", m_FileVersion).toInt();
    QString id = set.value("Plugin/ID").toString();
    QString protocol = set.value("Plugin/Protocol").toString();
    QString name = set.value("Plugin/Name").toString();
    Q_UNUSED(name);
    qDebug(log) << "LoadOperate protocol:" << protocol
                << "name:" << name << "id:" << id;
    pOperate = CreateOperate(id);
    if(pOperate) {
        int nRet = false;
        //bRet = pOperate->Load(szFile);
        bool bRet = QMetaObject::invokeMethod(
            pOperate,
            "Load",
            Qt::DirectConnection,
            Q_RETURN_ARG(int, nRet),
            Q_ARG(QString, szFile));
        if(!bRet) {
            qCritical(log) << "Call pOperate->Load(szFile) fail.";
            return nullptr;
        }
        if(nRet) {
            qCritical(log) << "Load parameter fail" << nRet;
            DeleteOperate(pOperate);
            return nullptr;
        }
        pOperate->SetSettingsFile(szFile);
    }
    else
        qCritical(log) << "Don't create Operate:" << protocol;
    
    return pOperate;
}

int CManager::SaveOperate(COperate *pOperate)
{
    if(!pOperate) return -1;
    
    QString szFile = pOperate->GetSettingsFile();
    if(szFile.isEmpty())
        szFile = RabbitCommon::CDir::Instance()->GetDirUserData()
                 + QDir::separator()
                 + pOperate->Id()
                 + ".rrc";
    
    QSettings set(szFile, QSettings::IniFormat);
    
    CPlugin* pPlugin = nullptr; //pOperate->GetPlugin;
    bool bRet = QMetaObject::invokeMethod(
        pOperate,
        "GetPlugin",
        Qt::DirectConnection,
        Q_RETURN_ARG(CPlugin*, pPlugin));
    if(!bRet || !pPlugin)
    {
        qCritical(log) << "Get plugin client fail";
    }
    Q_ASSERT(pPlugin);

    set.setValue("Manage/FileVersion", m_FileVersion);
    set.setValue("Plugin/ID", pPlugin->Id());
    set.setValue("Plugin/Protocol", pPlugin->Protocol());
    set.setValue("Plugin/Name", pPlugin->Name());
    int nRet = 0;
    //nRet = pOperate->Save(szFile);
    bRet = QMetaObject::invokeMethod(
        pOperate,
        "Save",
        Qt::DirectConnection,
        Q_RETURN_ARG(int, nRet),
        Q_ARG(QString, szFile));
    if(!bRet) {
        qCritical(log) << "Call pOperate->Save(szFile) fail.";
        return -1;
    }
    if(nRet) {
        qCritical(log) << "Save parameter fail" << nRet;
        return -2;
    }
    return 0;
}

int CManager::LoadSettings(const QString szFile)
{
    if(!m_pParameter) {
        qCritical(log) << "The m_pParameter is nullptr";
        Q_ASSERT_X(m_pParameter, "CManager", "The m_pParameter is nullptr");
        return -1;
    }

    QString s = szFile;
    if(s.isEmpty())
        s = m_szSettingsFile;
    return m_pParameter->Load(s);
}

int CManager::SaveSettings(const QString szFile)
{
    if(!m_pParameter) {
        qCritical(log) << "The m_pParameter is nullptr";
        Q_ASSERT_X(m_pParameter, "CManager", "The m_pParameter is nullptr");
        return -1;
    }

    QString s = szFile;
    if(s.isEmpty())
        s = m_szSettingsFile;
    return m_pParameter->Save(s);
}

QList<QWidget*> CManager::GetSettingsWidgets(QWidget* parent)
{
    QList<QWidget*> lstWidget;
    
    CFrmParameterClient* pClient = new CFrmParameterClient(parent);
    if(pClient) {
        pClient->SetParameter(&m_pParameter->m_Client);
        lstWidget.push_back(pClient);
    }
    
    CParameterRecordUI* pRecord = new CParameterRecordUI(parent);
    if(pRecord) {
        pRecord->SetParameter(&m_pParameter->m_Client.m_Record);
        lstWidget.push_back(pRecord);
    }
    
    return lstWidget;
}

int CManager::EnumPlugins(Handle *handle)
{
    int nRet = 0;
    QMap<QString, CPlugin*>::iterator it;
    for(it = m_Plugins.begin(); it != m_Plugins.end(); it++)
    {
        nRet = handle->onProcess(it.key(), it.value());
        if(nRet)
            return nRet;
    }
    return nRet;
}

#if HAS_CPP_11
int CManager::EnumPlugins(std::function<int(const QString &, CPlugin *)> cb)
{
    int nRet = 0;
    QMap<QString, CPlugin*>::iterator it;
    for(it = m_Plugins.begin(); it != m_Plugins.end(); it++)
    {
        nRet = cb(it.key(), it.value());
        if(nRet)
            return nRet;
    }
    return nRet;
}
#endif

const QString CManager::Details() const
{
    return m_szDetails;
}
