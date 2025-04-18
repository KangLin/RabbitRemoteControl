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

#include "Client.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonTools.h"
#include "FrmParameterClient.h"
#include "FrmViewer.h"
#include "Channel.h"
#include "ParameterRecordUI.h"

static Q_LOGGING_CATEGORY(log, "Client")

CClient::CClient(QObject *parent, QString szFile) : QObject(parent)
    , m_FileVersion(1)  //TODO: update version it if update data
    , m_pHook(nullptr)
{
    bool check = false;
//#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
//    Q_INIT_RESOURCE(translations_Client);
//#endif

    m_Translator = RabbitCommon::CTools::Instance()->InstallTranslator(
        "Client", RabbitCommon::CTools::TranslationType::Library);

    CChannel::InitTranslation();
    m_szSettingsFile = szFile;
    m_pParameterClient = new CParameterClient();
    if(m_pParameterClient) {
        LoadSettings(m_szSettingsFile);
        check = connect(m_pParameterClient, &CParameterClient::sigNativeWindowRecieveKeyboard,
                        this, [&](){
            if(m_pParameterClient->GetNativeWindowReceiveKeyboard()) {
                if(m_pHook) {
                    m_pHook->UnRegisterKeyboard();
                    m_pHook->deleteLater();
                    m_pHook = nullptr;
                }
            } else {
                m_pHook = CHook::GetHook(m_pParameterClient, this);
                if(m_pHook)
                    m_pHook->RegisterKeyboard();
            }
        });
        m_pHook = CHook::GetHook(m_pParameterClient, this);
        if(m_pHook)
            m_pHook->RegisterKeyboard();
    } else {
        qCritical(log) << "new CParameterClient() fail";
        Q_ASSERT(m_pParameterClient);
    }

    LoadPlugins();
}

CClient::~CClient()
{
    qDebug(log) << "CClient::~CClient()";
    
    qApp->removeEventFilter(this);
    
    if(m_pHook) {
        m_pHook->UnRegisterKeyboard();
        m_pHook->deleteLater();
        m_pHook = nullptr;
    }

    if(m_pParameterClient) {
        m_pParameterClient->deleteLater();
        m_pParameterClient = nullptr;
    }

    if(m_Translator)
        RabbitCommon::CTools::Instance()->RemoveTranslator(m_Translator);

    CChannel::RemoveTranslation();

//#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
//    Q_CLEANUP_RESOURCE(translations_Client);
//#endif
}

