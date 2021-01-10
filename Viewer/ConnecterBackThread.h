#ifndef CCONNECTERBACKTHREAD_H
#define CCONNECTERBACKTHREAD_H

#include "Connect.h"

class CConnecterBackThread : public CConnecter
{
    Q_OBJECT
public:
    explicit CConnecterBackThread(CPluginFactory *parent = nullptr);

    virtual int OnRun();
    virtual CConnect* InstanceConnect() = 0;

private:
    bool m_bExit;

public Q_SLOTS:
    int Connect();
    int DisConnect();
};

#endif // CCONNECTERBACKTHREAD_H
