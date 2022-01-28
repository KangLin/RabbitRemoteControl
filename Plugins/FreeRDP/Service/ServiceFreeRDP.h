#ifndef CSERVICEFREERDP_H_KL_2022_01_27_
#define CSERVICEFREERDP_H_KL_2022_01_27_

#pragma once

#include "Service.h"
#include "PluginServiceFreeRDP.h"

class CServiceFreeRDP : public CService
{
    Q_OBJECT
public:
    explicit CServiceFreeRDP(CPluginServiceFreeRDP *plugin);
    virtual ~CServiceFreeRDP();

protected:
    virtual int OnInit() override;
    virtual int OnClean() override;
    /// \return
    /// \li < 0: error or stop
    /// \li >=0: Call interval 
    virtual int OnProcess() override;
};

#endif // CSERVICEFREERDP_H_KL_2022_01_27_
