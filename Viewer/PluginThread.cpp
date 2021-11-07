#include "PluginThread.h"
#include "RabbitCommonLog.h"
#include <QTimer>
#include "ManageConnect.h"

CPluginThread::CPluginThread()
    : CConnectThread(nullptr)
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
    check = connect(this, SIGNAL(sigConnect(CConnecter*)),
                    &mc, SLOT(slotConnect(CConnecter*)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigDisconnect(CConnecter*)),
                    &mc, SLOT(slotDisconnect(CConnecter*)));
    Q_ASSERT(check);
    
    exec();
}
