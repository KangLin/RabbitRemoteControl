// Author: Kang Lin <kl222@126.com>

#pragma once
#include "ParameterServer.h"

class CParameterFreeRDPServer : public CParameterServer
{
    Q_OBJECT
public:
    explicit CParameterFreeRDPServer(QObject *parent = nullptr,
                                    const QString& szPrefix = QString());

public:
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

protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};
