// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "ChannelSFTP.h"
#include "BackendFileTransfer.h"
#include "RemoteFileSystemModel.h"
#include "ListFileModel.h"

#include <fcntl.h>
#include <sys/stat.h>

#ifdef _MSC_VER
    // See: [_topen](https://learn.microsoft.com/zh-cn/cpp/c-runtime-library/reference/open-wopen?view=msvc-170)
    #include <io.h>
    #define S_IRUSR  0400  // Owner read permission
    #define S_IWUSR  0200  // Owner write permission
    #define S_IXUSR  0100  // Owner execute permission
    #define S_IRWXU  (S_IREAD | S_IWRITE | S_IEXEC)  // Owner read, write, execute
#endif

static Q_LOGGING_CATEGORY(log, "Channel.SFTP")

CChannelSFTP::CChannelSFTP(CBackend *pBackend, CParameterSSH *pPara,
                           bool bWakeUp, QObject *parent)
    : CChannelSSH(pBackend, pPara, bWakeUp, parent)
    , m_SessionSftp(nullptr)
{
    bool check = false;
    CBackendFileTransfer* pB = qobject_cast<CBackendFileTransfer*>(pBackend);
    if(pB) {
        check = connect(this, SIGNAL(sigGetDir(CRemoteFileSystem*, QVector<QSharedPointer<CRemoteFileSystem> >, bool)),
                        pB, SIGNAL(sigGetDir(CRemoteFileSystem*, QVector<QSharedPointer<CRemoteFileSystem> >, bool)));
        Q_ASSERT(check);
        check = connect(this, SIGNAL(sigFileTransferUpdate(QSharedPointer<CFileTransfer>)),
                        pB, SIGNAL(sigFileTransferUpdate(QSharedPointer<CFileTransfer>)));
        Q_ASSERT(check);
        check = connect(this, SIGNAL(sigError(int,QString)),
                        pB, SIGNAL(sigError(int,QString)));
        Q_ASSERT(check);
    }
}

int CChannelSFTP::Process()
{
    int nRet = 0;

    struct timeval timeout = {0, DEFAULT_TIMEOUT};
    ssh_channel channels[2], channel_out[2];
    channels[0] = nullptr; //m_SessionSftp->channel;
    channels[1] = nullptr;

    fd_set set;
    FD_ZERO(&set);
    socket_t fdEvent = SSH_INVALID_SOCKET;
    if(m_pEvent)
        fdEvent = m_pEvent->GetFd();
    if(SSH_INVALID_SOCKET != fdEvent)
        FD_SET(fdEvent, &set);

    socket_t sshFd = ssh_get_fd(m_Session);
    if(SSH_INVALID_SOCKET != sshFd)
        FD_SET(sshFd, &set);

    socket_t f = qMax(sshFd, fdEvent);
    //qDebug(log) << "ssh_select:" << fdEvent;
    nRet = ssh_select(channels, channel_out, f + 1, &set, &timeout);
    //qDebug(log) << "ssh_select end:" << nRet;
    if(EINTR == nRet)
        return 0;

    if(SSH_OK != nRet) {
        QString szErr;
        szErr = "ssh_channel_select failed: " + QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        qCritical(log) << szErr;
        setErrorString(szErr);
        return -3;
    }

    if(SSH_INVALID_SOCKET != fdEvent && FD_ISSET(fdEvent, &set)) {
        //qDebug(log) << "fires event";
        if(m_pEvent) {
            nRet = m_pEvent->Reset();
            if(nRet) {
                QString szErr = "Reset event fail";
                qCritical(log) << szErr;
                setErrorString(szErr);
                return -4;
            }
        }
    }

    AsyncReadDir();

    AsyncFile();

    return 0;
}

