#include "ConnectFreeRdp.h"
#include <QDebug>

CConnectFreeRdp::CConnectFreeRdp(CFrmViewer *pView, QObject *parent)
    : CConnect(pView, parent)
{
    
}

CConnectFreeRdp::~CConnectFreeRdp()
{
    qDebug() << "CConnectFreeRdp::~CConnectFreeRdp()";
}

int CConnectFreeRdp::Connect()
{
    int nRet = 0;
    
    return nRet;
}

int CConnectFreeRdp::Disconnect()
{
    int nRet = 0;
    
    return nRet;
}

int CConnectFreeRdp::Process()
{
    int nRet = 0;
    
    return nRet;
}

void CConnectFreeRdp::slotClipBoardChange()
{
}
