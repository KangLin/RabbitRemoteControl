// Author: Kang Lin <kl222@126.com>

#ifndef CDATACHANNEL_H
#define CDATACHANNEL_H

#pragma once

#include <QLoggingCategory>
#include <QIODevice>
#include <QTcpSocket>
#include "channel_export.h"

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

private Q_SLOTS:
    void slotError(QAbstractSocket::SocketError e);
    void slotConnected();
    void slotDisconnected();

private:
    QLoggingCategory m_Log;
    QTcpSocket* m_pSocket;
    
    // QIODevice interface
protected:
    virtual qint64 readData(char *data, qint64 maxlen) override;
    virtual qint64 writeData(const char *data, qint64 len) override;
    virtual bool isSequential() const override;
};

#endif // CDATACHANNEL_H
