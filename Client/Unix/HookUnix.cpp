#include "HookUnix.h"

#include <QDebug>
#include <QApplication>
#include <QWidget>
#include <QAbstractEventDispatcher>
#include <QLoggingCategory>

CHookUnix::CHookUnix(CParameterClient *pParaClient, QObject *parent)
    : CHook(pParaClient, parent)
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
