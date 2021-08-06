#ifndef CPARAMETERSERVICE_H
#define CPARAMETERSERVICE_H

#include <QDataStream>

class CParameterService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint16 Port READ getPort WRITE setPort NOTIFY PortChanged)
    
public:
    explicit CParameterService(QObject *parent = nullptr);
    
    virtual int OnLoad(QDataStream& d);
    virtual int OnSave(QDataStream& d);
    
    quint16 getPort() const;
    void setPort(quint16 newPort);
    
signals:
    void PortChanged();
    
private:
    quint16 m_nPort;
};

#endif // CPARAMETERSERVICE_H
