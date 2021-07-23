// Author: Kang Lin <kl222@126.com>

#include "Plugin.h"

CPlugin::CPlugin(QObject *parent) : QObject(parent)
{
}

CPlugin::~CPlugin()
{}

const QString CPlugin::Id() const
{
    return Protol() + ":" + Name();
}

const QIcon CPlugin::Icon() const
{
    return QIcon(":/image/Connect");
}
