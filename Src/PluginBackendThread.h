// Author: Kang Lin <kl222@126.com>

#pragma once

#include "Plugin.h"

class CPluginThread;
/*!
 * \chinese 插件有一个后台线程，所有操作后端运行在此后台线程中
 * \english The plugin has a background thread,
 *          and all backend of the operations run on this background thread.
 * \see CPluginThread
 */
class PLUGIN_EXPORT CPluginBackendThread : public CPlugin
{
    Q_OBJECT
public:
    explicit CPluginBackendThread(QObject *parent = nullptr);
    virtual ~CPluginBackendThread();

private:
    virtual COperate* CreateOperate(const QString &szId, CParameterPlugin *para) override;

    CPluginThread* m_pThread;
};

