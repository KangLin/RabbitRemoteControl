#ifndef CSERVICEFREERDP_H_KL_2022_01_27_
#define CSERVICEFREERDP_H_KL_2022_01_27_

#pragma once

#include "Service.h"

class CServiceFreeRDP : public CService
{
    Q_OBJECT
public:
    explicit CServiceFreeRDP();
    virtual ~CServiceFreeRDP();
    
};

#endif // CSERVICEFREERDP_H_KL_2022_01_27_
