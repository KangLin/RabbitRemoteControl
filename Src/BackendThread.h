// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QThread>
#include "Operate.h"
#include "plugin_export.h"

/*!
 * \brief The backend thread
 * \ingroup PLUGIN_API
 */
class PLUGIN_EXPORT CBackendThread : public QThread
{
    Q_OBJECT
public:
    explicit CBackendThread(COperate *pOperate = nullptr);
    virtual ~CBackendThread() override;
protected:
    virtual void run() override;
    COperate* m_pOperate;
};
