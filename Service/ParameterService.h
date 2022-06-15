// Author: Kang Lin <kl222@126.com>

#ifndef CPARAMETERSERVICE_H
#define CPARAMETERSERVICE_H

#include <QObject>
#include "service_export.h"

class SERVICE_EXPORT CParameterService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint16 Port READ getPort WRITE setPort NOTIFY PortChanged)
    Q_PROPERTY(bool Enable READ getEnable WRITE setEnable NOTIFY EnableChanged)
    Q_PROPERTY(QString Password READ getPassword WRITE setPassword NOTIFY PasswordChanged)
    
public:
    explicit CParameterService(QObject *parent = nullptr);
    virtual ~CParameterService();
    
    virtual int Load(const QString& szFile = QString());
    virtual int Save(const QString& szFile = QString());
    
    quint16 getPort() const;
    void setPort(quint16 newPort);
    
    bool getEnable() const;
    void setEnable(bool newEnable);
    
    const QString &getPassword() const;
    void setPassword(const QString &newPassword);
    
signals:
    void PortChanged();   
    void EnableChanged();
    void PasswordChanged();
    
private:
    quint16 m_nPort;
    bool m_bEnable;
    QString m_szPassword;
};

#endif // CPARAMETERSERVICE_H
