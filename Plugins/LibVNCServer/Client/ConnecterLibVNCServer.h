// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTERLIBVNC_H
#define CCONNECTERLIBVNC_H

#pragma once

#include "ConnecterDesktopThread.h"
#include "ParameterLibVNCServer.h"

class CConnecterLibVNCServer : public CConnecterDesktopThread
{
    Q_OBJECT
public:
    explicit CConnecterLibVNCServer(CPluginClient *parent);
    virtual ~CConnecterLibVNCServer() override;

public:
    virtual qint16 Version() override;

protected:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;
    virtual CConnect* InstanceConnect() override;
    
private:
    CParameterLibVNCServer m_Para;
};

#endif // CCONNECTERLIBVNC_H
