#ifndef CPARAMETERNET_H
#define CPARAMETERNET_H

#include <QObject>
#include "client_export.h"
#include "ParameterConnecter.h"

class CLIENT_EXPORT CParameterNet
    : public CParameterConnecter
{
    Q_OBJECT

public:
    explicit CParameterNet(CParameterConnecter* parent);
    
    const QString GetName() const;
    void SetName(const QString& szName);
    
    const QString GetServerName() const;
    void SetServerName(const QString& szName);
    
    bool GetShowServerName() const;
    void SetShowServerName(bool NewShowServerName);
    
    virtual const QString GetHost() const;
    virtual void SetHost(const QString& szHost);
    
    virtual const quint16 GetPort() const;
    virtual void SetPort(quint16 port);
    
    virtual const QString GetUser() const;
    virtual void SetUser(const QString& szUser);
    
    virtual const QString GetPassword() const;
    virtual void SetPassword(const QString& szPassword);
    
    const bool GetSavePassword() const;
    /*!
     * \brief Set save password
     * \param save
     */
    void SetSavePassword(bool save);

public:
    virtual int onLoad(QSettings &set) override;
    virtual int onSave(QSettings &set) override;

private:
    QString m_szName;
    QString m_szServerName;
    bool m_bShowServerName;
    QString m_szHost;
    quint16 m_nPort;
    
    QString m_szUser;
    QString m_szPassword;
    bool m_bSavePassword;
};

#endif // CPARAMETERNET_H
