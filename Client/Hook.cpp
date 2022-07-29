// Author: Kang Lin <kl222@126.com>

#include "Hook.h"
#if defined(Q_OS_WIN)
    #include "Windows/HookWindows.h"
#endif
#include <QDebug>

CHook::CHook(QObject *parent)
    : QObject(parent)
{}

CHook::~CHook()
{
    qDebug() << "CHook::~CHook()";
}

CHook* CHook::GetHook(QObject *parent)
{
#if defined(Q_OS_WIN)
    return new CHookWindows(parent);
#endif
    return nullptr;
}
