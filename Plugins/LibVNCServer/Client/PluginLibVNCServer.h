// Author: Kang Lin <kl222@126.com>

#pragma once

#include "Plugin.h"

class CPluginLibVNCServer : public CPlugin
{
    Q_OBJECT
    Q_INTERFACES(CPlugin)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID CPlugin_iid)
#endif
    
public:
    CPluginLibVNCServer(QObject *parent = nullptr);
    virtual ~CPluginLibVNCServer() override;

    // CPluginFactory interface
public:
    virtual const TYPE Type() const override;
    virtual const QString Name() const override;
    virtual const QString DisplayName() const override;
    virtual const QString Description() const override;
    virtual const QString Protocol() const override;
    virtual const QIcon Icon() const override;
    virtual const QString Details() const override;
    virtual const QString Version() const override;

private:
    virtual COperate* OnCreateOperate(const QString &szID) override;
};
