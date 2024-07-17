// Author: Kang Lin <kl222@126.com>

#ifndef CDATACHANNEL_H
#define CDATACHANNEL_H

#pragma once

#include <QIODevice>
#include <QTcpSocket>
#include <QMessageBox>
#include "channel_export.h"
#include <QMutex>

/*!
 * \~chinese
 * \brief 通道接口类。此类默认实现一个 TCP 的通道。
 *
 * \~english
 * \brief The channel interface class. This class implements a TCP channel by default.
 * 
 * \~
 * \ingroup LIBAPI_CHANNEL
 */
class CHANNEL_EXPORT CChannel : public QIODevice
{
    Q_OBJECT
    
public:
    explicit CChannel(QObject *parent = nullptr);
    virtual ~CChannel();
    
    /// \~chinese
    /// \param pSocket: 所有者是这个类的实例
    /// \~english
    /// \param pSocket: The Owner is the instance of this class.
    /// 
    virtual bool open(QTcpSocket* pSocket, OpenMode mode);
    virtual void close() override;

Q_SIGNALS:
    void sigConnected();
    void sigDisconnected();
    void sigError(int nErr, const QString& szErr);

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

private Q_SLOTS:
    void slotError(QAbstractSocket::SocketError e);
    void slotConnected();
    void slotDisconnected();
    void slotReadyRead();
    
private:    
    QTcpSocket* m_pSocket;
    
protected:
    virtual int WakeUp();
    QByteArray m_readData;
    QMutex m_readMutex;
    
    QByteArray m_writeData;
    QMutex m_writeMutex;
    
    // QIODevice interface
protected:
    virtual qint64 readData(char *data, qint64 maxlen) override;
    virtual qint64 writeData(const char *data, qint64 len) override;
    virtual bool isSequential() const override;
    
    // QObject interface
public:
    virtual bool event(QEvent *event) override;
};

#endif // CDATACHANNEL_H
