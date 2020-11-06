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
    return Protol();
}

QString CManageConnectTigerVnc::Description()
{
    return "Access remote desktops such as unix/linux, windows, etc.";
}

QString CManageConnectTigerVnc::Protol()
{
    return "VNC";
}

CConnecter* CManageConnectTigerVnc::CreateConnecter(const QString &szProtol)
{
    if(Protol() == szProtol)
    {   
        CConnecter* p = new CConnecterTigerVnc(this);
        return p;
    }
    return nullptr;
}
