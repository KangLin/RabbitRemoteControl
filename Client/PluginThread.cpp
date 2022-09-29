#include "PluginThread.h"
#include <QTimer>
#include "ManageConnect.h"

CPluginThread::CPluginThread()
    : QThread()
{
}

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
    qDebug(Client) << "CConnecterThread::run()";
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
