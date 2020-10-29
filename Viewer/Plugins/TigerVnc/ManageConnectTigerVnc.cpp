#include "ManageConnectTigerVnc.h"
#include "ConnecterTigerVnc.h"
#include <rfb/LogWriter.h>
#include <rfb/Logger_stdio.h>
#include <QDebug>

static bool initlog = false;
CManageConnectTigerVnc::CManageConnectTigerVnc(QObject *parent)
    : CManageConnecter(parent)
{
    if(!initlog)
    {
        rfb::initStdIOLoggers();
#ifdef WIN32
        rfb::initFileLogger("C:\\temp\\vncviewer.log");
#else
        rfb::initFileLogger("/tmp/vncviewer.log");
#endif
        rfb::LogWriter::setLogParams("*:stderr:100");
        initlog = true;
    }
}

CManageConnectTigerVnc::~CManageConnectTigerVnc()
{
    qDebug() << "CManageConnectTigerVnc::~CManageConnectTigerVnc()";
}

QString CManageConnectTigerVnc::Name()
{
    return "VNC";
}

QString CManageConnectTigerVnc::Description()
{
    return "Access remote desktops such as unix/linux, windows, etc.";
}

CConnecter* CManageConnectTigerVnc::CreateConnecter(const QString &szName)
{
    if(Name() == szName)
        return new CConnecterTigerVnc(this);
    return nullptr;
}
