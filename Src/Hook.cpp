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

CHook::CHook(CParameterClient *pParaClient, QObject *parent)
    : QObject(parent)
    , m_pParameterClient(pParaClient)
{
    qDebug(log) << "CHook::~CHook()";
}

CHook::~CHook()
{
    qDebug(log) << "CHook::~CHook()";
}

CHook* CHook::GetHook(CParameterClient *pParaClient, QObject *parent)
{
    CHook* p = nullptr;
#if defined(Q_OS_WIN)
    p = new CHookWindows(pParaClient, parent);
#elif defined(Q_OS_LINUX)
    p = new CHook(pParaClient, parent);
#endif
    return p;
}

int CHook::RegisterKeyboard()
{
    int nRet = 0;
#if defined(Q_OS_MACOS) || defined(Q_OS_ANDROID) || defined(Q_OS_MACOS)
    qApp->installEventFilter(this);
#elif defined(Q_OS_LINUX)
    if(!g_pNativeEventFilter)
        g_pNativeEventFilter = new CNativeEventFilterUnix(m_pParameterClient);
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

    if(m_pParameterClient)
        qApp->removeEventFilter(this);

    return nRet;
}

bool CHook::eventFilter(QObject *watched, QEvent *event)
{
    if(QEvent::KeyPress == event->type() || QEvent::KeyRelease == event->type())
    {
        if(!m_pParameterClient->GetNativeWindowReceiveKeyboard()) {
            
            bool bProcess = false;
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            int key = keyEvent->key();
            switch (key) {
            case Qt::Key_Meta:
            case Qt::Key_Alt:
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
                        return true;
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
                }
                return true;
            }
            
            if(bProcess)
                return true;
        }
    }
    return false;
}
