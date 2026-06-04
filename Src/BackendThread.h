// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QThread>
#include "Operate.h"
#include "Backend.h"
#include "plugin_export.h"

/*!
 * \brief The backend thread
 * \ingroup PLUGIN_API
 */
class PLUGIN_EXPORT CBackendThread : public QThread
{
    Q_OBJECT

public:
    /*!
     * \brief CBackendThread
     * \param pOperate
     * \param bRunningSignal
     *          - true: when the thread is running, emit COperate::sigRunning signal
     *          - false: not emit signal
     * \param bFinishedSignal
     *          - true: when the thread is quit, emit COperate::sigFinished signal
     *          - false: not emit signal
     */
    explicit CBackendThread(COperate *pOperate, bool bRunningSignal, bool bFinishedSignal);
    virtual ~CBackendThread() override;
    /*!
     * \brief Quit
     * \note Use this replace QThread::quit, QThread::exit, QThread::terminate
     */
    virtual void quit();

protected:
    virtual void run() override;
    COperate* m_pOperate;
    CBackend* m_pBackend;

    bool m_bRunningSignal;
    bool m_bFinishedSignal;
};
