// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QCoreApplication>
#include <QLoggingCategory>
#include "BackendFieTransfer.h"

static Q_LOGGING_CATEGORY(log, "FileTransfer.Backend")

class CFileTransferEvent : public QEvent
{
public:
    enum class Command {
        GetFolder,
        Upload,
        Download
    };
    Q_ENUM(Command)
    CFileTransferEvent(Command cmd,
                       const QString& szSrcPath = QString(),
                       const QString& szDesPath = QString());
    
    Command m_Command;
    QString m_szSourcePath;
    QString m_szDestination;
};

CFileTransferEvent::CFileTransferEvent(Command cmd,
                                       const QString &szSrcPath,
                                       const QString &szDesPath)
    : QEvent(QEvent::Type::User)
    , m_Command(cmd)
    , m_szSourcePath(szSrcPath)
    , m_szDestination(szDesPath)
{
}

CBackendFieTransfer::CBackendFieTransfer(COperateFileTransfer *pOperate)
    : CBackend(pOperate)
    , m_pOperate(pOperate)
    , m_pSFTP(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
    SetConnect(pOperate);
}

CBackendFieTransfer::~CBackendFieTransfer()
{
    qDebug(log) << Q_FUNC_INFO;
}

bool CBackendFieTransfer::event(QEvent *event)
{
    if(event->type() == QEvent::Type::User) {
        CFileTransferEvent* pEvent = (CFileTransferEvent*)event;
        switch(pEvent->m_Command) {
        case CFileTransferEvent::Command::GetFolder:
        {
            if(m_pSFTP)
                m_pSFTP->slotGetFolder(pEvent->m_szSourcePath);
            break;
        }
        }
    }
    return CBackend::event(event);
}

CBackend::OnInitReturnValue CBackendFieTransfer::OnInit()
{
    OnInitReturnValue nRet = InitSFTP();
    return nRet;
}

int CBackendFieTransfer::OnClean()
{
    int nRet = 0;
    if(m_pSFTP) {
        m_pSFTP->close();
        m_pSFTP->deleteLater();
        m_pSFTP = nullptr;
    }
    return nRet;
}

int CBackendFieTransfer::OnProcess()
{
    int nRet = 0;
    
    if(m_pSFTP)
        nRet = m_pSFTP->Process();
    return nRet;
}

int CBackendFieTransfer::SetConnect(COperateFileTransfer *pOperate)
{
    int nRet = 0;
    bool check = false;
    CFrmFileTransfer* pForm = qobject_cast<CFrmFileTransfer*>(pOperate->GetViewer());
    Q_ASSERT(pForm);
    check = connect(pForm, SIGNAL(sigGetFolder(const QString&)),
                    this, SLOT(slotGetFolder(const QString&)),
                    Qt::DirectConnection);
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigGetFolder(const QString&, QVector<CRemoteFileSystem*>)),
                    pForm, SLOT(slotGetFolder(const QString&, QVector<CRemoteFileSystem*>)));
    Q_ASSERT(check);
    return nRet;
}

CBackendFieTransfer::OnInitReturnValue CBackendFieTransfer::InitSFTP()
{
    CParameterSSH* ssh = &m_pOperate->GetParameter()->m_SSH;
    m_pSFTP = new CChannelSFTP(this, ssh);
    if(!m_pSFTP) {
        return OnInitReturnValue::Fail;
    }
    bool bRet = m_pSFTP->open(QIODevice::ReadWrite);
    if(!bRet)
        return OnInitReturnValue::Fail;
    emit sigRunning();
    return OnInitReturnValue::UseOnProcess;
}

void CBackendFieTransfer::slotGetFolder(const QString& szPath)
{
    CFileTransferEvent* pEvent = new CFileTransferEvent(
        CFileTransferEvent::Command::GetFolder, szPath);
    QCoreApplication::postEvent(this, pEvent);
    WakeUp();
}
