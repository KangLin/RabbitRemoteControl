// Author: Kang Lin <kl222@126.com>

#include "Client.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonTools.h"
#include "FrmParameterClient.h"
#include "FrmViewer.h"

#include <QPluginLoader>
#include <QDebug>
#include <QtPlugin>
#include <QDataStream>
#include <QFile>
#include <QApplication>
#include <QSettings>
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Client")

CClient::CClient(QObject *parent) : QObject(parent),
    m_FileVersion(1)  //TODO: update version it if update data
{
    bool check = false;
//#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
//    Q_INIT_RESOURCE(translations_Client);
//#endif

    qApp->installEventFilter(this);

    m_Translator = RabbitCommon::CTools::Instance()->InstallTranslator(
        "Client",
        RabbitCommon::CTools::TranslationType::Library);

    LoadPlugins();

    check = connect(&m_ParameterClient, SIGNAL(sigHookKeyboardChanged()),
                    this, SLOT(slotHookKeyboardChanged()));
    Q_ASSERT(check);
    // TODO: Disable it ?
    if(m_ParameterClient.GetHookKeyboard())
        m_Hook = QSharedPointer<CHook>(CHook::GetHook());
}

CClient::~CClient()
{
    qDebug(log) << "CClient::~CClient()";
    qApp->installEventFilter(nullptr);
    RabbitCommon::CTools::Instance()->RemoveTranslator(m_Translator);

//#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
//    Q_CLEANUP_RESOURCE(translations_Client);
//#endif
}

int CClient::LoadPlugins()
{
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
    return FindPlugins(szPath, filters);
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
                                  << p->Name() << ":" << szPlugins;
                    AppendPlugin(p);
                }
                else
                    qWarning(log) << "The plugin [" << p->Name() << "] is exist.";
            }
        }else{
            QString szMsg;
            szMsg = "load plugin error: " + loader.errorString();
            qCritical(log) << szMsg;
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
    p->InitTranslator();
    return 0;
}

//! [CClient CreateConnecter]
CConnecter* CClient::CreateConnecter(const QString& id)
{
    auto it = m_Plugins.find(id);
    if(m_Plugins.end() != it)
    {
        qDebug(log) << "CreateConnecter id:" << id;
        CConnecter* p = it.value()->CreateConnecter(id);
        //if(p) p->SetParameterClient(&m_ParameterClient);
        if(p) {
            int val = 0;
            bool bRet = QMetaObject::invokeMethod(
                p,
                "SetParameterClient",
                Qt::DirectConnection,
                Q_RETURN_ARG(int, val),
                Q_ARG(CParameterClient*, &m_ParameterClient));
            if(!bRet|| val)
                return nullptr;
        }
        return p;
    }
    return nullptr;
}
//! [CClient CreateConnecter]

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
    if(pConnecter)
        pConnecter->Load(szFile);
    else
        qCritical(log) << "Don't create connecter:" << protocol;

    return pConnecter;
}

int CClient::SaveConnecter(QString szFile, CConnecter *pConnecter)
{
    if(!pConnecter) return -1;

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
    pConnecter->Save(szFile);

    return 0;
}

int CClient::LoadSettings(QString szFile)
{
    return m_ParameterClient.CParameter::Load(szFile);
}

int CClient::SaveSettings(QString szFile)
{
    return m_ParameterClient.CParameter::Save(szFile);
}

QList<QWidget*> CClient::GetSettingsWidgets(QWidget* parent)
{
    QList<QWidget*> lstWidget;

    QWidget* p = new CFrmParameterClient(&m_ParameterClient, parent);
    if(p) {
        int idx = p->metaObject()->indexOfSlot("slotAccept()");
        if(-1 == idx) {
            QString szErr = "Class ";
            szErr += p->metaObject()->className();
            szErr += " hasn't slot slotAccept(), please add it.";
            Q_ASSERT_X(false, "Client", szErr.toStdString().c_str());
        }

        lstWidget.push_back(p);
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

void CClient::slotHookKeyboardChanged()
{
    if(m_ParameterClient.GetHookKeyboard())
    {
        m_Hook = QSharedPointer<CHook>(CHook::GetHook());
    } else {
        if(m_Hook)
            m_Hook.reset();
    }
}

bool CClient::eventFilter(QObject *watched, QEvent *event)
{
    if(QEvent::KeyPress == event->type() || QEvent::KeyRelease == event->type())
    {
        //qDebug(log) << "eventFilter:" << event;
        bool bProcess = false;
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        int key = keyEvent->key();
        switch (key) {
        case Qt::Key_Meta:
#if defined(Q_OS_WIN)
            key = Qt::Key_Super_L;
#endif
#if defined(Q_OS_MACOS)
            key = Qt::Key_Control;
#endif
            bProcess = true;
            break;
        case Qt::Key_Tab:
        case Qt::Key_Alt:
            bProcess = true;
            break;
        default:
            break;
        }

        if(bProcess) {
            switch(event->type()) {
            case QEvent::KeyPress:
            {
                CFrmViewer* focus = qobject_cast<CFrmViewer*>(QApplication::focusWidget());
                if(focus) {
                    emit focus->sigKeyPressEvent(key, keyEvent->modifiers());
                    return true;
                }
            }
            case QEvent::KeyRelease:
            {
                CFrmViewer* focus = qobject_cast<CFrmViewer*>(QApplication::focusWidget());
                if(focus) {
                    emit focus->sigKeyReleaseEvent(key, keyEvent->modifiers());
                    return true;
                }
            }
            default:
                break;
            }
        }
    }
    return QObject::eventFilter(watched, event);
}
