// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#include <QCoreApplication>
#include <QLoggingCategory>
#include "BackendFileTransfer.h"

static Q_LOGGING_CATEGORY(log, "FileTransfer.Backend")

class CFileTransferEvent : public QEvent
{
public:
    enum class Command {
        MakeDir,
        RemoveDir,
        RemoveFile,
        Rename,
        GetDir,
        StartFileTransfer,
        StopFileTransfer
    };
    Q_ENUM(Command)
    CFileTransferEvent(Command cmd,
                       const QString& szSrcPath = QString(),
                       const QString& szDesPath = QString());
    
    Command m_Command;
    QString m_szSourcePath;
    QString m_szDestination;
    CRemoteFileSystem* m_pRemoteFileSystem;
    QSharedPointer<CFileTransfer> m_FileTransfer;
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

CBackendFileTransfer::CBackendFileTransfer(COperateFileTransfer *pOperate)
    : CBackend(pOperate)
    , m_pOperate(pOperate)
#if HAVE_LIBSSH
    , m_pSFTP(nullptr)
#endif
    , m_pPara(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_pOperate)
        m_pPara = m_pOperate->GetParameter();
    SetConnect(pOperate);
}

CBackendFileTransfer::~CBackendFileTransfer()
{
    qDebug(log) << Q_FUNC_INFO;
}

bool CBackendFileTransfer::event(QEvent *event)
{
    if(event->type() == QEvent::Type::User) {
        CFileTransferEvent* pEvent = (CFileTransferEvent*)event;
        switch(pEvent->m_Command) {
        case CFileTransferEvent::Command::MakeDir:
#if HAVE_LIBSSH
            if(m_pSFTP)
                m_pSFTP->MakeDir(pEvent->m_szSourcePath);
#endif
            break;
        case CFileTransferEvent::Command::RemoveDir:
#if HAVE_LIBSSH
            if(m_pSFTP)
                m_pSFTP->RemoveDir(pEvent->m_szSourcePath);
#endif
            break;
        case CFileTransferEvent::Command::RemoveFile:
#if HAVE_LIBSSH
            if(m_pSFTP)
                m_pSFTP->RemoveFile(pEvent->m_szSourcePath);
#endif
            break;
        case CFileTransferEvent::Command::Rename:
#if HAVE_LIBSSH
            if(m_pSFTP)
                m_pSFTP->Rename(pEvent->m_szSourcePath, pEvent->m_szDestination);
#endif
            break;
        case CFileTransferEvent::Command::GetDir:
        {
#if HAVE_LIBSSH
            if(m_pSFTP)
                m_pSFTP->slotGetDir(pEvent->m_szSourcePath, pEvent->m_pRemoteFileSystem);
#endif
            break;
        }
        case CFileTransferEvent::Command::StartFileTransfer:
        {
#if HAVE_LIBSSH
            if(m_pSFTP)
                m_pSFTP->slotStartFileTransfer(pEvent->m_FileTransfer);
#endif
            break;
        }
        case CFileTransferEvent::Command::StopFileTransfer:
        {
#if HAVE_LIBSSH
            if(m_pSFTP)
                m_pSFTP->slotStopFileTransfer(pEvent->m_FileTransfer);
#endif
            break;
        }
        default:
            break;
        }
    }
    return CBackend::event(event);
}

CBackend::OnInitReturnValue CBackendFileTransfer::OnInit()
{
    OnInitReturnValue nRet = OnInitReturnValue::Fail;
    if(m_pPara->GetProtocol() == CParameterFileTransfer::Protocol::SFTP)
        nRet = InitSFTP();
    return nRet;
}

int CBackendFileTransfer::OnClean()
{
    int nRet = 0;
#if HAVE_LIBSSH
    if(m_pSFTP) {
        m_pSFTP->close();
        m_pSFTP->deleteLater();
        m_pSFTP = nullptr;
    }
#endif
    return nRet;
}

int CBackendFileTransfer::OnProcess()
{
    int nRet = 0;
#if HAVE_LIBSSH
    if(m_pSFTP)
        nRet = m_pSFTP->Process();
#endif
    return nRet;
}

