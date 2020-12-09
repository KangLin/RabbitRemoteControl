//! @author: Kang Lin(kl222@126.com)

#include "PluginFactory.h"

CPluginFactory::CPluginFactory(QObject *parent) : QObject(parent)
{
}

QIcon CPluginFactory::Icon()
{
    return QIcon(":/image/Connect");
}
