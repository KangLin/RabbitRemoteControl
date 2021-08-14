#ifndef CPARAMETERSERVICELIBVNC_H
#define CPARAMETERSERVICELIBVNC_H

#include "ParameterService.h"

class CParameterServiceLibVNC : public CParameterService
{
    Q_OBJECT
    
public:
    explicit CParameterServiceLibVNC(QObject *parent = nullptr);
    
};

#endif // CPARAMETERSERVICELIBVNC_H
