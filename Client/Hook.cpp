// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QApplication>
#include "Hook.h"
#if defined(Q_OS_WIN)
    #include "HookWindows.h"
#endif
#include "NativeEventFilter.h"

CNativeEventFilter* g_pNativeEventFilter = nullptr;
static Q_LOGGING_CATEGORY(log, "Client.Hook")

CHook::CHook(CParameterClient *pParaClient, QObject *parent)
    : QObject(parent)
    , m_pParameterClient(pParaClient)
{}

CHook::~CHook()
{
    qDebug(log) << "CHook::~CHook()";
}

CHook* CHook::GetHook(CParameterClient *pParaClient, QObject *parent)
{
    CHook* p = nullptr;
#if defined(Q_OS_WIN)
    p = new CHookWindows(pParaClient, parent);
#else
    p = new CHook(pParaClient, parent);
#endif
    return p;
}

int CHook::RegisterKeyboard()
{
    int nRet = 0;
    if(!g_pNativeEventFilter)
        g_pNativeEventFilter = new CNativeEventFilter(m_pParameterClient);
    if(g_pNativeEventFilter)
        qApp->installNativeEventFilter(g_pNativeEventFilter);
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
    return nRet;
}
