#ifndef CPLUGINLIBSSH_H_KL_2021_07_23
#define CPLUGINLIBSSH_H_KL_2021_07_23

#pragma once

#include "PluginClient.h"
#include <QTranslator>

class CPluginLibSSH : public CPluginClient
{
    Q_OBJECT
    Q_INTERFACES(CPluginClient)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID CPluginClient_iid)
#endif
    
public:
    explicit CPluginLibSSH();
    virtual ~CPluginLibSSH();

public:
    virtual const QString Protocol() const override;
    virtual const QString Name() const override;
    virtual const QString Description() const override;
    virtual const QIcon Icon() const override;

private:
    virtual CConnecter *CreateConnecter(const QString &szProtocol) override;
};

#endif // CPLUGINLIBSSH_H_KL_2021_07_23
