#ifndef CPARAMTER_H
#define CPARAMTER_H

#include <QObject>

class CParamter : public QObject
{
    Q_OBJECT

public:
    explicit CParamter(QObject *parent = nullptr);

    QString szHost;
    qint16 nPort;
    
    QString szUser;
    QString szPassword;

};

#endif // CPARAMTER_H
