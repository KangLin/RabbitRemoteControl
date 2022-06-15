// Author: Kang Lin <kl222@126.com>

#ifndef CPARAMETERSERVICEFREERDP_H
#define CPARAMETERSERVICEFREERDP_H

#include "ParameterService.h"

class CParameterServiceFreeRDP : public CParameterService
{
    Q_OBJECT
    
public:
    explicit CParameterServiceFreeRDP(QObject *parent = nullptr);
    
    virtual int Load(const QString &szFile) override;
    virtual int Save(const QString &szFile) override;
    
    bool getTlsSecurity() const;
    void setTlsSecurity(bool newTlsSecurity);
    bool getRdpSecurity() const;
    void setRdpSecurity(bool newRdpSecurity);
    bool getNlaSecurity() const;
    void setNlaSecurity(bool newNlaSecurity);
    bool getNlaExtSecurity() const;
    void setNlaExtSecurity(bool newNlaExtSecurity);
    const QString &getSamFile() const;
    void setSamFile(const QString &newSamFile);
    
    bool getAuthentication() const;
    void setAuthentication(bool newAuthentication);
    
    bool getMayView() const;
    void setMayView(bool newMayView);
    bool getMayInteract() const;
    void setMayInteract(bool newMayInteract);
    
Q_SIGNALS:
    void sigTlsSecurityChanged();
    void sigRdpSecurityChanged();
    void sigNlaecurityChanged();
    void sigNlaExtSecurityChanged();
    void sigSamFileChanged();
    
    void sigAuthenticationChanged();
    
    void sigMayViewChanged();
    void sigMayInteractChanged();
    
private:
    bool m_bTlsSecurity;
    bool m_bRdpSecurity;
    bool m_bNlaSecurity;
    bool m_bNlaExtSecurity;
    QString m_szSamFile;
   
    bool m_bAuthentication;
    
    bool m_MayView;
    bool m_MayInteract;

    Q_PROPERTY(bool TlsSecurity READ getTlsSecurity WRITE setTlsSecurity)
    Q_PROPERTY(bool RdpSecurity READ getRdpSecurity WRITE setRdpSecurity)
    Q_PROPERTY(bool NlaSecurity READ getNlaSecurity WRITE setNlaSecurity)
    Q_PROPERTY(bool NlaExtSecurity READ getNlaExtSecurity WRITE setNlaExtSecurity NOTIFY sigNlaExtSecurityChanged)
    Q_PROPERTY(bool Authentication READ getAuthentication WRITE setAuthentication NOTIFY sigAuthenticationChanged)
    Q_PROPERTY(QString SamFile READ getSamFile WRITE setSamFile NOTIFY sigSamFileChanged)
    Q_PROPERTY(bool MayView READ getMayView WRITE setMayView NOTIFY sigMayViewChanged)
    Q_PROPERTY(bool MayInteract READ getMayInteract WRITE setMayInteract NOTIFY sigMayInteractChanged)
};

#endif // CPARAMETERSERVICEFREERDP_H
