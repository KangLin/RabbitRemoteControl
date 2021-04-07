//! @author: Kang Lin (kl222@126.com)

#include "PluginFactory.h"

CPluginFactory::CPluginFactory(QObject *parent) : QObject(parent)
{
}

CPluginFactory::~CPluginFactory()
{}

const QString CPluginFactory::Id() const
{
    return Protol() + ":" + Name();
}

const QIcon CPluginFactory::Icon() const
{
    return QIcon(":/image/Connect");
}
