#ifndef CDISPLY_H
#define CDISPLY_H

#include <QObject>

class CDisply : public QObject
{
    Q_OBJECT
public:
    explicit CDisply(QObject *parent = nullptr);
    
signals:
    
};

#endif // CDISPLY_H
