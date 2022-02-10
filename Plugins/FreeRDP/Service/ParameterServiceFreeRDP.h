#ifndef CPARAMETERSERVICEFREERDP_H
#define CPARAMETERSERVICEFREERDP_H

#include "ParameterService.h"

class CParameterServiceFreeRDP : public CParameterService
{
    Q_OBJECT
public:
    explicit CParameterServiceFreeRDP(QObject *parent = nullptr);
};

#endif // CPARAMETERSERVICEFREERDP_H
