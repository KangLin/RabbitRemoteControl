#include "Service.h"
#include <QTimer>
#include "RabbitCommonLog.h"

CService::CService(QObject *parent) : QObject(parent),
    m_pPara(nullptr)
{
}

CService::~CService()
{
    LOG_MODEL_DEBUG("CService", "CService::~CService()");
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

    if(m_pPara)
    {
        m_pPara->deleteLater();
        m_pPara = nullptr;
    }
    return 0;
}

int CService::OnClean()
{
    return 0;
}

void CService::slotProcess()
{
    int nRet = OnProcess();
    if(nRet < 0) return;
    if(0 == nRet)
        QTimer::singleShot(0, this, SLOT(slotProcess()));
}

int CService::OnProcess()
{
    return 0;
}

int CService::OnLoad(QDataStream &d)
{
    return 0;
}

int CService::OnSave(QDataStream &d)
{
    return 0;
}

CParameterService* CService::GetParameters()
{
    return m_pPara;
}
