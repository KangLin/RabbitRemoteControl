// Author: Kang Lin <kl222@126.com>

#ifndef CDATACHANNEL_H
#define CDATACHANNEL_H

#pragma once

#include <QIODevice>
#include <QTcpSocket>
#include "rdr/InStream.h"
#include "rdr/OutStream.h"

class CChannel : public QIODevice
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
    
    rdr::InStream* InStream();
    rdr::OutStream* OutStream();
    
Q_SIGNALS:
    void sigConnected();
    void sigDisconnected();
    void sigError(int nErr, const QString& szErr);

private Q_SLOTS:
    void slotError(QAbstractSocket::SocketError e);
    void slotConnected();
    void slotDisconnected();

private:
    QTcpSocket* m_pSocket;
    rdr::InStream* m_pInStream;
    rdr::OutStream* m_pOutStream;
    
    // QIODevice interface
protected:
    virtual qint64 readData(char *data, qint64 maxlen) override;
    virtual qint64 writeData(const char *data, qint64 len) override;
    virtual bool isSequential() const override;
};

#endif // CDATACHANNEL_H
