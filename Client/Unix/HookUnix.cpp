#include "HookUnix.h"

#include <QDebug>
#include <QApplication>
#include <QWidget>
#include <QAbstractEventDispatcher>
#include <QLoggingCategory>

CHook* CHook::GetHook(QObject *parent)
{
    CHookUnix* p = nullptr;
    if(!p)
        p = new CHookUnix(parent);
    return p;
}

CHookUnix::CHookUnix(QObject *parent)
    : CHook(parent)
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
    return 0;
}

int CHookUnix::UnRegisterKeyboard()
{
    return 0;
}
