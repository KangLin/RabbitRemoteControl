// Author: Kang Lin <kl222@126.com>

#include "BackendSSH.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Plugin.SSH.Backend")

CBackendSSH::CBackendSSH(COperateSSH *pOperate)
    : CBackend(pOperate)
    , m_pOperate(pOperate)
    , m_pPara(nullptr)
    , m_pChannelSSH(nullptr)
    , m_pTerminal(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
    m_pPara = pOperate->GetParameter();
    m_pTerminal = qobject_cast<QTermWidget*>(m_pOperate->GetViewer());
}

CBackendSSH::~CBackendSSH()
{
    qDebug(log) << Q_FUNC_INFO;
}

CBackendSSH::OnInitReturnValue CBackendSSH::OnInit()
{
    bool check = false;
    if(!m_pTerminal)
    {
        Q_ASSERT(m_pTerminal);
        return OnInitReturnValue::Fail;
    }

    m_pChannelSSH = new CChannelSSHTerminal(this, &m_pPara->m_SSH);
    if(!m_pChannelSSH) {
        qCritical(log) << "new CChannelSSH fail";
        return OnInitReturnValue::Fail;
    }

    check = connect(m_pChannelSSH, SIGNAL(sigConnected()),
                    this, SIGNAL(sigRunning()));
    Q_ASSERT(check);
    check = connect(m_pChannelSSH, &CChannelSSHTerminal::readyRead,
                    this, [&](){
        char buf[256];
        int nLen = 0;
        int nRet = 0;
        if(m_pChannelSSH)
            nLen = m_pChannelSSH->read(buf, 256);
        if(m_pTerminal && nLen > 0) {
#if defined(Q_OS_UNIX)
            nRet = write(m_pTerminal->getPtySlaveFd(), buf, nLen);
#endif
        }
        else
            qCritical(log) << "The m_pTerminal is nullptr or nLen <= 0";
        qDebug(log) << "Write data to QTermWidget: " << nRet << nLen;

    });
    Q_ASSERT(check);
    check = connect(m_pTerminal, &QTermWidget::sendData,
                    this, [&](const char* data, int len){
        if(m_pChannelSSH && len > 0) {
            qint64 nRet = m_pChannelSSH->write(data, len);
            qDebug(log) << "Write data to ssh channel:" << nRet << len;
        } else {
            qCritical(log) << "m_pChannelSSH && len <= 0";
        }
    });
    Q_ASSERT(check);

    m_pChannelSSH->SetSize(m_pTerminal->screenLinesCount(), m_pTerminal->screenColumnsCount());

    if(!m_pChannelSSH->open(QIODevice::ReadWrite)) {
        qCritical(log) << "Open ssh channel fail";
        return OnInitReturnValue::Fail;
    }

    m_pTerminal->startTerminalTeletype();

    return OnInitReturnValue::UseOnProcess;
}

int CBackendSSH::OnProcess()
{
    int nRet = 0;
    if(m_pChannelSSH)
        nRet = m_pChannelSSH->OnProcess(100000);
    return nRet;
}

int CBackendSSH::OnClean()
{
    int nRet = 0;
    m_pChannelSSH->WakeUp();
    if(m_pChannelSSH) {
        m_pChannelSSH->close();
        m_pChannelSSH->deleteLater();
        m_pChannelSSH = nullptr;
    }
    return nRet;
}
