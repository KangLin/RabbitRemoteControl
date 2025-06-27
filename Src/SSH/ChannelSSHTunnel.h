// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QSharedPointer>
#include <QSocketNotifier>
#include <QMutex>

#include "ChannelSSH.h"
#include "Event.h"
#include "Backend.h"

/*!
 * \~chinese
 * \brief SSH 隧道
 * 
 * \~english
 * \brief ssh tunnel class
 * 
 * \~
 * \ingroup LIBAPI_CHANNEL
 */
class PLUGIN_EXPORT CChannelSSHTunnel : public CChannelSSH
{
    Q_OBJECT

public:
    explicit CChannelSSHTunnel(CParameterSSHTunnel* parameter,
                               CParameterNet* remote,
                               CBackend *pBackend,
                               bool bWakeUp = true,
                               QObject *parent = nullptr);
    virtual ~CChannelSSHTunnel();

    // QIODevice interface
public:
    int Process();
    virtual int WakeUp();
    virtual int DoWait(bool bWrite, int timeout);
    int GetSocket();

    // QIODevice interface
protected:
    virtual qint64 readData(char *data, qint64 maxlen) override;
    virtual qint64 writeData(const char *data, qint64 len) override;

private:
    virtual int OnOpen(ssh_session session) override;
    virtual void OnClose() override;

private:
    CParameterNet* m_pRemoteNet;
    /*
    QSocketNotifier* m_pSocketRead;
    QSocketNotifier* m_pSocketWrite;
    QSocketNotifier* m_pSocketException;
    */
    Channel::CEvent* m_pEvent;
};
