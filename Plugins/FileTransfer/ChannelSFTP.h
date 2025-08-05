// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#pragma once
#include "ChannelSSH.h"
#include "libssh/sftp.h"
#include <QDateTime>

class CRemoteFileSystem;
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
private:
Q_SIGNALS:
    void sigGetDir(CRemoteFileSystem* p, QVector<QSharedPointer<CRemoteFileSystem> > contents, bool bEnd);

protected:
    virtual qint64 readData(char *data, qint64 maxlen) override;
    virtual qint64 writeData(const char *data, qint64 len) override;

private:
    virtual int OnOpen(ssh_session session) override;
    virtual void OnClose() override;
    QSharedPointer<CRemoteFileSystem> GetFileNode(const QString &szPath, sftp_attributes attributes);
    int AsyncReadDir();

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

private:
    sftp_session m_SessionSftp;
    QVector<QSharedPointer<DIR_READER> > m_vDirs;
    QVector<sftp_file> m_vFiles;
};
