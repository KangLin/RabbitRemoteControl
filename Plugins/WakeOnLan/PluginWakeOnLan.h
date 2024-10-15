// Author: Kang Lin <kl222@126.com>

#ifndef __PLUGINWAKEONLAN_H_KL_2024_10_02__
#define __PLUGINWAKEONLAN_H_KL_2024_10_02__

#pragma once
#include "PluginClient.h"

class CPluginWakeOnLan : public CPluginClient
{
    Q_OBJECT
    // [Qt plugin interface]
    Q_INTERFACES(CPluginClient)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID CPluginClient_iid)
#endif
    // [Qt plugin interface]

public:
    explicit CPluginWakeOnLan(QObject *parent = nullptr);
    virtual ~CPluginWakeOnLan();

    // CPluginClient interface
public:
    virtual const QString Protocol() const override;
    virtual const QString Name() const override;
    virtual const QString DisplayName() const override;
    virtual const QString Description() const override;
    virtual const QIcon Icon() const override;
    virtual const QString Details() const override;
    virtual const QString Version() const override;

private:
    virtual CConnecter *CreateConnecter(const QString &szId) override;

};

#endif // __PLUGINWAKEONLAN_H_KL_2024_10_02__
