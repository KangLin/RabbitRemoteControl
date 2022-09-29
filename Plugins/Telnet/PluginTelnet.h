#ifndef CPLUGINTELNET_H_KL_2021_07_23
#define CPLUGINTELNET_H_KL_2021_07_23

#pragma once

#include "PluginClient.h"
#include <QTranslator>

class CPluginTelnet : public CPluginClient
{
    Q_OBJECT
public:
    explicit CPluginTelnet(QObject *parent = nullptr);
    virtual ~CPluginTelnet();
    
    Q_INTERFACES(CPluginClient)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID CPluginClient_iid)
#endif

    // CPluginFactory interface
public:
    virtual const QString Protocol() const override;
    virtual const QString Name() const override;
    virtual const QString Description() const override;
    virtual const QIcon Icon() const override;
private:
    virtual CConnecter *CreateConnecter(const QString &szProtocol) override;
};

#endif // CPLUGINTELNET_H_KL_2021_07_23
