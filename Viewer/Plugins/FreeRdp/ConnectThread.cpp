#include "ConnectThread.h"
#include "ConnectFreeRdp.h"
#include "ConnecterFreeRdp.h"

#include <QDebug>

CConnectThread::CConnectThread(CFrmViewer *pView,
                               CConnecter *pConnecter,
                               QObject *parent)
    : QThread(parent),
    m_bExit(false),
    m_pView(pView),
    m_pConnecter(pConnecter)
{}

CConnectThread::~CConnectThread()
{
    m_bExit = true;
    qDebug() << "CConnectThread::~CConnectThread()";
}

void CConnectThread::run()
{
    CConnecterFreeRdp* connecter = dynamic_cast<CConnecterFreeRdp*>(m_pConnecter);
    CConnectFreeRdp c(m_pView);

    bool check = connect(&c, SIGNAL(sigConnected()),
                         m_pConnecter, SIGNAL(sigConnected()));
    Q_ASSERT(check);
    check = connect(&c, SIGNAL(sigDisconnected()),
                    m_pConnecter, SIGNAL(sigDisconnected()));
    Q_ASSERT(check);
    check = connect(m_pConnecter, SIGNAL(sigDisconnected()),
            &c, SLOT(Disconnect()), Qt::DirectConnection);
    Q_ASSERT(check);
    
    c.Connect();
    
    while (!m_bExit) {
        int nRet = c.Process();
        if(nRet) break;
    }
    
    emit m_pConnecter->sigDisconnected();

    qDebug() << "CConnectThread::run() end";
}
