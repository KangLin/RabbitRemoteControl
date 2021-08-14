#ifndef CPARAMETERSERVICETIGERVNC_H
#define CPARAMETERSERVICETIGERVNC_H

#include "ParameterService.h"

class CParameterServiceTigerVNC : public CParameterService
{
    Q_OBJECT
    
    
public:
    explicit CParameterServiceTigerVNC(QObject *parent = nullptr);
    
    // CParameterService interface
public:
    virtual int OnLoad(const QString& szFile = QString());
    virtual int OnSave(const QString& szFile = QString());
};

#endif // CPARAMETERSERVICETIGERVNC_H
