#include "PluginThread.h"
#include "ManageConnect.h"

#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Client.Plugin.Thread")

CPluginThread::CPluginThread(QObject *parent)
    : QThread(parent)
{}

CPluginThread::~CPluginThread()
{
    qDebug(log) << "CConnecterThread::~CConnecterThread()";
}

/*!
 * \brief One thread handles multiple CConnecter.
 *        Register sigConnect and sigDisconnect, then enter event loop.
 *        
 */
void CPluginThread::run()
{
    qDebug(log) << "CConnecterThread::run() start";
    CManageConnect mc;
    bool check = false;
    check = connect(this, SIGNAL(sigConnect(CConnecterConnect*)),
                    &mc, SLOT(slotConnect(CConnecterConnect*)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigDisconnect(CConnecterConnect*)),
                    &mc, SLOT(slotDisconnect(CConnecterConnect*)));
    Q_ASSERT(check);
    
    exec();
    qDebug(log) << "CConnecterThread::run() end";
}
