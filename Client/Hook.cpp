// Author: Kang Lin <kl222@126.com>

#include "Hook.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(Client)

CHook::CHook(QObject *parent)
    : QObject(parent)
{}

CHook::~CHook()
{
    qDebug(Client) << "CHook::~CHook()";
}

#ifdef Q_OS_ANDROID
CHook* CHook::GetHook(QObject *parent)
{
    return nullptr;
}
#endif
