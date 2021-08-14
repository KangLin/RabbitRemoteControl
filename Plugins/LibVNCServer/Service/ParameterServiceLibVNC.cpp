#include "ParameterServiceLibVNC.h"

CParameterServiceLibVNC::CParameterServiceLibVNC(QObject *parent) : CParameterService(parent)
{
    setPort(5900);
    setEnable(false);
}
