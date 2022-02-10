#include "ParameterServiceFreeRDP.h"

CParameterServiceFreeRDP::CParameterServiceFreeRDP(QObject *parent)
    : CParameterService{parent}
{
    setPort(3389);
}
