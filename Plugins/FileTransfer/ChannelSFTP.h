// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#pragma once
#include "ChannelSSH.h"
#include "libssh/sftp.h"
#include <QDateTime>

#define BUF_SIZE 4096

class CRemoteFileSystem;
class CFileTransfer;
class CChannelSFTP : public CChannelSSH
{
    Q_OBJECT

public:
    explicit CChannelSFTP(CBackend* pBackend, CParameterSSH* pPara,
                          bool bWakeUp = true, QObject *parent = nullptr);

    /*!
     * \brief Synchronize to get the directory
     * \param szPath
     * \return 
     */
    int GetDir(CRemoteFileSystem* p);
    int MakeDir(const QString& dir);
    int RemoveDir(const QString& dir);
    int RemoveFile(const QString& file);
    int Rename(const QString& oldPath, const QString& newPath);

    int Process();

public Q_SLOTS:
    /*!
     * \brief Get the directory asynchronously
     */
    void slotGetDir(CRemoteFileSystem *p);
Q_SIGNALS:
    void sigGetDir(CRemoteFileSystem* p, QVector<QSharedPointer<CRemoteFileSystem> > contents, bool bEnd);

public Q_SLOTS:
    void slotStartFileTransfer(QSharedPointer<CFileTransfer> f);
    void slotStopFileTransfer(QSharedPointer<CFileTransfer> f);
Q_SIGNALS:
    void sigFileTransferUpdate(QSharedPointer<CFileTransfer> f);

protected:
    virtual qint64 readData(char *data, qint64 maxlen) override;
    virtual qint64 writeData(const char *data, qint64 len) override;

private:
    enum class STATE {
        OPEN,
        READ,
        CLOSE,
        FINISH,
        ERR
    };
    struct DIR_READER {
        sftp_dir sftp;
        QString szPath;
        STATE state;
        CRemoteFileSystem* remoteFileSystem;
        QVector<QSharedPointer<CRemoteFileSystem> > vFileNode;
        int Error;
    };

    struct _AIO {
        sftp_aio aio;
        quint64 nStart;
        quint64 nRequests;
        quint64 nTransfers;
        char* buffer;
    };

    struct _AFILE {
        sftp_file remote;
        int local;
        STATE state;
        QSharedPointer<CFileTransfer> fileTransfer;

#if  LIBSSH_VERSION_INT >= SSH_VERSION_INT(0, 11, 0)
        QVector<sftp_aio> aio;
        // The chunk size to use for the transfer
        quint64 nChunkSize;
        quint64 nRequests;
        quint64 nTransfers;
        int nConcurrentCount;
        char *buffer;
#else
        int asyncReadId;
#endif
        
    };

    virtual int OnOpen(ssh_session session) override;
    virtual void OnClose() override;
    QSharedPointer<CRemoteFileSystem> GetFileNode(const QString &szPath, sftp_attributes attributes);
    int AsyncReadDir();
    int AsyncFile();
    int CleanFileAIO(QSharedPointer<_AFILE> aio);

private:
    sftp_session m_SessionSftp;
    QVector<QSharedPointer<DIR_READER> > m_vDirs;
    QVector<QSharedPointer<_AFILE> > m_vFiles;
};
