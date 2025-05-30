#ifndef CONNECTLAYERQTCPSOCKET_H
#define CONNECTLAYERQTCPSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include "ConnectLayer.h"

class CConnectLayerQTcpSocket : public CConnectLayer
{
    Q_OBJECT
public:
    explicit CConnectLayerQTcpSocket(CConnectFreeRDP* connect = nullptr);
    virtual ~CConnectLayerQTcpSocket();

private Q_SLOTS:
    void slotError(QAbstractSocket::SocketError e);
    void slotConnected();
    void slotReadyRead();

private:
    virtual int OnInit(rdpContext *context) override;
    virtual int OnClean() override;
    virtual int OnLayerRead(void *data, int bytes) override;
    virtual int OnLayerWrite(const void *data, int bytes) override;
    virtual BOOL OnLayerWait(BOOL waitWrite, DWORD timeout) override;
    virtual HANDLE OnLayerGetEvent() override;

private:
    QTcpSocket m_TcpSocket;
    HANDLE m_hSocket;
};

#endif // CONNECTLAYERQTCPSOCKET_H
