// Author: Kang Lin <kl222@126.com>

#pragma once
#include "ParameterOperate.h"

class CParameterTemplateBase : public CParameterOperate
{
    Q_OBJECT
public:
    explicit CParameterTemplateBase(QObject *parent = nullptr,
                                    const QString& szPrefix = QString());

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};