QSharedPointer<CRemoteFileSystem> CChannelSFTP::GetFileNode(
    const QString& szPath, sftp_attributes attributes)
{
    if(!attributes) return nullptr;
    /*
    qDebug(log) << szPath << "name:" << attributes->name
                << "size:" << attributes->size << "type:" << attributes->type;//*/
    QString szName = QString::fromUtf8(attributes->name);
    if("." == szName || ".." == szName)
        return nullptr;
    CRemoteFileSystem::TYPE type = CRemoteFileSystem::TYPE::NO;
    switch(attributes->type) {
    case SSH_FILEXFER_TYPE_DIRECTORY:
        type = CRemoteFileSystem::TYPE::DIR;
        break;
    case SSH_FILEXFER_TYPE_SYMLINK:
        type = CRemoteFileSystem::TYPE::SYMLINK;
        break;
    case SSH_FILEXFER_TYPE_REGULAR:
        type = CRemoteFileSystem::TYPE::FILE;
        break;
    case SSH_FILEXFER_TYPE_SPECIAL:
        type = CRemoteFileSystem::TYPE::SPECIAL;
        break;
    default:
        qWarning(log) << "Unsupported type:" << attributes->type;
        return nullptr;
        break;
    }
    if(szPath.right(1) == '/')
        szName = szPath + szName;
    else
        szName = szPath + "/" + szName;
    QSharedPointer<CRemoteFileSystem> p(new CRemoteFileSystem(szName, type));
    p->SetSize(attributes->size);
    p->SetPermissions((QFileDevice::Permissions)attributes->permissions);
    p->SetLastModified(QDateTime::fromSecsSinceEpoch(attributes->mtime));
    p->SetCreateTime(QDateTime::fromSecsSinceEpoch(attributes->createtime));
    return p;
}

int CChannelSFTP::GetDir(CRemoteFileSystem* p)
{
    int nRet = SSH_OK;
    if(!m_SessionSftp || !p)
        return -1;
    sftp_dir dir = nullptr;
    sftp_attributes attributes = nullptr;

    QString szPath = p->GetPath();
    QVector<QSharedPointer<CRemoteFileSystem> > vFileNode;
    dir = sftp_opendir(m_SessionSftp, szPath.toStdString().c_str());
    if (!dir)
    {
        QString szErr = "Directory not opened:"
                            + QString::number(sftp_get_error(m_SessionSftp))
                            + ssh_get_error(m_Session);
        qCritical(log) << szErr;
        emit sigError(-1, szErr);
        return SSH_ERROR;
    }

    while ((attributes = sftp_readdir(m_SessionSftp, dir)) != NULL)
    {
        qDebug(log) << "name:" << attributes->name << "size:" << attributes->size;
        auto p = GetFileNode(szPath, attributes);
        if(p)
            vFileNode.append(p);
        sftp_attributes_free(attributes);
    }

    if (!sftp_dir_eof(dir))
    {
        qCritical(log) << "Can't list directory:"
                       << sftp_get_error(m_SessionSftp)
                       << ssh_get_error(m_Session);
        sftp_closedir(dir);
        return SSH_ERROR;
    }

    nRet = sftp_closedir(dir);
    if (nRet != SSH_OK)
    {
        qCritical(log) << "Can't close directory:"
                       << sftp_get_error(m_SessionSftp)
                       << ssh_get_error(m_Session);
        return nRet;
    }

    emit sigGetDir(p, vFileNode, true);

    return nRet;
}

int CChannelSFTP::MakeDir(const QString &dir)
{
    int nRet = SSH_FX_OK;
    if(!m_SessionSftp)
        return SSH_FX_NO_CONNECTION;
    nRet = sftp_mkdir(m_SessionSftp, dir.toStdString().c_str(), S_IRWXU);
    if (nRet != SSH_OK)
    {
        if (sftp_get_error(m_SessionSftp) != SSH_FX_FILE_ALREADY_EXISTS)
        {
            QString szErr = "Can't create directory: "
                            + dir + QString::number(nRet)
                            + ssh_get_error(m_Session);
            qCritical(log) << szErr;
            emit sigError(nRet, szErr);
        } else
            qDebug(log) << "Create directory:" << dir;
    }
    return nRet;
}

