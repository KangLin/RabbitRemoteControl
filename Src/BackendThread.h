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
     * \param bFinishedSignal
     *          - true: When an error occurs, emit a `COperate::sigFinished()` signal
     *          - false: not emit signal
     */
    explicit CBackendThread(COperate *pOperate = nullptr, bool bFinishedSignal = true);
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
    
    //! When an error occurs, emit a `COperate::sigFinished()` signal
    bool m_bFinishedSignal;
};
