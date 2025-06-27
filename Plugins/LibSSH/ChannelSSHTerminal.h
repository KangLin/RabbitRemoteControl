// Author: Kang Lin <kl222@126.com>

#pragma once

#include "ChannelSSH.h"
#include "qtermwidget.h"

class CChannelSSHTerminal : public CChannelSSH
{
    Q_OBJECT
public:
    explicit CChannelSSHTerminal(CBackend *pBackend, CParameterSSH* pPara);
    virtual ~CChannelSSHTerminal();

    virtual int OnProcess(int timeout);
    void SetSize(int row, int column);

private:
    int m_nRow;
    int m_nColumn;
    // QIODevice interface
protected:
    virtual qint64 readData(char *data, qint64 maxlen) override;
    virtual qint64 writeData(const char *data, qint64 len) override;

    // CChannelSSH interface
private:
    virtual int OnOpen(ssh_session session) override;
    virtual void OnClose() override;
};
