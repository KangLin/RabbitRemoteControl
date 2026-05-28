// Author: Kang Lin <kl222@126.com>

#pragma once
#include "ParameterServer.h"

class CParameterTemplateServer : public CParameterServer
{
    Q_OBJECT
public:
    explicit CParameterTemplateServer(QObject *parent = nullptr,
                                    const QString& szPrefix = QString());

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};
