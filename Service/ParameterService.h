#ifndef CPARAMETERSERVICE_H
#define CPARAMETERSERVICE_H

#include <QDataStream>
#include "service_export.h"

class SERVICE_EXPORT CParameterService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint16 Port READ getPort WRITE setPort NOTIFY PortChanged)
    Q_PROPERTY(bool Enable READ getEnable WRITE setEnable NOTIFY EnableChanged)
    
public:
    explicit CParameterService(QObject *parent = nullptr);
    
    virtual int OnLoad(QDataStream& d);
    virtual int OnSave(QDataStream& d);
    virtual int OnLoad(const QString& szFile = QString());
    virtual int OnSave(const QString& szFile = QString());
    
    quint16 getPort() const;
    void setPort(quint16 newPort);
    
    bool getEnable() const;
    void setEnable(bool newEnable);
    
signals:
    void PortChanged();
    
    void EnableChanged();
    
private:
    quint16 m_nPort;
    bool m_bEnable;
    
};

#endif // CPARAMETERSERVICE_H
