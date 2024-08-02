#ifndef CCHANNELSSHTUNNEL_H
#define CCHANNELSSHTUNNEL_H

#pragma once

#include <QSharedPointer>
#include <QSocketNotifier>
#include <QMutex>

#include "Channel.h"
#include "libssh/libssh.h"
#include "libssh/callbacks.h"
#include "ParameterChannelSSH.h"
#include "channel_export.h"

/*!
 * \~chinese
 * \brief SSH 隧道
 * 
 * \~english
 * \brief ssh tunnel class
 * 
 * \~
 * \ingroup LIBAPI_CHANNEL
 */
class CHANNEL_EXPORT CChannelSSHTunnel : public CChannel
{
    Q_OBJECT

public:
    explicit CChannelSSHTunnel(QSharedPointer<CParameterChannelSSH> parameter,
                               QObject *parent = nullptr);
    virtual ~CChannelSSHTunnel();

    // QIODevice interface
public:
    virtual bool open(OpenMode mode) override;
    virtual void close() override;
    
    int Process();
    virtual void WakeUp();

Q_SIGNALS:
    /*!
     * \~chinese
     * 阻塞后台线程，并在前台线程中显示消息对话框(QMessageBox)
     *
     * \~english
     * \brief Block background threads and display message dialogs in foreground threads (QMessageBox)
     * \param title
     * \param message
     * \param buttons
     * \param nRet
     * \param checkBox
     * \param checkBoxContext
     * 
     * \~
     * \see CConnecter::slotBlockShowMessageBox()
     */
    void sigBlockShowMessageBox(const QString& szTitle,
                                const QString& szMessage,
                                QMessageBox::StandardButtons buttons,
                                QMessageBox::StandardButton& nRet,
                                bool &checkBox,
                                QString checkBoxContext = QString());

    // QIODevice interface
protected:
    virtual qint64 readData(char *data, qint64 maxlen) override;
    virtual qint64 writeData(const char *data, qint64 len) override;

private:
    int verifyKnownhost(ssh_session session);
    int authentication(
        ssh_session session,
        const QString szUser,
        const QString szPassword,
        const QString szPassphrase,
        const int nMethod = SSH_AUTH_METHOD_PASSWORD);
    int authenticationPublicKey(
        ssh_session session,
        const QString szUser,
        const QString szPublicKeyFile,
        const QString szPrivateKeyFile,
        const QString szPassphrase);
    int forward(ssh_session session);

    static void cb_log(ssh_session session,
                       int priority,
                       const char *message,
                       void *userdata);
    
    int InitSemaphore();
    int ReadSemphore();
    int ClearSemphore();

private:
    ssh_session m_Session;
    ssh_channel m_Channel;
    ssh_pcap_file m_pcapFile;
    QSharedPointer<CParameterChannelSSH> m_Parameter;
    
    QSocketNotifier* m_pSocketRead;
    QSocketNotifier* m_pSocketWrite;
    QSocketNotifier* m_pSocketException;
    
    QByteArray m_readData;
    int m_eventWriteFD;

};

#endif // CCHANNELSSHTUNNEL_H
