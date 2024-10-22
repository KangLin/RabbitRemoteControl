// Author: Kang Lin <kl222@126.com>

#ifndef PLUGINSCREENCAPTURE_H
#define PLUGINSCREENCAPTURE_H

#pragma once
#include "PluginClient.h"

class CPluginScreenCapture : public CPluginClient
{
    Q_OBJECT
public:
    explicit CPluginScreenCapture(QObject *parent = nullptr);

    Q_INTERFACES(CPluginClient)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID CPluginClient_iid)
#endif

    // CPluginClient interface
public:
    virtual const QString Protocol() const override;
    virtual const QString Name() const override;
    virtual const QString DisplayName() const override;
    virtual const QString Description() const override;
    virtual const QString Version() const override;
    virtual const QIcon Icon() const override;

private:
    virtual CConnecter *CreateConnecter(const QString &szId) override;
};

#endif // PLUGINSCREENCAPTURE_H
