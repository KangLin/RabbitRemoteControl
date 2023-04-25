// Author: Kang Lin <kl222@126.com>

#include "PluginServiceRabbitVNC.h"
#include "RabbitCommonDir.h"
#include <QDebug>
#include <QApplication>
#include <rfb/LogWriter.h>
#include <rfb/Logger_stdio.h>
#include "ServiceRabbitVNC.h"
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(RabbitVNC, "RabbitVNC")

CPluginServiceRabbitVNC::CPluginServiceRabbitVNC(QObject *parent)
    : CPluginServiceThread(parent)
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

CPluginServiceRabbitVNC::~CPluginServiceRabbitVNC()
{
   qDebug(RabbitVNC) << "CPluginServiceRabbitVNC::~CPluginServiceRabbitVNC()";
}

const QString CPluginServiceRabbitVNC::Protocol() const
{
    return "RFB";
}

const QString CPluginServiceRabbitVNC::Name() const
{
    return "RabbitVNC";
}

const QString CPluginServiceRabbitVNC::DisplayName() const
{
    return tr("RabbitVNC");
}

const QString CPluginServiceRabbitVNC::Description() const
{
    return tr("VNC(Virtual Network Console): Access remote desktops such as unix/linux, windows, etc.") + "\n"
            + tr("VNC is a set of programs using the RFB (Remote Frame Buffer) protocol.");
}

CService *CPluginServiceRabbitVNC::NewService()
{
    return new CServiceRabbitVNC(this);
}
