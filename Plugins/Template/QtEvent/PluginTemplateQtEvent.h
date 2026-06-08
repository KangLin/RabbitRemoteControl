// Author: Kang Lin <kl222@126.com>

#pragma once
#include "PluginBackendThread.h"

/*!
 * \brief The plugin has a background thread,
 *        and all backend of the operations run on this background thread.
 */
class CPluginTemplateQtEvent : public CPluginBackendThread
{
    Q_OBJECT

    Q_INTERFACES(CPlugin)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID CPlugin_iid)
#endif

public:
    explicit CPluginTemplateQtEvent(QObject *parent = nullptr);
    virtual ~CPluginTemplateQtEvent();

    // CPlugin interface
public:
    virtual const TYPE Type() const override;
    virtual const QString Protocol() const override;
    virtual const QString Name() const override;
    virtual const QString Description() const override;
    virtual const QString Version() const override;
    virtual const QIcon Icon() const override;
    virtual const QString Details() const override;

protected:
    virtual COperate *OnCreateOperate(const QString &szId) override;
};
