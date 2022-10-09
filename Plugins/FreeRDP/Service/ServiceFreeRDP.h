// Author: Kang Lin <kl222@126.com>

#ifndef CSERVICEFREERDP_H_KL_2022_01_27_
#define CSERVICEFREERDP_H_KL_2022_01_27_

#pragma once

#include <QLoggingCategory>
#include "Service.h"
#include "PluginServiceFreeRDP.h"
#include <freerdp/server/shadow.h>

class CServiceFreeRDP : public CService
{
    Q_OBJECT
public:
    explicit CServiceFreeRDP(CPluginServiceFreeRDP *plugin);
    virtual ~CServiceFreeRDP();

#ifdef HAVE_GUI
    virtual QWidget* GetParameterWidget(QWidget* parent = nullptr) override;
#endif

protected:
    virtual int OnInit() override;
    virtual int OnClean() override;

private:
    QLoggingCategory m_Log;
    rdpSettings* m_pSettings;
	rdpShadowServer* m_pServer;
    bool m_bServerInit;

    int SetParameters();
};

#endif // CSERVICEFREERDP_H_KL_2022_01_27_
