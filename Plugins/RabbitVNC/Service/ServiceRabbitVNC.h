// Author: Kang Lin <kl222@126.com>

#ifndef CSERVICETIGERVNC_H
#define CSERVICETIGERVNC_H

#pragma once

#include "Service.h"
#include <QSharedPointer>
#include <QTcpServer>

#ifdef HAVE_ICE
    #include "ICE/IceSignal.h"
#endif

class CConnection;
class CServiceRabbitVNC : public CService//, rfb::SDesktop
{
    Q_OBJECT
public:
    explicit CServiceRabbitVNC(CPluginService *plugin);
    virtual ~CServiceRabbitVNC();

#ifdef HAVE_GUI
    virtual QWidget* GetParameterWidget(QWidget* parent = nullptr) override;
#endif
    
    // CService interface
protected:
    virtual int OnInit() override;
    virtual int OnClean() override;
    
private Q_SLOTS:
    void slotNewConnection();
    void slotDisconnected();
    void slotError(int nErr, QString szErr);
    
private:
    QTcpServer m_Lister;
    QList<QSharedPointer<CConnection> > m_lstConnection;
    
#ifdef HAVE_ICE
private Q_SLOTS:
    void slotSignalConnected();
    void slotSignalDisConnected();
    void slotSignalError(int nErr, const QString& szErr);
    void slotSignalOffer(const QString& fromUser,
                  const QString& toUser,
                  const QString& channelId,
                  const QString& type,
                  const QString& sdp);
    
private:
    QSharedPointer<CIceSignal> m_Signal;
#endif
};

#endif // CSERVICETIGERVNC_H
