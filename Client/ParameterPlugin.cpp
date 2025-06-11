#include "ParameterPlugin.h"

CParameterPlugin::CParameterPlugin(QObject *parent, const QString &szPrefix)
    : CParameter{parent, szPrefix}
    , m_Client(this)
{}


int CParameterPlugin::OnLoad(QSettings &set)
{
    return 0;
}

int CParameterPlugin::OnSave(QSettings &set)
{
    return 0;
}
