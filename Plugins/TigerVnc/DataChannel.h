// Author: Kang Lin <kl222@126.com>

#ifndef CDATACHANNEL_H
#define CDATACHANNEL_H

#pragma once

#include <QIODevice>
#include <QTcpSocket>
#include "rdr/InStream.h"
#include "rdr/OutStream.h"

class CDataChannel : public QIODevice
{
    Q_OBJECT
    
public:
    ///
    /// \brief CDataChannel
    /// \param pSocket: The Owner is the instance of this class.
    /// \param parent
    ///
    explicit CDataChannel(QTcpSocket* pSocket, QObject *parent = nullptr);
    virtual ~CDataChannel();
    
    rdr::InStream* InStream();
    rdr::OutStream* OutStream();
    
Q_SIGNALS:
    void sigConnected();
    void sigDisconnected();
    void sigError(int nErr, QString szErr);

private Q_SLOTS:
    void slotError(QAbstractSocket::SocketError e);

private:
    QTcpSocket* m_pSocket;
    rdr::InStream* m_pInStream;
    rdr::OutStream* m_pOutStream;
    
    // QIODevice interface
protected:
    virtual qint64 readData(char *data, qint64 maxlen) override;
    virtual qint64 writeData(const char *data, qint64 len) override;
};

#endif // CDATACHANNEL_H
