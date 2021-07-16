#ifndef CSERVICETIGERVNC_H
#define CSERVICETIGERVNC_H

#include "Service.h"
#include "rfb/SDesktop.h"
#include "rfb/VNCServerST.h"
#include <QSharedPointer>
#include <QTcpServer>

class CServiceTigerVNC : public CService, rfb::SDesktop
{
    Q_OBJECT
public:
    explicit CServiceTigerVNC(QObject *parent = nullptr);
    bool Enable();    

    // CService interface
protected:
    virtual int OnProcess() override;
    virtual int OnInit() override;
    virtual int OnClean() override;
    
private:
    QSharedPointer<rfb::VNCServerST> m_Server;
    QTcpServer m_Lister;
};

#endif // CSERVICETIGERVNC_H
