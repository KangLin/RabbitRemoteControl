#ifndef CSERVICETIGERVNC_H
#define CSERVICETIGERVNC_H

#pragma once

#include <QSharedPointer>
#include <QTcpServer>
#include "Service.h"

class CConnection;
class CServiceTigerVNC : public CService//, rfb::SDesktop
{
    Q_OBJECT
public:
    explicit CServiceTigerVNC(CPluginService *plugin);
    bool Enable();    

    // CService interface
protected:
    virtual int OnProcess() override;
    virtual int OnInit() override;
    virtual int OnClean() override;
    
private Q_SLOTS:
    void slotNewConnection();

private:
    QTcpServer m_Lister;
    QList<QSharedPointer<CConnection> > m_lstConnection;
};

#endif // CSERVICETIGERVNC_H
