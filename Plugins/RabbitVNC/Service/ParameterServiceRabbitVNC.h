// Author: Kang Lin <kl222@126.com>

#ifndef CPARAMETERSERVICETIGERVNC_H
#define CPARAMETERSERVICETIGERVNC_H

#include "ParameterService.h"

class CParameterServiceRabbitVNC : public CParameterService
{
    Q_OBJECT
    Q_PROPERTY(bool Ice READ getIce WRITE setIce NOTIFY sigIceChanged)
    Q_PROPERTY(QString SignalServer READ getSignalServer WRITE setSignalServer NOTIFY sigSignalServerChanged)
    Q_PROPERTY(quint16 SignalPort READ getSignalPort WRITE setSignalPort NOTIFY sigSignalPortChanged)
    Q_PROPERTY(QString SignalUser READ getSignalUser WRITE setSignalUser NOTIFY sigSignalUserChanged)
    Q_PROPERTY(QString SignalPassword READ getSignalPassword WRITE setSignalPassword NOTIFY sigSignalPasswordChanged)
    Q_PROPERTY(QString StunServer READ getStunServer WRITE setStunServer NOTIFY sigStunServerChanged)
    Q_PROPERTY(quint16 StunPort READ getStunPort WRITE setStunPort NOTIFY sigStunPortChanged)
    Q_PROPERTY(QString TurnServer READ getTurnServer WRITE setTurnServer NOTIFY sigTurnServerChanged)
    Q_PROPERTY(quint16 TurnPort READ getTurnPort WRITE setTurnPort NOTIFY sigTurnPortChanged)
    Q_PROPERTY(QString TurnUser READ getTurnUser WRITE setTurnUser NOTIFY sigTurnUserChanged)
    Q_PROPERTY(QString TurnPassword READ getTurnPassword WRITE setTurnPassword NOTIFY sigTurnPasswordChanged)

public:
    explicit CParameterServiceRabbitVNC(QObject *parent = nullptr);
    virtual ~CParameterServiceRabbitVNC();
    
    // CParameterService interface
public:
    virtual int Load(const QString& szFile = QString());
    virtual int Save(const QString& szFile = QString());
    
    bool getIce() const;
    void setIce(bool newBIce);
    const QString &getSignalServer() const;
    void setSignalServer(const QString &newSzSignalServer);
    quint16 getSignalPort() const;
    void setSignalPort(quint16 newNSignalPort);
    const QString &getSignalUser() const;
    void setSignalUser(const QString &newSzSignalUser);
    const QString &getSignalPassword() const;
    void setSignalPassword(const QString &newSzSignalPassword);
    const QString &getStunServer() const;
    void setStunServer(const QString &newSzStunServer);
    quint16 getStunPort() const;
    void setStunPort(quint16 newNStunPort);
    const QString &getTurnServer() const;
    void setTurnServer(const QString &newSzTurnServer);
    quint16 getTurnPort() const;
    void setTurnPort(quint16 newNTurnPort);
    const QString &getTurnUser() const;
    void setTurnUser(const QString &newSzTurnUser);
    const QString &getTurnPassword() const;
    void setTurnPassword(const QString &newSzTurnPassword);
    
signals:
    void sigIceChanged();
    void sigSignalServerChanged();
    void sigSignalPortChanged();
    void sigSignalUserChanged();
    void sigSignalPasswordChanged();
    void sigStunServerChanged();
    void sigStunPortChanged();
    void sigTurnServerChanged();
    void sigTurnPortChanged();
    void sigTurnUserChanged();
    void sigTurnPasswordChanged();
    
private:
    bool m_bIce;
    QString m_szSignalServer;
    quint16 m_nSignalPort;
    QString m_szSignalUser, m_szSignalPassword;
    QString m_szStunServer;
    quint16 m_nStunPort;
    QString m_szTurnServer;
    quint16 m_nTurnPort;
    QString m_szTurnUser, m_szTurnPassword;
};

#endif // CPARAMETERSERVICETIGERVNC_H
