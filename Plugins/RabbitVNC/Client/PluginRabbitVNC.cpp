// Author: Kang Lin <kl222@126.com>

#include "PluginRabbitVNC.h"

#include <rfb/LogWriter.h>
#include <rfb/Logger_stdio.h>

#include "RabbitCommonDir.h"
#include "../../TigerVnc/Client/ConnecterVnc.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(RabbitVNC, "RabbitVNC")

CPluginRabbitVNC::CPluginRabbitVNC(QObject *parent)
    : CPluginClient(parent)
{
    //! [Initialize resource]

    //rfb::SecurityClient::setDefaults();
    static bool initlog = false;
    if(!initlog)
    {
        rfb::initStdIOLoggers();

        QString szFile = RabbitCommon::CDir::Instance()->GetDirLog()
                + QDir::separator()
                + "RabbitVNC.log";
        rfb::initFileLogger(szFile.toStdString().c_str());

        rfb::LogWriter::setLogParams("*:stderr:100");
        initlog = true;
    }
    //! [Initialize resource]
}

CPluginRabbitVNC::~CPluginRabbitVNC()
{
    //! [Clean resource]
    qDebug(RabbitVNC) << "CPluginRabbitVnc::~CPluginRabbitVnc()";
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

const QString CPluginRabbitVNC::Protocol() const
{
    return "RFB";
}

CConnecter *CPluginRabbitVNC::CreateConnecter(const QString &szID)
{
    if(Id() == szID)
    {
        return new CConnecterVnc(this);
    }
    return nullptr;
}


const QIcon CPluginRabbitVNC::Icon() const
{
    return QIcon::fromTheme("network-wired");
}

const QString CPluginRabbitVNC::Details() const
{
    QString szDetails;
    return szDetails;
}
