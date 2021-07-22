#ifndef CDATACHANNEL_H
#define CDATACHANNEL_H

#pragma once

#include <QIODevice>
#include <QTcpSocket>
#include "rdr/InStream.h"
#include "rdr/OutStream.h"

class CDataChannel : public QIODevice
{
public:
    explicit CDataChannel(QTcpSocket* pSocket, QObject *parent = nullptr);
    virtual ~CDataChannel();
    
    rdr::InStream* InStream();
    rdr::OutStream* OutStream();
    
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