int CChannelSFTP::RemoveDir(const QString& dir)
{
    if (!m_SessionSftp)
        return SSH_FX_NO_CONNECTION;

    // 1. 打开目录
    sftp_dir sftpDir = sftp_opendir(m_SessionSftp, dir.toStdString().c_str());
    if (!sftpDir) {
        qCritical(log) << "Failed to open directory for remove:" << dir;
        // 如果打不开目录，尝试直接删除（可能是空目录或者文件）
        int ret = sftp_rmdir(m_SessionSftp, dir.toStdString().c_str());
        if (ret == SSH_OK) {
            qDebug(log) << "Removed directory (directly):" << dir;
            return SSH_OK;
        } else {
            QString szErr = "Can't remove directory:" + dir + ssh_get_error(m_Session);
            qCritical(log) << szErr;
            emit sigError(ret, szErr);
            return ret;
        }
    }

    // 2. 遍历目录内容
    sftp_attributes attr;
    while ((attr = sftp_readdir(m_SessionSftp, sftpDir)) != NULL) {
        QString name = QString::fromUtf8(attr->name);
        if (name == "." || name == "..") {
            sftp_attributes_free(attr);
            continue;
        }
        QString fullPath = dir + "/" + name;
        if (attr->type == SSH_FILEXFER_TYPE_DIRECTORY) {
            // 递归删除子目录
            RemoveDir(fullPath);
        } else {
            // 删除文件
            int ret = sftp_unlink(m_SessionSftp, fullPath.toStdString().c_str());
            if (ret != SSH_OK) {
                QString szErr = "Can't remove file:" + fullPath + ssh_get_error(m_Session);
                qCritical(log) << szErr;
                emit sigError(ret, szErr);
            } else {
                qDebug(log) << "Removed file:" << fullPath;
            }
        }
        sftp_attributes_free(attr);
    }

    sftp_closedir(sftpDir);
    
    // 3. 删除空目录
    int ret = sftp_rmdir(m_SessionSftp, dir.toStdString().c_str());
    if (ret != SSH_OK) {
        QString szErr = "Can't remove directory:" + dir + ssh_get_error(m_Session);
        qCritical(log) << szErr;
        emit sigError(ret, szErr);
    } else {
        qDebug(log) << "Removed directory:" << dir;
    }
    return ret;
}

int CChannelSFTP::RemoveFile(const QString &file)
{
    // 删除文件
    int ret = sftp_unlink(m_SessionSftp, file.toStdString().c_str());
    if (ret != SSH_OK) {
        QString szErr = "Can't remove file:" + file + ssh_get_error(m_Session);
        qCritical(log) << szErr;
        emit sigError(ret, szErr);
    } else {
        qDebug(log) << "Removed file:" << file;
    }
    return ret;
}

int CChannelSFTP::Rename(const QString &oldPath, const QString &newPath)
{
    int nRet = SSH_FX_OK;
    if(!m_SessionSftp)
        return SSH_FX_NO_CONNECTION;
    nRet = sftp_rename(m_SessionSftp,
                       oldPath.toStdString().c_str(),
                       newPath.toStdString().c_str());
    if (nRet != SSH_OK)
    {
        QString szErr = "Fail: Can't rename: " + QString::number(nRet)
                       + ssh_get_error(m_Session)
                       + " " + oldPath + " to " + newPath;
        qCritical(log) << szErr;
        emit sigError(nRet, szErr);
    } else
        qDebug(log) << "Rename:" << oldPath << "to" << newPath;
    return nRet;
}

qint64 CChannelSFTP::readData(char *data, qint64 maxlen)
{
    qint64 nRet = 0;
    
    return nRet;
}

qint64 CChannelSFTP::writeData(const char *data, qint64 len)
{
    qint64 nRet = 0;
    
    return nRet;
}

int CChannelSFTP::OnOpen(ssh_session session)
{
    int nRet = SSH_OK;

    m_SessionSftp = sftp_new(session);
    if (!m_SessionSftp)
    {
        QString szErr = "Error allocating SFTP session: ";
        szErr += ssh_get_error(session);
        qCritical(log) << szErr;
        emit sigError(nRet, szErr);
        return SSH_ERROR;
    }

    nRet = sftp_init(m_SessionSftp);
    if (SSH_OK != nRet)
    {
        QString szErr = "Error initializing SFTP session:" + sftp_get_error(m_SessionSftp);
        qCritical(log) << szErr;
        emit sigError(nRet, szErr);
        sftp_free(m_SessionSftp);
        m_SessionSftp = nullptr;
        return nRet;
    }

    return nRet;
}

