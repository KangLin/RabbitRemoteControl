#ifndef CPARAMETERSIGNAL_H
#define CPARAMETERSIGNAL_H

#include <QObject>
#include <QSettings>

class CParameterSignal : public QObject
{
    Q_OBJECT
    
public:
    explicit CParameterSignal(QObject *parent = nullptr);
    
    virtual int Load(QSettings &set);
    virtual int Save(QSettings &set);
    
    bool GetIce() const;
    void SetIce(bool newIce);
    
    const QString &GetSignalServer() const;
    void SetSignalServer(const QString &newSignalServer);
    
    quint16 GetSignalPort() const;
    void SetSignalPort(quint16 newSignalPort);
    
    const QString &GetSignalUser() const;
    void SetSignalUser(const QString &newSignalUser);
    
    const QString &GetSignalPassword() const;
    void SetSignalPassword(const QString &newSignalPassword);
    
    const QString &GetPeerUser() const;
    void SetPeerUser(const QString &newPeerUser);
    
    const QString &GetStunServer() const;
    void SetStunServer(const QString &newStunServer);
    
    quint16 GetStunPort() const;
    void SetStunPort(quint16 newStunPort);
    
    const QString &GetTurnServer() const;
    void SetTurnServer(const QString &newTurnServer);
    
    quint16 GetTurnPort() const;
    void SetTurnPort(quint16 newTurnPort);
    
    const QString &GetTurnUser() const;
    void SetTurnUser(const QString &newTurnUser);
    
    const QString &GetTurnPassword() const;
    void SetTurnPassword(const QString &newTurnPassword);
    
private:
    bool m_bIce;
    QString m_szSignalServer;
    quint16 m_nSignalPort;
    QString m_szSignalUser, m_szSignalPassword, m_szPeerUser;
    QString m_szStunServer;
    quint16 m_nStunPort;
    QString m_szTurnServer;
    quint16 m_nTurnPort;
    QString m_szTurnUser, m_szTurnPassword;
};

#endif // CPARAMETERSIGNAL_H
