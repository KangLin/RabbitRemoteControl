#include "HookUnix.h"

#include <QDebug>
#include <QApplication>
#include <QWidget>
#include <QAbstractEventDispatcher>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(Client)

CHook* CHook::GetHook(QObject *parent)
{
    CHookUnix* p = nullptr;
    if(!p)
        p = new CHookUnix(parent);
    return p;
}

CHookUnix::CHookUnix(QObject *parent)
    : CHook(parent),
    m_pNativeEventFilter(new CHookUnixNativeEventFilter())
{
    //TODO:
    //RegisterKeyboard();
}

CHookUnix::~CHookUnix()
{
    UnRegisterKeyboard();
}

int CHookUnix::RegisterKeyboard()
{
    if(!m_pNativeEventFilter)
    {
        Q_ASSERT(false);
        return -1;
    }
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    qApp->installNativeEventFilter(m_pNativeEventFilter);
#else
    if(QCoreApplication::eventDispatcher())
        QCoreApplication::eventDispatcher()->installNativeEventFilter(m_pNativeEventFilter);
#endif
    return 0;
}

int CHookUnix::UnRegisterKeyboard()
{
    if(!m_pNativeEventFilter) return -1;

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    qApp->removeNativeEventFilter(m_pNativeEventFilter);
#else
    if(QCoreApplication::eventDispatcher())
        QCoreApplication::eventDispatcher()->removeNativeEventFilter(m_pNativeEventFilter);
#endif
    return 0;
}
