// Author: Kang Lin <kl222@126.com>

#ifndef CPARAMETERSERVICETIGERVNC_H
#define CPARAMETERSERVICETIGERVNC_H

#include "ParameterService.h"

class CParameterServiceTigerVNC : public CParameterService
{
    Q_OBJECT
    Q_PROPERTY(bool Ice READ getIce WRITE setIce NOTIFY sigIceChanged)
    Q_PROPERTY(bool Socket READ GetEnableSocket WRITE SetEnableSocket NOTIFY sigEnableSocketChanged)

public:
    explicit CParameterServiceTigerVNC(QObject *parent = nullptr);
    virtual ~CParameterServiceTigerVNC();
    
    // CParameterService interface
public:
    virtual int Load(const QString& szFile = QString());
    virtual int Save(const QString& szFile = QString());
    
    bool getIce() const;
    void setIce(bool newBIce);

    bool GetEnableSocket() const;
    void SetEnableSocket(bool newSocket);

signals:
    void sigIceChanged();
    void sigEnableSocketChanged(bool bSocket);
    
private:
    bool m_bIce;
    bool m_bEnableSocket;
};

#endif // CPARAMETERSERVICETIGERVNC_H
