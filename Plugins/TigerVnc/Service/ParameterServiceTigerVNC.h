// Author: Kang Lin <kl222@126.com>

#ifndef CPARAMETERSERVICETIGERVNC_H
#define CPARAMETERSERVICETIGERVNC_H

#include "ParameterService.h"

class CParameterServiceTigerVNC : public CParameterService
{
    Q_OBJECT
    Q_PROPERTY(bool Ice READ getIce WRITE setIce NOTIFY sigIceChanged)

public:
    explicit CParameterServiceTigerVNC(QObject *parent = nullptr);
    virtual ~CParameterServiceTigerVNC();
    
    // CParameterService interface
public:
    virtual int Load(const QString& szFile = QString());
    virtual int Save(const QString& szFile = QString());
    
    bool getIce() const;
    void setIce(bool newBIce);

signals:
    void sigIceChanged();
    
private:
    bool m_bIce;
};

#endif // CPARAMETERSERVICETIGERVNC_H
