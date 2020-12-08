#include "PluginFactory.h"

CPluginFactory::CPluginFactory(QObject *parent) : QObject(parent)
{
}

QIcon CPluginFactory::Icon()
{
    return QIcon(":/image/Connect");
}
