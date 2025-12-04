// Author: Kang Lin <kl222@126.com>

#pragma once

#include "Parameter.h"
#include "FrmMediaDevices.h"

class PLUGIN_EXPORT CParameterMediaDevices : public CParameter
{
    Q_OBJECT
public:
    explicit CParameterMediaDevices(QObject *parent = nullptr,
                                    const QString& szPrefix = QString());
    virtual CParameterMediaDevices& operator=(const CParameterMediaDevices &in);
    CFrmMediaDevices::CParameter m_Para;

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};
