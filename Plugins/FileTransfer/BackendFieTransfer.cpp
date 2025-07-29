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
        MakeDir,
        RemoveDir,
        GetDir,
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
    CRemoteFileSystem* m_pRemoteFileSystem;
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
        case CFileTransferEvent::Command::GetDir:
        {
            if(m_pSFTP) {
                m_pSFTP->slotGetDir(pEvent->m_pRemoteFileSystem);
            }
            break;
        }
        case CFileTransferEvent::Command::Upload:
        case CFileTransferEvent::Command::Download:
        default:
            break;
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
    check = connect(pForm, SIGNAL(sigGetDir(CRemoteFileSystem*)),
                    this, SLOT(slotGetDir(CRemoteFileSystem*)),
                    Qt::DirectConnection);
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigGetDir(CRemoteFileSystem*, QVector<QSharedPointer<CRemoteFileSystem> >, bool)),
                    pForm, SIGNAL(sigGetDir(CRemoteFileSystem*, QVector<QSharedPointer<CRemoteFileSystem> >, bool)));
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

void CBackendFieTransfer::slotGetDir(CRemoteFileSystem *p)
{
    if(!p || p->GetPath().isEmpty())
        return;
    QString szPath = p->GetPath();
    CFileTransferEvent* pEvent = new CFileTransferEvent(
        CFileTransferEvent::Command::GetDir, szPath);
    pEvent->m_pRemoteFileSystem = (CRemoteFileSystem*) p;
    QCoreApplication::postEvent(this, pEvent);
    WakeUp();
}
