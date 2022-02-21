// Author: Kang Lin <kl222@126.com>

#include "PluginServiceTigerVNC.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonLog.h"
#include <QDebug>
#include <QApplication>
#include <rfb/LogWriter.h>
#include <rfb/Logger_stdio.h>
#include "ServiceTigerVNC.h"

CPluginServiceTigerVNC::CPluginServiceTigerVNC(QObject *parent) : CPluginService(parent)
{
    //rfb::SecurityClient::setDefaults();
    
    rfb::initStdIOLoggers();
#ifdef WIN32
    rfb::initFileLogger("C:\\temp\\vncviewer.log");
#else
    rfb::initFileLogger("/tmp/vncviewer.log");
#endif
    rfb::LogWriter::setLogParams("*:stderr:100");
    
}

CPluginServiceTigerVNC::~CPluginServiceTigerVNC()
{
   qDebug() << "CManageConnectTigerVnc::~CManageConnectTigerVnc()";
}

const QString CPluginServiceTigerVNC::Protol() const
{
    return "RFB";
}

const QString CPluginServiceTigerVNC::Name() const
{
    return "TigerVNC";
}

const QString CPluginServiceTigerVNC::DisplayName() const
{
    return tr("TigerVNC");
}

const QString CPluginServiceTigerVNC::Description() const
{
    return tr("VNC(Virtual Network Console): Access remote desktops such as unix/linux, windows, etc.") + "\n"
            + tr("VNC is a set of programs using the RFB (Remote Frame Buffer) protocol.");
}

CService *CPluginServiceTigerVNC::NewService()
{
    return new CServiceTigerVNC(this);
}
