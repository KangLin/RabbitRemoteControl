#ifndef CPARAMETERSSH_H
#define CPARAMETERSSH_H

#pragma once

#include <QObject>
#include "ParameterConnecter.h"
#include "ParameterChannelSSH.h"

class CParameterSSH : public CParameterConnecter
{
    Q_OBJECT
public:
    explicit CParameterSSH(CParameterConnecter* parent);

    CParameterChannelSSH m_Channel;

    // CParameter interface
protected:
    virtual int onLoad(QSettings &set) override;
    virtual int onSave(QSettings &set) override;
};

#endif // CPARAMETERSSH_H
