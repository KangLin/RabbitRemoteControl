// Author: Kang Lin <kl222@126.com>

#ifndef CSERVICETHREAD_H
#define CSERVICETHREAD_H

#include <QThread>
#include "PluginService.h"

class CServiceThread : public QThread
{
    Q_OBJECT
public:
    explicit CServiceThread(CPluginService* pPlugin, QObject *parent = nullptr);
    virtual ~CServiceThread();

protected:
    virtual void run() override;

private:
    CPluginService* m_pPlugin;
};

#endif // CSERVICETHREAD_H
