// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "ChannelSFTP.h"
#include "BackendFileTransfer.h"
#include "RemoteFileSystemModel.h"
#include "ListFileModel.h"

#if defined(Q_OS_LINUX)
#include <sys/stat.h>
#endif
#ifdef _MSC_VER
#define S_IRUSR  0400  // Owner read permission
#define S_IWUSR  0200  // Owner write permission
#define S_IXUSR  0100  // Owner execute permission
#define S_IRWXU  (S_IRUSR | S_IWUSR | S_IXUSR)  // Owner read, write, execute
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

    nRet = AsyncReadDir();
    if(SSH_OK != nRet)
        return -1;

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
    p->SetPermissions((CRemoteFileSystem::Permissions)attributes->permissions);
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
    int nRet = SSH_OK;
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
            nRet = sftp_closedir(d->sftp);
            if(SSH_OK == nRet) {
                d->state = STATE::FINISH;
                d->sftp = nullptr;
                break;
            }
            int err = ssh_get_error_code(m_Session);
            if(err == SSH_REQUEST_DENIED)
                break;
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

        if(SSH_ERROR == nRet)
            break;
        it++;
    }

    return nRet;
}

void CChannelSFTP::slotStartFileTransfer(QSharedPointer<CFileTransfer> f)
{
    f->slotSetstate(CFileTransfer::State::Connecting);
    QSharedPointer<FILE_AIO> file(new FILE_AIO);
    file->fileTransfer = f;
    file->local = -1;
    file->remote = nullptr;
    file->state = STATE::OPEN;
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
