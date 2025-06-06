// Author: Kang Lin <kl222@126.com>

#ifndef PLUGINPLAYER_H
#define PLUGINPLAYER_H

#include "PluginClient.h"

class CPluginPlayer : public CPluginClient
{
    Q_OBJECT
public:
    explicit CPluginPlayer(QObject *parent = nullptr);

    // [Qt plugin interface]
    Q_INTERFACES(CPluginClient)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID CPluginClient_iid)
#endif
    // [Qt plugin interface]

    // CPluginClient interface
public:
    virtual const TYPE Type() const override;
    virtual const QString Protocol() const override;
    virtual const QString Name() const override;
    virtual const QString DisplayName() const override;
    virtual const QString Description() const override;
    virtual const QString Version() const override;
    virtual const QIcon Icon() const override;
    virtual const QString Details() const override;

private:
    virtual CConnecter *OnCreateConnecter(const QString &szId) override;

};

#endif // PLUGINPLAYER_H