int CBackendFileTransfer::SetConnect(COperateFileTransfer *pOperate)
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
    check = connect(pForm, SIGNAL(sigMakeDir(const QString&)),
                    this, SLOT(slotMakeDir(const QString&)));
    Q_ASSERT(check);
    check = connect(pForm, SIGNAL(sigRemoveDir(const QString&)),
                    this, SLOT(slotRemoveDir(const QString&)));
    Q_ASSERT(check);
    check = connect(pForm, SIGNAL(sigRemoveFile(const QString&)),
                    this, SLOT(slotRemoveFile(const QString&)));
    Q_ASSERT(check);
    check = connect(pForm, SIGNAL(sigRename(const QString&, const QString&)),
                    this, SLOT(slotRename(const QString&, const QString&)));
    Q_ASSERT(check);
    check = connect(pForm, SIGNAL(sigStartFileTransfer(QSharedPointer<CFileTransfer>)),
                    this, SLOT(slotStartFileTransfer(QSharedPointer<CFileTransfer>)));
    Q_ASSERT(check);
    check = connect(pForm, SIGNAL(sigStopFileTransfer(QSharedPointer<CFileTransfer>)),
                    this, SLOT(slotStopFileTransfer(QSharedPointer<CFileTransfer>)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigFileTransferUpdate(QSharedPointer<CFileTransfer>)),
                    pForm, SLOT(slotFileTransferUpdate(QSharedPointer<CFileTransfer>)));
    Q_ASSERT(check);
    return nRet;
}

CBackendFileTransfer::OnInitReturnValue CBackendFileTransfer::InitSFTP()
{
    CParameterSSH* ssh = &m_pOperate->GetParameter()->m_SSH;
#if HAVE_LIBSSH
    m_pSFTP = new CChannelSFTP(this, ssh);
    if(!m_pSFTP) {
        return OnInitReturnValue::Fail;
    }
    bool bRet = m_pSFTP->open(QIODevice::ReadWrite);
    if(!bRet) {
        QString szErr;
        szErr = tr("Open SFTP fail.") + m_pSFTP->errorString();
        qCritical(log) << szErr;
        emit sigShowMessageBox(tr("Error"), szErr, QMessageBox::Critical);
        return OnInitReturnValue::Fail;
    }
#endif
    emit sigRunning();
    return OnInitReturnValue::UseOnProcess;
}

void CBackendFileTransfer::slotMakeDir(const QString &szDir)
{
    if(szDir.isEmpty()) return;
    CFileTransferEvent* pEvent = new CFileTransferEvent(
        CFileTransferEvent::Command::MakeDir, szDir);
    QCoreApplication::postEvent(this, pEvent);
    WakeUp();
}

void CBackendFileTransfer::slotRemoveDir(const QString &szDir)
{
    if(szDir.isEmpty()) return;
    CFileTransferEvent* pEvent = new CFileTransferEvent(
        CFileTransferEvent::Command::RemoveDir, szDir);
    QCoreApplication::postEvent(this, pEvent);
    WakeUp();
}

void CBackendFileTransfer::slotRemoveFile(const QString &szFile)
{
    if(szFile.isEmpty()) return;
    CFileTransferEvent* pEvent = new CFileTransferEvent(
        CFileTransferEvent::Command::RemoveFile, szFile);
    QCoreApplication::postEvent(this, pEvent);
    WakeUp();
}

void CBackendFileTransfer::slotRename(const QString &oldName, const QString &newName)
{
    if(oldName.isEmpty() && newName.isEmpty()) return;
    CFileTransferEvent* pEvent = new CFileTransferEvent(
        CFileTransferEvent::Command::Rename, oldName, newName);
    QCoreApplication::postEvent(this, pEvent);
    WakeUp();
}

void CBackendFileTransfer::slotGetDir(CRemoteFileSystem *pRemoteFileSystem)
{
    if(!pRemoteFileSystem || pRemoteFileSystem->GetPath().isEmpty())
        return;
    QString szPath = pRemoteFileSystem->GetPath();
    CFileTransferEvent* pEvent = new CFileTransferEvent(
        CFileTransferEvent::Command::GetDir, szPath);
    pEvent->m_pRemoteFileSystem = (CRemoteFileSystem*) pRemoteFileSystem;
    QCoreApplication::postEvent(this, pEvent);
    WakeUp();
}

void CBackendFileTransfer::slotStartFileTransfer(QSharedPointer<CFileTransfer> f)
{
    if(!f) return;
    CFileTransferEvent* pEvent = new CFileTransferEvent(
        CFileTransferEvent::Command::StartFileTransfer);
    pEvent->m_FileTransfer = f;
    QCoreApplication::postEvent(this, pEvent);
    WakeUp();
}

void CBackendFileTransfer::slotStopFileTransfer(QSharedPointer<CFileTransfer> f)
{
    if(!f) return;
    CFileTransferEvent* pEvent = new CFileTransferEvent(
        CFileTransferEvent::Command::StopFileTransfer);
    pEvent->m_FileTransfer = f;
    QCoreApplication::postEvent(this, pEvent);
    WakeUp();
}
