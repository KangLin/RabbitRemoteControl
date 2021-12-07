#ifndef CPARAMETERSSH_H
#define CPARAMETERSSH_H

#include "ParameterTerminal.h"

class CParameterSSH : public CParameterTerminal
{
    Q_OBJECT

public:
    CParameterSSH();
    
    QString captrueFile;
};

#endif // CPARAMETERSSH_H
