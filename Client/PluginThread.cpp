#include "PluginThread.h"
#include "ManageConnect.h"

#include <QTimer>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(Client)

CPluginThread::CPluginThread(QObject *parent)
    : QThread(parent)
{}

CPluginThread::~CPluginThread()
{
    qDebug(Client) << "CConnecterThread::~CConnecterThread()";
}

/*!
 * \brief One thread handles multiple CConnecter.
 *        Register sigConnect and sigDisconnect, then enter event loop.
 *        
 */
void CPluginThread::run()
{
    qDebug(Client) << "CConnecterThread::run() start";
    CManageConnect mc;
    bool check = false;
    check = connect(this, SIGNAL(sigConnect(CConnecterDesktop*)),
                    &mc, SLOT(slotConnect(CConnecterDesktop*)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigDisconnect(CConnecterDesktop*)),
                    &mc, SLOT(slotDisconnect(CConnecterDesktop*)));
    Q_ASSERT(check);
    
    exec();
    qDebug(Client) << "CConnecterThread::run() end";
}
