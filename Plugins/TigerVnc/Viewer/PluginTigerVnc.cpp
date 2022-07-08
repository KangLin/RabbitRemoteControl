// Author: Kang Lin <kl222@126.com>

#include "PluginTigerVnc.h"
#include "ConnecterTigerVnc.h"
#include <rfb/LogWriter.h>
#include <rfb/Logger_stdio.h>
#include <QDebug>
#include <QApplication>
#include "RabbitCommonDir.h"

static bool initlog = false;
CPluginTigerVnc::CPluginTigerVnc(QObject *parent)
    : CPluginViewerThread(parent)
{
    //! [Initialize resource]

    //rfb::SecurityClient::setDefaults();
    if(!initlog)
    {
        rfb::initStdIOLoggers();
        QString szFile = RabbitCommon::CDir::Instance()->GetDirApplication()
                + QDir::separator()
                + "vncviewer.log";
        rfb::initFileLogger(szFile.toStdString().c_str());
        rfb::LogWriter::setLogParams("*:stderr:100");
        initlog = true;
    }
    //! [Initialize resource]
}

CPluginTigerVnc::~CPluginTigerVnc()
{
    //! [Clean resource]
    qDebug() << "CPluginTigerVnc::~CPluginTigerVnc()";
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

const QString CPluginTigerVnc::Protol() const
{
    return "RFB";
}

CConnecterDesktop *CPluginTigerVnc::OnCreateConnecter(const QString &szProtol)
{
    if(Id() == szProtol)
    {
        return new CConnecterTigerVnc(this);
    }
    return nullptr;
}

const QIcon CPluginTigerVnc::Icon() const
{
    return QIcon(":/image/Connect");
}
