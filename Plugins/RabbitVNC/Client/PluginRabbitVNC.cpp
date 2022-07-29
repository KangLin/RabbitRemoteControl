// Author: Kang Lin <kl222@126.com>

#include "PluginRabbitVNC.h"
#include "ConnecterRabbitVNC.h"
#include <rfb/LogWriter.h>
#include <rfb/Logger_stdio.h>
#include <QDebug>
#include <QApplication>
#include "RabbitCommonDir.h"

static bool initlog = false;
CPluginRabbitVNC::CPluginRabbitVNC(QObject *parent)
    : CPluginClientThread(parent)
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

CPluginRabbitVNC::~CPluginRabbitVNC()
{
    //! [Clean resource]
    qDebug() << "CPluginRabbitVnc::~CPluginRabbitVnc()";
    //! [Clean resource]
}

const QString CPluginRabbitVNC::Name() const
{
    return "RabbitVNC";
}

const QString CPluginRabbitVNC::DisplayName() const
{
    return tr("RabbitVNC");
}

const QString CPluginRabbitVNC::Description() const
{
    return tr("VNC(Virtual Network Console): Access remote desktops such as unix/linux, windows, etc.") + "\n"
            + tr("VNC is a set of programs using the RFB (Remote Frame Buffer) protocol.");
}

const QString CPluginRabbitVNC::Protol() const
{
    return "RFB";
}

CConnecterDesktop *CPluginRabbitVNC::OnCreateConnecter(const QString &szProtol)
{
    if(Id() == szProtol)
    {
        return new CConnecterRabbitVNC(this);
    }
    return nullptr;
}

const QIcon CPluginRabbitVNC::Icon() const
{
    return QIcon(":/image/Connect");
}