void CChannelSFTP::OnClose()
{
    foreach (auto d, m_vDirs) {
        if(d->sftp) {
            sftp_closedir(d->sftp);
            d->sftp = nullptr;
        }
    }
    m_vDirs.clear();
    foreach(auto f, m_vFiles) {
        if(f->remote) {
            sftp_close(f->remote);
            f->remote = nullptr;
        }
        if(-1 != f->local) {
            ::close(f->local);
            f->local = -1;
        }
    }
    m_vFiles.clear();
    if(m_SessionSftp) {
        sftp_free(m_SessionSftp);
        m_SessionSftp = nullptr;
    }
}

void CChannelSFTP::slotGetDir(CRemoteFileSystem *p)
{
    if(!p) return;
    QString szPath = p->GetPath();
    if(szPath.isEmpty()) {
        qCritical(log) << "The path is empty";
        return;
    }
    foreach(auto d, m_vDirs) {
        if(d->remoteFileSystem == p) {
            qDebug(log) << szPath << "already exists";
            return;
        }
    }
    QSharedPointer<DIR_READER> dir(new DIR_READER());
    if(!dir)
        return;
    dir->sftp = nullptr;
    dir->szPath = szPath;
    dir->state = STATE::OPEN;
    dir->remoteFileSystem = p;
    dir->Error = SSH_FX_OK;
    m_vDirs.append(dir);
    WakeUp();
}

int CChannelSFTP::AsyncReadDir()
{
    //qDebug(log) << Q_FUNC_INFO;
    for(auto it = m_vDirs.begin(); it != m_vDirs.end();) {
        auto d = *it;
        switch (d->state) {
        case STATE::OPEN: {
            d->sftp = sftp_opendir(m_SessionSftp, d->szPath.toStdString().c_str());
            if(d->sftp) {
                d->state = STATE::READ;
                break;
            }
            int err = sftp_get_error(m_SessionSftp);
            if (err == SSH_FX_OK || err == SSH_FX_EOF) {
                d->state = STATE::FINISH;
                break;
            }
            if (err == SSH_FX_FAILURE && ssh_get_error_code(m_Session) == SSH_REQUEST_DENIED) {
                qDebug(log) << "Request denied:" << d->szPath;
                break;
            }
            qCritical(log) << "Error opening directory:" << d->szPath
                           << "Error:"
                           << sftp_get_error(m_SessionSftp)
                           << ssh_get_error(m_Session);
            d->state = STATE::ERR;
            d->Error = err;
            break;
        }
        case STATE::READ: {
            sftp_attributes attributes = nullptr;
            while ((attributes = sftp_readdir(m_SessionSftp, d->sftp)) != NULL) {
                auto p = GetFileNode(d->szPath, attributes);
                if(p)
                    d->vFileNode.append(p);
                sftp_attributes_free(attributes);
            }
            // 检查是否结束
            if (sftp_dir_eof(d->sftp)) {
                d->state = STATE::CLOSE;
                break;
            }
            int err = sftp_get_error(m_SessionSftp);
            if (err == SSH_FX_OK)
                break;
            if (err == SSH_FX_EOF) {
                break;
                d->state = STATE::CLOSE;
            }
            qCritical(log) << "Error reading directory:" << d->szPath
                           << "Error:" << err << ssh_get_error(m_Session);
            d->state = STATE::ERR;
            d->Error = err;
            break;
        }
        case STATE::CLOSE: {
            if(!d->sftp) {
                d->state = STATE::FINISH;
                break;
            }
            int rc = sftp_closedir(d->sftp);
            if(SSH_NO_ERROR == rc) {
                d->state = STATE::FINISH;
                d->sftp = nullptr;
                break;
            }
            int err = ssh_get_error_code(m_Session);
            qCritical(log) << "Error close directory:" << d->szPath
                           << "Error:" << err << ssh_get_error(m_Session);
            d->state = STATE::FINISH;
            d->Error = err;
            break;
        }
        case STATE::FINISH:
            if(d && STATE::FINISH == d->state) {
                qDebug(log) << "Remote" << d->szPath << d->vFileNode.size();
                emit sigGetDir(d->remoteFileSystem, d->vFileNode, true);
                it = m_vDirs.erase(it);
            }
            continue;
        case STATE::ERR:
            d->state = STATE::CLOSE;
            break;
        default:
            break;
        }

        it++;
    }

    return 0;
}

