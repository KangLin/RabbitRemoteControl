// Author: Kang Lin <kl222@126.com>

#ifndef CDATACHANNEL_H
#define CDATACHANNEL_H

#pragma once

#include <QIODevice>
#include <QTcpSocket>
#include <QMessageBox>
#include <QMutex>
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
    /*!
     * \brief CChannel
     * \param pSocket: Its owner is the caller
     * \param parent
     */
    explicit CChannel(QTcpSocket* pSocket, QObject *parent = nullptr);
    virtual ~CChannel();

    /// \~chinese
    /// \param pSocket: 所有者是这个类的实例
    /// \~english
    /// \param pSocket: The Owner is the instance of this class.
    ///
    virtual bool open(OpenMode mode) override;
    virtual void close() override;

    //! \~chinese 依赖信息
    //! \~english Depend on information
    virtual QString GetDetails();

    //! \brief Initial translation
    static int InitTranslation();
    //! \brief Remove translation
    static int RemoveTranslation();

Q_SIGNALS:
    /*!
     * \brief emit when the channel is connected.
     */
    void sigConnected();
    //! emit when the channel is disconnected
    void sigDisconnected();
    //! emit when the channel is error
    void sigError(int nErr, const QString& szErr);

private Q_SLOTS:
    void slotError(QAbstractSocket::SocketError e);

private:
    QTcpSocket* m_pSocket;

protected:
    explicit CChannel(QObject *parent = nullptr);

    // QIODevice interface
protected:
    virtual qint64 readData(char *data, qint64 maxlen) override;
    virtual qint64 writeData(const char *data, qint64 len) override;
    virtual bool isSequential() const override;
};

#endif // CDATACHANNEL_H
