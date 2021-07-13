#ifndef CSERVICETIGERVNC_H
#define CSERVICETIGERVNC_H

#include "Service.h"

class CServiceTigerVNC : public CService
{
    Q_OBJECT
public:
    explicit CServiceTigerVNC(QObject *parent = nullptr);
    bool Enable();    

    // CService interface
protected:
    virtual int OnProcess() override;
};

#endif // CSERVICETIGERVNC_H
