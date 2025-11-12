// Author: Kang Lin <kl222@126.com>

#include <QDesktopServices>
#include <QLoggingCategory>
#include <QApplication>
#include <QKeyEvent>
#include "Hook.h"
#if defined(Q_OS_WIN)
    #include "Windows/HookWindows.h"
#elif defined(Q_OS_ANDROID)
#elif defined(Q_OS_MACOS)
#elif defined(Q_OS_LINUX)
    #include "Unix/NativeEventFilterUnix.h"
#endif
#include "RabbitCommonTools.h"

QAbstractNativeEventFilter* g_pNativeEventFilter = nullptr;
static Q_LOGGING_CATEGORY(log, "Plugin.Hook")

CHook::CHook(CParameterPlugin *pPara, QObject *parent)
    : QObject(parent)
    , m_pParameterPlugin(pPara)
    , m_bScript(false)
{
    qDebug(log) << "CHook::~CHook()";
}

CHook::~CHook()
{
    qDebug(log) << "CHook::~CHook()";
}

CHook* CHook::GetHook(CParameterPlugin *pPara, QObject *parent)
{
    CHook* p = nullptr;
#if defined(Q_OS_WIN)
    p = new CHookWindows(pPara, parent);
#elif defined(Q_OS_LINUX)
    p = new CHook(pPara, parent);
#endif
    return p;
}

int CHook::RegisterKeyboard()
{
    OnRegisterKeyboard();

    if(m_pParameterPlugin->GetDesktopShortcutsScript()
        && !m_pParameterPlugin->GetRestoreDesktopShortcutsScript().isEmpty()
        && !m_pParameterPlugin->GetDisableDesktopShortcutsScript().isEmpty()) {
#if defined(Q_OS_WIN32)
        QDesktopServices::openUrl(m_pParameterPlugin->GetDisableDesktopShortcutsScript());
#else
        RunCommand(m_pParameterPlugin->GetDisableDesktopShortcutsScript());
#endif
        m_bScript = true;
    } else {
        OnDisableDesktopShortcuts();
        m_bScript = false;
    }
    return 0;
}

int CHook::UnRegisterKeyboard()
{
    if(m_bScript) {
#if defined(Q_OS_WIN32)
        QDesktopServices::openUrl(m_pParameterPlugin->GetRestoreDesktopShortcutsScript());
#else
        RunCommand(m_pParameterPlugin->GetRestoreDesktopShortcutsScript());
#endif
    } else {
        OnRestoreDesktopShortcuts();
    }

    OnUnRegisterKeyboard();
    return 0;
}

int CHook::OnRegisterKeyboard()
{
#if defined(Q_OS_LINUX) && !defined(Q_OS_MACOS) && !defined(Q_OS_ANDROID)
    if(!g_pNativeEventFilter)
        g_pNativeEventFilter = new CNativeEventFilterUnix(m_pParameterPlugin);
    if(g_pNativeEventFilter)
        qApp->installNativeEventFilter(g_pNativeEventFilter);
#else
    qApp->installEventFilter(this);
#endif
    return 0;
}

int CHook::OnUnRegisterKeyboard()
{
    if(g_pNativeEventFilter) {
        qApp->removeNativeEventFilter(g_pNativeEventFilter);
        delete g_pNativeEventFilter;
        g_pNativeEventFilter = nullptr;
    }

    if(m_pParameterPlugin)
        qApp->removeEventFilter(this);

    return 0;
}

int CHook::OnDisableDesktopShortcuts()
{
#if defined(Q_OS_LINUX) && !defined(Q_OS_MACOS) && !defined(Q_OS_ANDROID)
    m_DesktopShortcutsManager.disableAllShortcuts();
#endif
    return 0;
}

int CHook::OnRestoreDesktopShortcuts()
{
#if defined(Q_OS_LINUX) && !defined(Q_OS_MACOS) && !defined(Q_OS_ANDROID)
    m_DesktopShortcutsManager.restoreAllShortcuts();
#endif
    return 0;
}

bool CHook::eventFilter(QObject *watched, QEvent *event)
{
    if(QEvent::KeyPress == event->type() || QEvent::KeyRelease == event->type())
    {
        if(m_pParameterPlugin && m_pParameterPlugin->GetCaptureAllKeyboard()) {
            
            bool bProcess = false;
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            int key = keyEvent->key();
            switch (key) {
            case Qt::Key_Meta:
            case Qt::Key_Alt:
            case Qt::Key_Super_L:
            case Qt::Key_Super_R:
                bProcess = true;
                break;
            default:
                bProcess = false;
                break;
            }
            
            CFrmViewer* focus = qobject_cast<CFrmViewer*>(QApplication::focusWidget());
            qDebug(log) << "eventFilter:" << keyEvent
                        << watched << focus << bProcess;
            if(focus) {
                if(focus == watched) {
                    if(bProcess)
                        return false;
                }

                /*
                QKeyEvent* ke = new QKeyEvent(
                    keyEvent->type(), keyEvent->key(),
                    keyEvent->modifiers(), keyEvent->text());
                QApplication::postEvent(focus, ke);
                return true;//*/
                switch(keyEvent->type())
                {
                case QKeyEvent::KeyPress:
                    emit focus->sigKeyPressEvent(keyEvent);
                    break;
                case QKeyEvent::KeyRelease:
                    emit focus->sigKeyReleaseEvent(keyEvent);
                    break;
                default:
                    break;
                }
                return true;
            }
        }
    }
    return false;
}

bool CHook::RunCommand(const QString &program, const QStringList &args, int timeout)
{
    QProcess process;
    process.setProgram(program);
    if(!args.isEmpty())
        process.setArguments(args);
    
    //qDebug(log) << "Command:" << program << args;
    
    process.start();
    
    if (!process.waitForFinished(timeout)) {
        qWarning(log) << "Command timeout:" << program << args;
        return false;
    }
    
    if (process.exitCode() != 0) {
        QString errorOutput = process.readAllStandardError();
        qWarning(log) << "Command failed:" << program << args 
                      << "exit code:" << process.exitCode()
                      << "failed:" << errorOutput;
        return false;
    }
    
    QString standardOutput = process.readAllStandardOutput();
    if (!standardOutput.isEmpty()) {
        qDebug(log) << "Command output:" << standardOutput.trimmed();
    }
    
    return true;
}
