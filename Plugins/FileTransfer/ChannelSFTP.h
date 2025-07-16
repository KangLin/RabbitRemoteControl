// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#pragma once
#include "ChannelSSH.h"
#include "RemoteFileSystemModel.h"
#include "libssh/sftp.h"

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
    int GetFolder(const QString& szPath);

    int Process();
    
public Q_SLOTS:
    /*!
     * \brief Get the directory asynchronously
     * \param szPath
     */
    void slotGetFolder(const QString& szPath);

private:
Q_SIGNALS:
    void sigGetFolder(const QString& szPath,
                      QVector<CRemoteFileSystem*> contents);

protected:
    virtual qint64 readData(char *data, qint64 maxlen) override;
    virtual qint64 writeData(const char *data, qint64 len) override;

private:
    virtual int OnOpen(ssh_session session) override;
    virtual void OnClose() override;
    CRemoteFileSystem* GetRemoteFileSystem(const QString &szPath, sftp_attributes attributes);
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
        QVector<CRemoteFileSystem*> vRemoteFileSystem;
    };

private:
    sftp_session m_SessionSftp;
    QVector<DIR_READER*> m_vDirs;
    QVector<sftp_file> m_vFiles;
};
