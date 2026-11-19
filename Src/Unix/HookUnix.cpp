// Author: Kang Lin <kl222@126.com>

#include "HookUnix.h"

#include <QDebug>
#include <QApplication>
#include <QWidget>
#include <QAbstractEventDispatcher>
#include <QLoggingCategory>
#include "ParameterPlugin.h"

CHookUnix::CHookUnix(CParameterPlugin *para, QObject *parent)
    : CHook(para, parent)
{
}

CHookUnix::~CHookUnix()
{
}

int CHookUnix::RegisterKeyboard()
{
    return 0;
}

int CHookUnix::UnRegisterKeyboard()
{
    return 0;
}
