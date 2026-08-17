// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QThread>
#include "Operate.h"
#include "Backend.h"
#include "plugin_export.h"

/*!
 * \brief The backend thread
 * \ingroup LIBAPI_THREAD
 */
class PLUGIN_EXPORT CBackendThread : public QThread
{
    Q_OBJECT

public:
    /*!
     * \brief CBackendThread
     * \param pOperate
     * \param pParent: if is nullptr, then the thread is deleted when it is finished.
     *                 other, the thread is deleted by the parent when the parent is deleted.
     */
    explicit CBackendThread(COperate *pOperate, QObject* pParent = nullptr);
    virtual ~CBackendThread() override;
    /*!
     * \brief Quit
     * \note Use this replace QThread::quit, QThread::exit, QThread::terminate
     */
    void quit();

protected:
    virtual void run() override;
    COperate* m_pOperate;
    CBackend* m_pBackend;
};
