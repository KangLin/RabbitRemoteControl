#ifndef CPARAMETERSERVICETIGERVNC_H
#define CPARAMETERSERVICETIGERVNC_H

#include "ParameterService.h"

class CParameterServiceTigerVNC : public CParameterService
{
    Q_OBJECT
    Q_PROPERTY(QString Password READ getPassword WRITE setPassword NOTIFY PasswordChanged)
    
public:
    explicit CParameterServiceTigerVNC(QObject *parent = nullptr);
    
    // CParameterService interface
public:
    virtual int OnLoad(QDataStream &d) override;
    virtual int OnSave(QDataStream &d) override;
    virtual int OnLoad(const QString& szFile = QString());
    virtual int OnSave(const QString& szFile = QString());
    
    const QString &getPassword() const;
    void setPassword(const QString &newPassword);
    
signals:
    void PasswordChanged();
    
private:
    QString m_szPassword;
    
};

#endif // CPARAMETERSERVICETIGERVNC_H
