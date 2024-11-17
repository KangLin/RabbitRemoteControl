// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTERTIGERVNC_H_2024_07_29
#define CCONNECTERTIGERVNC_H_2024_07_29

#pragma once

#include "ConnecterThread.h"
#include "ParameterVnc.h"

class CConnecterVnc : public CConnecterThread
{
    Q_OBJECT

public:
    explicit CConnecterVnc(CPluginClient *plugin);
    virtual ~CConnecterVnc() override;

    virtual const QString Id() override;
    virtual qint16 Version() override;
    virtual int Initial() override;
    virtual int Clean() override;

protected:
    virtual QDialog* OnOpenDialogSettings(QWidget* parent = nullptr) override;
    virtual CConnect* InstanceConnect() override;
    virtual QString ServerName() override;

private:
    CParameterVnc m_Para;
};

#endif // CCONNECTERTIGERVNC_H_2024_07_29
