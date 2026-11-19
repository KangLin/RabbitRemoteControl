// Author: Kang Lin <kl222@126.com>

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
    #include "Unix/DesktopManagerUnix.h"
#endif
#include "RabbitCommonTools.h"

QAbstractNativeEventFilter* g_pNativeEventFilter = nullptr;
static Q_LOGGING_CATEGORY(log, "Plugin.Hook")

CHook::CHook(CParameterPlugin *pPara, QObject *parent)
    : QObject(parent)
    , m_pParameterPlugin(pPara)
    , m_pDesktopManagerThread(nullptr)
{
    qDebug(log) << "CHook::CHook()";

    m_pDesktopManagerThread = new CDesktopManagerThread(pPara);
    if(m_pDesktopManagerThread) {
        m_pDesktopManagerThread->start();
        while(!m_pDesktopManagerThread->IsStart())
            QApplication::processEvents();
    }
}

CHook::~CHook()
{
    qDebug(log) << "CHook::~CHook()";
    if(m_pDesktopManagerThread) {
        m_pDesktopManagerThread->quit();
        int n = 0;
        while(!m_pDesktopManagerThread->isFinished() && 10 >= n++)
            QThread::msleep(500);
    }
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
    OnDisableDesktopShortcuts();
    return 0;
}

int CHook::UnRegisterKeyboard()
{
    OnRestoreDesktopShortcuts();
    OnUnRegisterKeyboard();
    return 0;
}

int CHook::OnRegisterKeyboard()
{
#if defined(Q_OS_LINUX) && !defined(Q_OS_MACOS) && !defined(Q_OS_ANDROID)
    if(-1 == QGuiApplication::platformName().indexOf(QRegularExpression("wayland.*"))) {
        if(!g_pNativeEventFilter)
            g_pNativeEventFilter = new CNativeEventFilterUnix(m_pParameterPlugin);
        if(g_pNativeEventFilter)
            qApp->installNativeEventFilter(g_pNativeEventFilter);
    } else
        qApp->installEventFilter(this);
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
    qDebug(log) << Q_FUNC_INFO;
    if(m_pDesktopManagerThread)
        emit m_pDesktopManagerThread->sigDisableShortcuts();
    return 0;
}

int CHook::OnRestoreDesktopShortcuts()
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_pDesktopManagerThread)
        emit m_pDesktopManagerThread->sigRestoreShortcuts();
    return 0;
}

bool CHook::eventFilter(QObject *watched, QEvent *event)
{
    bool bRet = false;
    if(QEvent::KeyPress == event->type() || QEvent::KeyRelease == event->type())
    {
        if(m_pParameterPlugin && m_pParameterPlugin->GetCaptureAllKeyboard()) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

            CFrmViewer* focus = qobject_cast<CFrmViewer*>(QApplication::focusWidget());
            qDebug(log) << "eventFilter:" << keyEvent
                        << watched << focus;
            if(focus) {
                if(focus == watched) {
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
                    bRet = true;
                    break;
                case QKeyEvent::KeyRelease:
                    emit focus->sigKeyReleaseEvent(keyEvent);
                    bRet = true;
                    break;
                default:
                    break;
                }
            }
        }
    }
    return bRet;
}
