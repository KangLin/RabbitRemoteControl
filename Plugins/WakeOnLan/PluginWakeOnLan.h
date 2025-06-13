// Author: Kang Lin <kl222@126.com>

#pragma once
#include "Plugin.h"

class CPluginWakeOnLan : public CPlugin
{
    Q_OBJECT
    // [Qt plugin interface]
    Q_INTERFACES(CPlugin)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID CPlugin_iid)
#endif
    // [Qt plugin interface]

public:
    explicit CPluginWakeOnLan(QObject *parent = nullptr);
    virtual ~CPluginWakeOnLan();

    // CPlugin interface
public:
    [[nodiscard]] virtual const TYPE Type() const override;
    [[nodiscard]] virtual const QString Name() const override;
    [[nodiscard]] virtual const QString DisplayName() const override;
    [[nodiscard]] virtual const QString Description() const override;
    [[nodiscard]] virtual const QString Protocol() const override;
    [[nodiscard]] virtual const QIcon Icon() const override;
    [[nodiscard]] virtual const QString Details() const override;
    [[nodiscard]] virtual const QString Version() const override;

private:
    [[nodiscard]] virtual COperate* CreateOperate(const QString& szId, CParameterPlugin* para) override;
    [[nodiscard]] virtual COperate* OnCreateOperate(const QString &szId) override;
    virtual int DeleteOperate(COperate* p) override;

    COperate* m_pOperate;
};
