// Author: Kang Lin <kl222@126.com>

#pragma once
#include "ParameterBase.h"

class CParameterTemplateDesktop : public CParameterDesktop
{
    Q_OBJECT
public:
    explicit CParameterTemplateDesktop(CParameterOperate *parent = nullptr,
                                    const QString& szPrefix = QString());

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};
