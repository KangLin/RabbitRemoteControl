// Author: Kang Lin <kl222@126.com>

#pragma once
#include "Plugin.h"
class CPluginWebBrowser: public CPlugin
{
    Q_OBJECT
    // [Qt plugin interface]
    Q_INTERFACES(CPlugin)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID CPlugin_iid)
#endif
    // [Qt plugin interface]

public:
    explicit CPluginWebBrowser(QObject *parent = nullptr);
    virtual ~CPluginWebBrowser();

    // CPlugin interface
public:
    virtual const TYPE Type() const override;
    virtual const QString Protocol() const override;
    virtual const QString Name() const override;
    virtual const QString DisplayName() const override;
    virtual const QString Description() const override;
    virtual const QString Version() const override;
    virtual const QIcon Icon() const override;
    virtual const QString Details() const override;
    
protected:
    virtual COperate *OnCreateOperate(const QString &szId) override;
    Q_INVOKABLE virtual QWidget* GetSettingsWidget(QWidget *parent) override;
};


