#include "PluginThread.h"
#include "RabbitCommonLog.h"
#include <QTimer>
#include "ManageConnect.h"

CPluginThread::CPluginThread()
    : QThread()
{
}

CPluginThread::~CPluginThread()
{
    LOG_MODEL_DEBUG("CConnecterThread", "CConnecterThread::~CConnecterThread()");
}

/*!
 * \brief One thread handles multiple CConnecter.
 *        Register sigConnect and sigDisconnect, then enter event loop.
 *        
 */
void CPluginThread::run()
{
    LOG_MODEL_DEBUG("CConnecterThread", "CConnecterThread::run()");
    CManageConnect mc;
    bool check = false;
    check = connect(this, SIGNAL(sigConnect(CConnecterDesktop*)),
                    &mc, SLOT(slotConnect(CConnecterDesktop*)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigDisconnect(CConnecterDesktop*)),
                    &mc, SLOT(slotDisconnect(CConnecterDesktop*)));
    Q_ASSERT(check);
    
    exec();
}
