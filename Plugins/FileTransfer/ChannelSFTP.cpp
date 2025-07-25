// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "ChannelSFTP.h"
#include "BackendFieTransfer.h"
#include "RemoteFileSystemModel.h"

static Q_LOGGING_CATEGORY(log, "Channel.SFTP")

CChannelSFTP::CChannelSFTP(CBackend *pBackend, CParameterSSH *pPara,
                           bool bWakeUp, QObject *parent)
    : CChannelSSH(pBackend, pPara, bWakeUp, parent)
    , m_SessionSftp(nullptr)
{
    bool check = false;
    CBackendFieTransfer* pB = qobject_cast<CBackendFieTransfer*>(pBackend);
    if(pB) {
        check = connect(this, SIGNAL(sigGetFolder(CRemoteFileSystem*, QVector<QSharedPointer<CRemoteFileSystem> >, bool)),
                        pB, SIGNAL(sigGetFolder(CRemoteFileSystem*, QVector<QSharedPointer<CRemoteFileSystem> >, bool)));
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
    socket_t fd = SSH_INVALID_SOCKET;
    if(m_pEvent)
        fd = m_pEvent->GetFd();
    if(SSH_INVALID_SOCKET != fd)
        FD_SET(fd, &set);

    //qDebug(log) << "ssh_select:" << fd;
    nRet = ssh_select(channels, channel_out, fd + 1, &set, &timeout);
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

    if(SSH_INVALID_SOCKET != fd && FD_ISSET(fd, &set)) {
        //qDebug(log) << "fires event";
        if(m_pEvent) {
            nRet = m_pEvent->Reset();
            if(nRet) return -4;
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
    qDebug(log) << szPath << "name:" << attributes->name
                << "size:" << attributes->size << "type:" << attributes->type;
    QString szName(attributes->name);
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

int CChannelSFTP::GetFolder(CRemoteFileSystem* p)
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
        qCritical(log) << "Directory not opened:" << ssh_get_error(m_Session);
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
        qCritical(log) << "Can't list directory:" << ssh_get_error(m_Session);
        sftp_closedir(dir);
        return SSH_ERROR;
    }

    nRet = sftp_closedir(dir);
    if (nRet != SSH_OK)
    {
        qCritical(log) << "Can't close directory:" << ssh_get_error(m_Session);
        return nRet;
    }

    emit sigGetFolder(p, vFileNode, true);

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
        qCritical(log) << "Error allocating SFTP session:" << ssh_get_error(session);
        return SSH_ERROR;
    }

    nRet = sftp_init(m_SessionSftp);
    if (SSH_OK != nRet)
    {
        qCritical(log) << "Error initializing SFTP session:" << sftp_get_error(m_SessionSftp);
        sftp_free(m_SessionSftp);
        m_SessionSftp = nullptr;
        return nRet;
    }

    return nRet;
}

void CChannelSFTP::OnClose()
{
    if(m_SessionSftp) {
        sftp_free(m_SessionSftp);
        m_SessionSftp = nullptr;
    }
    m_vDirs.clear();
}

void CChannelSFTP::slotGetFolder(CRemoteFileSystem *p)
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
            qCritical(log) << "Error opening directory:" << d->szPath << "Error:" << ssh_get_error(m_Session);
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
            qCritical(log) << "Error reading directory:" << d->szPath << err << ssh_get_error(m_Session);
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
            qCritical(log) << "Error close directory:" << d->szPath << err << ssh_get_error(m_Session);
            d->state = STATE::FINISH;
            d->Error = err;
            break;
        }
        case STATE::FINISH:
            if(d && STATE::FINISH == d->state) {
                qDebug(log) << "Remote" << d->szPath << d->vFileNode.size();
                emit sigGetFolder(d->remoteFileSystem, d->vFileNode, true);
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
