// Author: Kang Lin <kl222@126.com>

#include "PluginViewer.h"

CPluginViewer::CPluginViewer(QObject *parent) : QObject(parent)
{
}

CPluginViewer::~CPluginViewer()
{}

const QString CPluginViewer::Id() const
{
    return Protol() + ":" + Name();
}

const QIcon CPluginViewer::Icon() const
{
    return QIcon(":/image/Connect");
}