void CChannelSFTP::slotStartFileTransfer(QSharedPointer<CFileTransfer> f)
{
    f->slotSetstate(CFileTransfer::State::Opening);
    QSharedPointer<_AFILE> file(new _AFILE);
    memset(file.data(), 0, sizeof(_AFILE));
    file->local = -1;
    file->remote = nullptr;
    file->state = STATE::OPEN;
    file->fileTransfer = f;
#if  LIBSSH_VERSION_INT >= SSH_VERSION_INT(0, 11, 0)
    file->nChunkSize = BUF_SIZE;
    file->nConcurrentCount = 5;
#endif
    m_vFiles.append(file);
    emit sigFileTransferUpdate(f);
    WakeUp();
}

void CChannelSFTP::slotStopFileTransfer(QSharedPointer<CFileTransfer> f)
{
    foreach(auto file, m_vFiles) {
        if(file->fileTransfer == f) {
            m_vFiles.removeAll(file);
            if(-1 != file->local) {
                ::close(file->local);
                file->local = -1;
            }
            if(file->remote) {
                sftp_close(file->remote);
                file->remote = nullptr;
            }
            f->slotSetstate(CFileTransfer::State::Stop);
            emit sigFileTransferUpdate(f);
            break;
        }
    }
}

int CChannelSFTP::AsyncFile()
{
    for(auto it = m_vFiles.begin(); it != m_vFiles.end();) {
        auto file = *it;
        switch(file->state) {
        case STATE::OPEN: {
            int remoteFlag = O_WRONLY | O_CREAT | O_TRUNC;
            int localFlag = O_RDONLY;
            quint32 permission = file->fileTransfer->GetRemotePermission();

            if(file->fileTransfer->GetDirection() == CFileTransfer::Direction::Download) {
                remoteFlag = O_RDONLY;
                localFlag = O_WRONLY | O_CREAT | O_TRUNC;
            }

            file->remote = sftp_open(
                m_SessionSftp,
                file->fileTransfer->GetRemoteFile().toStdString().c_str(),
                remoteFlag, permission); // S_IRWXU);
            if (!file->remote)
            {
                file->state = STATE::ERR;
                QString szErr = "Can't open remote file: " + file->fileTransfer->GetRemoteFile() 
                        + ssh_get_error(m_Session);
                file->fileTransfer->slotSetExplanation(szErr);
                qCritical(log) << szErr;
                break;
            }
            qDebug(log) << "Open remote file:" << file->fileTransfer->GetRemoteFile();

            sftp_file_set_nonblocking(file->remote);

            file->local = ::open(
                file->fileTransfer->GetLocalFile().toStdString().c_str(),
                localFlag, permission);
            if(-1 == file->local) {
                file->state = STATE::ERR;
                QString szErr = "Can't open local file: " + file->fileTransfer->GetLocalFile() + " " + strerror(errno);
                file->fileTransfer->slotSetExplanation(szErr);
                qCritical(log) << szErr;
                break;
            }
            qDebug(log) << "Open local file:" << file->fileTransfer->GetLocalFile();

            if(file->fileTransfer->GetDirection() == CFileTransfer::Direction::Download) {
#if LIBSSH_VERSION_INT >= SSH_VERSION_INT(0, 11, 0)
                if(ssh_version(SSH_VERSION_INT(0, 11,0))) {
                    sftp_limits_t lim = sftp_limits(m_SessionSftp);
                    if(lim) {
                        file->nChunkSize = lim->max_read_length;
                        qDebug(log) << "limits: max_open_handles:" << lim->max_open_handles
                                    << "max_packet_length" << lim->max_packet_length
                                    << "max_read_length" << lim->max_read_length
                                    << "max_write_length:" << lim->max_write_length;
                        sftp_limits_free(lim);
                    }

                    quint64 nRequestBytes = 0;
                    for(int i = 0;
                         i < file->nConcurrentCount
                         && nRequestBytes < file->fileTransfer->GetFileSize();
                         i++) {
                        sftp_aio aio = nullptr;
                        quint64 nRequest = file->fileTransfer->GetFileSize() - nRequestBytes;
                        if(nRequest > file->nChunkSize)
                            nRequest = file->nChunkSize;
                        ssize_t nRet = sftp_aio_begin_read(file->remote, nRequest, &aio);
                        if(0 > nRet) {
                            int rc = ssh_get_error_code(m_Session);
                            if (rc != SSH_NO_ERROR) {
                                QString szErr = "Error during sftp aio download: " + QString::number(rc);
                                szErr += ssh_get_error(m_Session);
                                qCritical(log) << szErr;
                                break;
                            }
                        }
                        Q_ASSERT(nRequest == nRet);
                        nRequestBytes += nRet;
                        file->aio.append(aio);
                    }
                }
#else
                file->asyncReadId = sftp_async_read_begin(file->remote, BUF_SIZE);
                if(file->asyncReadId < 0) {
                    file->state = STATE::ERR;
                    QString szErr = "sftp_async_read_begin failed." + sftp_get_error(m_SessionSftp);
                    file->fileTransfer->slotSetExplanation(szErr);
                    qCritical(log) << szErr;
                    break;
                }
#endif
            } else { // Upload
#if LIBSSH_VERSION_INT >= SSH_VERSION_INT(0, 11, 0)
                if(ssh_version(SSH_VERSION_INT(0, 11,0))) {
                    sftp_limits_t lim = sftp_limits(m_SessionSftp);
                    if(lim) {
                        file->nChunkSize = lim->max_write_length;
                        qDebug(log) << "limits: max_open_handles:" << lim->max_open_handles
                                    << "max_packet_length" << lim->max_packet_length
                                    << "max_read_length" << lim->max_read_length
                                    << "max_write_length:" << lim->max_write_length;
                        sftp_limits_free(lim);
                    }
                    
                    if(!file->buffer)
                        file->buffer = new char[file->nChunkSize];
                    if(!file->buffer) {
                        file->state = STATE::ERR;
                        break;
                    }

                    for(int i = 0;
                         i < file->nConcurrentCount
                         && file->nRequests < file->fileTransfer->GetFileSize();
                         i++) {
                        sftp_aio aio = nullptr;
                        quint64 nRequest = file->fileTransfer->GetFileSize() - file->nRequests;
                        if(nRequest > file->nChunkSize)
                            nRequest = file->nChunkSize;
                        ssize_t nLen = ::read(file->local, file->buffer, nRequest);
                        Q_ASSERT(nLen == nRequest);
                        ssize_t nRet = sftp_aio_begin_write(file->remote, file->buffer, nLen, &aio);
                        if(0 > nRet) {
                            int rc = ssh_get_error_code(m_Session);
                            if (rc != SSH_NO_ERROR) {
                                QString szErr = "Error during sftp aio download: " + QString::number(rc);
                                szErr += ssh_get_error(m_Session);
                                qCritical(log) << szErr;
                                break;
                            }
                        }
                        Q_ASSERT(nRequest == nRet);
                        file->nRequests += nRet;
                        file->aio.append(aio);
                    }
                }
#endif
            }

            file->state = STATE::READ;
            file->fileTransfer->slotSetstate(CFileTransfer::State::Transferring);
            emit sigFileTransferUpdate(file->fileTransfer);
        }
        case STATE::READ: {
            if(file->fileTransfer->GetDirection() == CFileTransfer::Direction::Download) {
#if  LIBSSH_VERSION_INT >= SSH_VERSION_INT(0, 11, 0)
                if(ssh_version(SSH_VERSION_INT(0, 11,0))) {
                    for(auto it = file->aio.begin(); it != file->aio.end();) {
                        auto aio = *it;
                        if(nullptr == file->buffer)
                            file->buffer = new char[file->nChunkSize];
                        if(!(aio && file->buffer))
                            break;
                        ssize_t nRet = sftp_aio_wait_read(&aio, file->buffer, file->nChunkSize);
                        if (nRet < 0) {
                            if(SSH_AGAIN == nRet)
                                break;
                            int rc = ssh_get_error_code(m_Session);
                            if (rc != SSH_NO_ERROR) {
                                file->state = STATE::ERR;
                                QString szErr = "Error during sftp aio download: " + QString::number(rc);
                                szErr += ssh_get_error(m_Session);
                                qCritical(log) << szErr;
                                break;
                            }
                        }
                        file->nTransfers += nRet;
                        if(file->fileTransfer->GetFileSize() == file->nTransfers) {
                            file->state = STATE::CLOSE;
                        }
                        it = file->aio.erase(it);

                        int nLen = ::write(file->local, file->buffer, nRet);
                        if(nLen < 0) {
                            if(EAGAIN != nLen)
                                file->state = STATE::ERR;
                            break;
                        }
                        if(nLen != nRet) {
                            qCritical(log) << "IO is buse, Write file error:" << file->fileTransfer->GetLocalFile();
                            Q_ASSERT(false);
                        }
                        file->fileTransfer->slotTransferSize(nRet);
                        emit sigFileTransferUpdate(file->fileTransfer);
                    }
                    if(file->fileTransfer->GetFileSize() == file->nTransfers) {
                        file->state = STATE::CLOSE;
                        break;
                    }
                    if(file->aio.size() > 0)
                        break;

                    quint64 nRequestBytes = file->nTransfers;
                    for(int i = 0;
                         i < file->nConcurrentCount
                         && nRequestBytes < file->fileTransfer->GetFileSize();
                         i++) {
                        sftp_aio aio = nullptr;
                        quint64 nRequest = file->fileTransfer->GetFileSize() - nRequestBytes;
                        if(nRequest > file->nChunkSize)
                            nRequest = file->nChunkSize;
                        ssize_t nRet = sftp_aio_begin_read(file->remote, nRequest, &aio);
                        if(0 > nRet) {
                            int rc = ssh_get_error_code(m_Session);
                            if (rc != SSH_NO_ERROR) {
                                QString szErr = "Error during sftp aio download: " + QString::number(rc);
                                szErr += ssh_get_error(m_Session);
                                qCritical(log) << szErr;
                                break;
                            }
                        }
                        if (nRequest != nRet) {
                            QString szErr =
                                "Error during sftp aio download: sftp_aio_begin_read() "
                                "requesting less bytes even when the number of bytes "
                                "asked to read are within the max limit";
                            qWarning(log) << szErr << nRequest << nRet;
                        }

                        nRequestBytes += nRet;
                        file->aio.append(aio);
                    }
                } else {
                    file->state = STATE::ERR;
                    QString szErr = tr("Error: Asynchronous uploads are not supported");
                    file->fileTransfer->slotSetExplanation(szErr);
                    qCritical(log) << szErr;
                    break;
                }
#else
                int nbytes = sftp_async_read(file->remote, file->buffer + file->offset, BUF_SIZE, file->asyncReadId);
                if (nbytes > 0 || SSH_AGAIN == nbytes) {
                    if(nbytes > 0 ) {
                        int nLen = ::write(file->local, file->buffer + file->offset, nbytes);
                        if(nLen > 0) {
                            file->fileTransfer->slotTransferSize(nLen);
                            //TODO: add nLen < nbytes
                            emit sigFileTransferUpdate(file->fileTransfer);
                        } else {
                            file->state = STATE::ERR;
                            QString szErr = "Write local file fail:" + sftp_get_error(m_SessionSftp);
                            file->fileTransfer->slotSetExplanation(szErr);
                        }
                    }
                    // Start next async read
                    if (file->asyncReadId = sftp_async_read_begin(file->remote, BUF_SIZE) < 0) {
                        file->state = STATE::ERR;
                        QString szErr = "sftp_async_read_begin failed." + sftp_get_error(m_SessionSftp);
                        file->fileTransfer->slotSetExplanation(szErr);
                        qCritical(log) << szErr;
                    }
                } else if (nbytes == 0) {
                    file->state = STATE::CLOSE;
                    file->asyncReadId = -1;
                } else {                    
                        file->state = STATE::ERR;
                        QString szErr = "sftp_async_read failed." + sftp_get_error(m_SessionSftp);
                        file->fileTransfer->slotSetExplanation(szErr);
                        qCritical(log) << szErr;
                }
#endif
            } else { // Upload
                if(ssh_version(SSH_VERSION_INT(0, 11,0))) {
                    for(auto it = file->aio.begin(); it != file->aio.end();) {
                        auto aio = *it;
                        ssize_t nRet = sftp_aio_wait_write(&aio);
                        if (nRet < 0) {
                            if(SSH_AGAIN == nRet)
                                break;
                            int rc = ssh_get_error_code(m_Session);
                            if (rc != SSH_NO_ERROR) {
                                file->state = STATE::ERR;
                                QString szErr = "Error during sftp aio upload: " + QString::number(rc);
                                szErr += ssh_get_error(m_Session);
                                qCritical(log) << szErr;
                                break;
                            }
                        }
                        file->nTransfers += nRet;
                        if(file->fileTransfer->GetFileSize() == file->nTransfers) {
                            file->state = STATE::CLOSE;
                        }
                        it = file->aio.erase(it);
                    }
                    if(file->aio.size() >= file->nConcurrentCount)
                        break;
                    int size = file->nConcurrentCount - file->aio.size();
                    for(int i = 0;
                         i < size
                         && file->nRequests < file->fileTransfer->GetFileSize();
                         i++) {
                        sftp_aio aio = nullptr;
                        quint64 nRequest = file->fileTransfer->GetFileSize() - file->nRequests;
                        if(nRequest > file->nChunkSize)
                            nRequest = file->nChunkSize;
                        ssize_t nLen = ::read(file->local, file->buffer, nRequest);
                        Q_ASSERT(nLen == nRequest);
                        ssize_t nRet = sftp_aio_begin_write(file->remote, file->buffer, nLen, &aio);
                        if(0 > nRet) {
                            int rc = ssh_get_error_code(m_Session);
                            if (rc != SSH_NO_ERROR) {
                                QString szErr = "Error during sftp aio download: " + QString::number(rc);
                                szErr += ssh_get_error(m_Session);
                                qCritical(log) << szErr;
                                break;
                            }
                        }
                        Q_ASSERT(nRequest == nRet);
                        file->nRequests += nRet;
                        file->aio.append(aio);
                    }
                }
            }
            break;
        }
        case STATE::CLOSE: {
            file->state = STATE::FINISH;
            file->fileTransfer->slotSetstate(CFileTransfer::State::Closing);
            emit sigFileTransferUpdate(file->fileTransfer);
            break;
        }
        case STATE::FINISH: {
#if  LIBSSH_VERSION_INT >= SSH_VERSION_INT(0, 11, 0)
            CleanFileAIO(file);
#endif
            it = m_vFiles.erase(it);
            file->fileTransfer->slotSetstate(CFileTransfer::State::Finish);
            emit sigFileTransferUpdate(file->fileTransfer);
            continue;
        }
        case STATE::ERR: {
#if  LIBSSH_VERSION_INT >= SSH_VERSION_INT(0, 11, 0)
            foreach (auto aio, file->aio) {
                sftp_aio_free(aio);
            }
#endif
            CleanFileAIO(file);

            it = m_vFiles.erase(it);
            file->fileTransfer->slotSetstate(CFileTransfer::State::Fail);
            emit sigFileTransferUpdate(file->fileTransfer);
            continue;
        }
        }

        it++;
    }
    return 0;
}

int CChannelSFTP::CleanFileAIO(QSharedPointer<_AFILE> file)
{
    if(file->buffer) {
        delete []file->buffer;
        file->buffer = nullptr;
    }
    if(file->remote) {
        sftp_close(file->remote);
        file->remote == nullptr;
    }
    if(-1 != file->local) {
        ::close(file->local);
        file->local = -1;
    }
    return 0;
}
