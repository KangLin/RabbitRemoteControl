// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QAtomicInteger>
#include <QThread>
#include "DesktopManager.h"

/*!
 * \~chinese 桌面管理线程
 *
 * \~english Desktop manager thread
 */
class CDesktopManagerThread : public QThread
{
    Q_OBJECT

public:
    explicit CDesktopManagerThread(CParameterPlugin *pPara, QObject *parent = nullptr);
    virtual ~CDesktopManagerThread() override;
    bool IsStart();

Q_SIGNALS:
    void sigDisableShortcuts();
    void sigRestoreShortcuts();

protected:
    virtual void run() override;

private:
    CDesktopManager* m_pDesktopManager;
    CParameterPlugin *m_pPara;
    QAtomicInteger<int> m_Start;
};
