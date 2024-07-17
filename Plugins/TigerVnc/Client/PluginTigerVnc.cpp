// Author: Kang Lin <kl222@126.com>

#include "PluginTigerVnc.h"
#include "ConnecterTigerVnc.h"
#include <rfb/LogWriter.h>
#include <rfb/Logger_stdio.h>
#include <QDebug>
#include <QApplication>
#include <QLoggingCategory>
#include "RabbitCommonDir.h"

static Q_LOGGING_CATEGORY(log, "VNC.Tiger.Plugin")

static bool initlog = false;
CPluginTigerVnc::CPluginTigerVnc(QObject *parent)
    : CPluginClient(parent)
{
    //! [Initialize resource]

    //rfb::SecurityClient::setDefaults();
    if(!initlog)
    {
        rfb::initStdIOLoggers();
        QString szFile = RabbitCommon::CDir::Instance()->GetDirLog()
                + QDir::separator()
                + "TigerVnc.log";
        rfb::initFileLogger(szFile.toStdString().c_str());
        rfb::LogWriter::setLogParams("*:stderr:100");
        initlog = true;
    }
    //! [Initialize resource]
}

CPluginTigerVnc::~CPluginTigerVnc()
{
    //! [Clean resource]
    qDebug(log) << "CPluginTigerVnc::~CPluginTigerVnc()";
    //! [Clean resource]
}

const QString CPluginTigerVnc::Name() const
{
    return tr("TigerVNC");
}

const QString CPluginTigerVnc::Description() const
{
    return tr("VNC(Virtual Network Console): Access remote desktops such as unix/linux, windows, etc.") + "\n"
            + tr("VNC is a set of programs using the RFB (Remote Frame Buffer) protocol.");
}

const QString CPluginTigerVnc::Protocol() const
{
    return "RFB";
}

CConnecter *CPluginTigerVnc::CreateConnecter(const QString &szProtocol)
{
    if(Id() == szProtocol)
    {
        return new CConnecterTigerVnc(this);
    }
    return nullptr;
}

const QIcon CPluginTigerVnc::Icon() const
{
    return QIcon::fromTheme("network-wired");
}
