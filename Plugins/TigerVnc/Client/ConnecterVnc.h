// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTERTIGERVNC_H
#define CCONNECTERTIGERVNC_H

#pragma once

#include "ConnecterDesktop.h"
#include "ParameterVnc.h"

class CConnecterVnc : public CConnecterDesktopThread
{
    Q_OBJECT

public:
    explicit CConnecterVnc(CPluginClient *parent);
    virtual ~CConnecterVnc() override;

    virtual const QString Id() override;
    virtual qint16 Version() override;

protected:
    virtual QDialog* OnOpenDialogSettings(QWidget* parent = nullptr) override;
    virtual CConnect* InstanceConnect() override;
    virtual QString ServerName() override;

private:
    CParameterVnc m_Para;
};

#endif // CCONNECTERTIGERVNC_H