int CClient::LoadPlugins()
{
    int nRet = 0;
    foreach (QObject *plugin, QPluginLoader::staticInstances())
    {
        CPluginClient* p = qobject_cast<CPluginClient*>(plugin);
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
#if !defined (Q_OS_ANDROID)
    szPath = szPath + QDir::separator() + "Client";
#endif

    QStringList filters;
#if defined (Q_OS_WINDOWS) || defined(Q_OS_WIN)
    filters << "*PluginClient*.dll";
#else
    filters << "*PluginClient*.so";
#endif
    nRet = FindPlugins(szPath, filters);
    if(!m_szDetails.isEmpty())
        m_szDetails = tr("### Plugins") + "\n" + m_szDetails;

    qDebug(log) << ("Client details:\n" + Details()).toStdString().c_str();
    return nRet;
}

int CClient::FindPlugins(QDir dir, QStringList filters)
{
    QString fileName;
    if(filters.isEmpty())
    {
#if defined (Q_OS_WINDOWS) || defined(Q_OS_WIN)
        filters << "*PluginClient*.dll";
#else
        filters << "*PluginClient*.so";
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
            CPluginClient* p = qobject_cast<CPluginClient*>(plugin);
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

int CClient::AppendPlugin(CPluginClient *p)
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

//! [CClient CreateConnecter]
CConnecter* CClient::CreateConnecter(const QString& id)
{
    CConnecter* pConnecter = nullptr;
    auto it = m_Plugins.find(id);
    if(m_Plugins.end() != it)
    {
        bool bRet = 0;
        qDebug(log) << "CreateConnecter id:" << id;
        auto plugin = it.value();
        if(plugin) {
            //p = plugin->CreateConnecter(id);
            bRet = QMetaObject::invokeMethod(
                plugin,
                "CreateConnecter",
                Qt::DirectConnection,
                Q_RETURN_ARG(CConnecter*, pConnecter),
                Q_ARG(QString, id),
                Q_ARG(CParameterClient*, m_pParameterClient));
            if(!bRet) {
                qCritical(log) << "Create CConnecter fail.";
                return nullptr;
            }
        }
    }
    return pConnecter;
}
//! [CClient CreateConnecter]

int CClient::DeleteConnecter(CConnecter *p)
{
    qDebug(log) << Q_FUNC_INFO;
    if(!p) return 0;

    CPluginClient* pPluginClient = nullptr;
    //pPluginClient->GetPlugClient();
    bool bRet = QMetaObject::invokeMethod(
        p,
        "GetPlugClient",
        Qt::DirectConnection,
        Q_RETURN_ARG(CPluginClient*, pPluginClient));

    if(bRet && pPluginClient) {
        int nRet = 0;
        //pPluginClient->DeleteConnecter(p);
        bRet = QMetaObject::invokeMethod(
            pPluginClient,
            "DeleteConnecter",
            Qt::DirectConnection,
            Q_RETURN_ARG(int, nRet),
            Q_ARG(CConnecter*, p));
        if(!bRet) {
            nRet = -1;
            qCritical(log) << "Call pPluginClient->DeleteConnecter(p) fail";
        }
        return nRet;
    }

    qCritical(log) << "Get CClient fail.";
    return -1;
}

CConnecter* CClient::LoadConnecter(const QString &szFile)
{
    CConnecter* pConnecter = nullptr;
    if(szFile.isEmpty()) return nullptr;

    QSettings set(szFile, QSettings::IniFormat);
    m_FileVersion = set.value("Manage/FileVersion", m_FileVersion).toInt();
    QString id = set.value("Plugin/ID").toString();
    QString protocol = set.value("Plugin/Protocol").toString();
    QString name = set.value("Plugin/Name").toString();
    Q_UNUSED(name);
    qDebug(log) << "LoadConnecter protocol:" << protocol
                << "name:" << name << "id:" << id;
    pConnecter = CreateConnecter(id);
    if(pConnecter) {
        int nRet = false;
        //bRet = pConnecter->Load(szFile);
        bool bRet = QMetaObject::invokeMethod(
            pConnecter,
            "Load",
            Qt::DirectConnection,
            Q_RETURN_ARG(int, nRet),
            Q_ARG(QString, szFile));
        if(!bRet) {
            qCritical(log) << "Call pConnecter->Load(szFile) fail.";
            return nullptr;
        }
        if(nRet) {
            qCritical(log) << "Load parameter fail" << nRet;
            DeleteConnecter(pConnecter);
            return nullptr;
        }
        pConnecter->SetSettingsFile(szFile);
    }
    else
        qCritical(log) << "Don't create connecter:" << protocol;

    return pConnecter;
}

int CClient::SaveConnecter(CConnecter *pConnecter)
{
    if(!pConnecter) return -1;

    QString szFile = pConnecter->GetSettingsFile();
    if(szFile.isEmpty())
        szFile = RabbitCommon::CDir::Instance()->GetDirUserData()
                + QDir::separator()
                + pConnecter->Id()
                + ".rrc";

    QSettings set(szFile, QSettings::IniFormat);

    CPluginClient* pPluginClient = nullptr; //pConnecter->m_pPluginClient;
    bool bRet = QMetaObject::invokeMethod(
        pConnecter,
        "GetPlugClient",
        Qt::DirectConnection,
        Q_RETURN_ARG(CPluginClient*, pPluginClient));
    if(!bRet || !pPluginClient)
    {
        qCritical(log) << "Get plugin client fail";
    }
    Q_ASSERT(pPluginClient);

    set.setValue("Manage/FileVersion", m_FileVersion);
    set.setValue("Plugin/ID", pPluginClient->Id());
    set.setValue("Plugin/Protocol", pPluginClient->Protocol());
    set.setValue("Plugin/Name", pPluginClient->Name());
    int nRet = 0;
    //nRet = pConnecter->Save(szFile);
    bRet = QMetaObject::invokeMethod(
        pConnecter,
        "Save",
        Qt::DirectConnection,
        Q_RETURN_ARG(int, nRet),
        Q_ARG(QString, szFile));
    if(!bRet) {
        qCritical(log) << "Call pConnecter->Save(szFile) fail.";
        return -1;
    }
    if(nRet) {
        qCritical(log) << "Save parameter fail" << nRet;
        return -2;
    }
    return 0;
}

int CClient::LoadSettings(const QString szFile)
{
    if(!m_pParameterClient) {
        qCritical(log) << "The m_pParameterClient is nullptr";
        Q_ASSERT_X(m_pParameterClient, "CClient", "The m_pParameterClient is nullptr");
        return -1;
    }

    QString s = szFile;
    if(s.isEmpty())
        s = m_szSettingsFile;
    return m_pParameterClient->CParameter::Load(s);
}

int CClient::SaveSettings(const QString szFile)
{
    if(!m_pParameterClient) {
        qCritical(log) << "The m_pParameterClient is nullptr";
        Q_ASSERT_X(m_pParameterClient, "CClient", "The m_pParameterClient is nullptr");
        return -1;
    }

    QString s = szFile;
    if(s.isEmpty())
        s = m_szSettingsFile;
    return m_pParameterClient->CParameter::Save(s);
}

QList<QWidget*> CClient::GetSettingsWidgets(QWidget* parent)
{
    QList<QWidget*> lstWidget;

    CFrmParameterClient* pClient = new CFrmParameterClient(parent);
    if(pClient) {
        pClient->SetParameter(m_pParameterClient);
        lstWidget.push_back(pClient);
    }

    CParameterRecordUI* pRecord = new CParameterRecordUI(parent);
    if(pRecord) {
        pRecord->SetParameter(&m_pParameterClient->m_Record);
        lstWidget.push_back(pRecord);
    }

    return lstWidget;
}

int CClient::EnumPlugins(Handle *handle)
{
    int nRet = 0;
    QMap<QString, CPluginClient*>::iterator it;
    for(it = m_Plugins.begin(); it != m_Plugins.end(); it++)
    {
        nRet = handle->onProcess(it.key(), it.value());
        if(nRet)
            return nRet;
    }
    return nRet;
}

#if HAS_CPP_11
int CClient::EnumPlugins(std::function<int(const QString &, CPluginClient *)> cb)
{
    int nRet = 0;
    QMap<QString, CPluginClient*>::iterator it;
    for(it = m_Plugins.begin(); it != m_Plugins.end(); it++)
    {
        nRet = cb(it.key(), it.value());
        if(nRet)
            return nRet;
    }
    return nRet;
}
#endif

const QString CClient::Details() const
{
    return m_szDetails;
}
/*
void CClient::slotHookKeyboardChanged()
{
    if(m_pParameterClient->GetHookKeyboard())
    {
        if(!RabbitCommon::CTools::Instance()->HasAdministratorPrivilege()
            && m_pParameterClient->GetPromptAdministratorPrivilege())
        {
            int nRet = 0;
            QMessageBox msg(
                QMessageBox::Warning, tr("Warning"),
                tr("The programe is not administrator privilege.\n"
                   "Don't disable system shortcuts(eg: Ctrl+Alt+del).\n"
                   "Restart program by administrator?"),
                QMessageBox::Yes | QMessageBox::No);
            msg.setCheckBox(new QCheckBox(tr("Always shown"), &msg));
            msg.checkBox()->setCheckable(true);
            msg.checkBox()->setChecked(
                m_pParameterClient->GetPromptAdministratorPrivilege());
            nRet = msg.exec();
            if(QMessageBox::Yes == nRet) {
                RabbitCommon::CTools::Instance()->StartWithAdministratorPrivilege(true);
            }
            if(m_pParameterClient->GetPromptAdministratorPrivilege()
                != msg.checkBox()->isChecked()) {
                m_pParameterClient->SetPromptAdministratorPrivilege(
                    msg.checkBox()->isChecked());
                SaveSettings();
            }
        }
        m_pHook = CHook::GetHook(m_pParameterClient);
    } else {
        if(m_pHook) {
            m_pHook->deleteLater();
            m_pHook = nullptr;
        }
    }
}
*/
