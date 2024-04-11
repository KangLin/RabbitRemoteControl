#include "ConnecterDesktop.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Client.Connecter.Desktop")

CConnecterDesktop::CConnecterDesktop(CPluginClient *parent)
    : CConnecterDesktopThread(parent)
{
}

CConnecterDesktop::~CConnecterDesktop()
{
    qDebug(log) << "CConnecterDesktop::~CConnecterDesktop";
}
int CConnecterDesktop::Connect()
{
    qDebug(log) << "CConnecterDesktop::Connect";
    emit sigOpenConnect(this);
    return 0;
}

int CConnecterDesktop::DisConnect()
{
    qDebug(log) << "CConnecterDesktop::DisConnect";
    emit sigCloseconnect(this);
    return 0;
}

