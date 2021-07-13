#include "Service.h"
#include <QTimer>

CService::CService(QObject *parent) : QObject(parent)
{
}

bool CService::Enable()
{
    return true;
}

int CService::Init()
{
    OnInit();
    QTimer::singleShot(0, this, SLOT(slotProcess()));
    return 0;
}

int CService::OnInit()
{
    return 0;
}

int CService::Clean()
{
    OnClean();
    return 0;
}

int CService::OnClean()
{
    return 0;
}

void CService::slotProcess()
{
    int nRet = OnProcess();
    if(nRet) return;
    QTimer::singleShot(0, this, SLOT(slotProcess()));
}

int CService::OnProcess()
{
    return 0;
}
