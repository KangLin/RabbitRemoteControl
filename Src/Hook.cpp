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
#endif

QAbstractNativeEventFilter* g_pNativeEventFilter = nullptr;
static Q_LOGGING_CATEGORY(log, "Client.Hook")

CHook::CHook(CParameterPlugin *pPara, QObject *parent)
    : QObject(parent)
    , m_pParameterPlugin(pPara)
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
    int nRet = 0;
#if defined(Q_OS_LINUX) && !defined(Q_OS_MACOS) && !defined(Q_OS_ANDROID)
    if(!g_pNativeEventFilter)
        g_pNativeEventFilter = new CNativeEventFilterUnix(m_pParameterPlugin);
    if(g_pNativeEventFilter)
        qApp->installNativeEventFilter(g_pNativeEventFilter);
#else
    qApp->installEventFilter(this);
#endif
    return nRet;
}

int CHook::UnRegisterKeyboard()
{
    int nRet = 0;
    if(g_pNativeEventFilter) {
        qApp->removeNativeEventFilter(g_pNativeEventFilter);
        delete g_pNativeEventFilter;
        g_pNativeEventFilter = nullptr;
    }

    if(m_pParameterPlugin)
        qApp->removeEventFilter(this);

    return nRet;
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
