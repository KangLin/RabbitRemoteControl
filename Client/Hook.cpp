// Author: Kang Lin <kl222@126.com>

#include "Hook.h"

#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Client.Hook")

CHook::CHook(QObject *parent)
    : QObject(parent)
{}

CHook::~CHook()
{
    qDebug(log) << "CHook::~CHook()";
}

#ifdef Q_OS_ANDROID
CHook* CHook::GetHook(QObject *parent)
{
    return nullptr;
}
#endif
