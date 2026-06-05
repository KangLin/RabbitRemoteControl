// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QObject>
#include "Operate.h"
#include "Backend.h"

/**
 * \~chinese 管理操作。此类仅用于 CPluginThread
 * \note 它运行在后台线程中。
 * \~english Manage operate. only used by CPluginThread
 * \note It runs in a background thread.
 * \~
 * \see CPluginThread CPluginBackendThread
 * \ingroup LIBAPI_THREAD
 */
class CManageBackend : public QObject
{
    Q_OBJECT

public:
    explicit CManageBackend(QObject *parent = nullptr);
    virtual ~CManageBackend();

public Q_SLOTS:
    void slotNewBackend(COperate* pOperate);
    void slotDeleteBackend(COperate* pOperate);

private:
    QMap<COperate*, CBackend*> m_Backends;
};
