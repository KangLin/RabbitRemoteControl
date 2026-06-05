// Author: Kang Lin <kl222@126.com>

#include "PluginThread.h"
#include "ManageBackend.h"

#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Client.Plugin.Thread")

CPluginThread::CPluginThread(QObject *parent)
    : QThread(parent)
{
    qDebug(log) << Q_FUNC_INFO;
}

CPluginThread::~CPluginThread()
{
    qDebug(log) << Q_FUNC_INFO;
}

/*!
 * \brief One thread handles multiple operate.
 *        Register sigNewBackend and sigDeleteBackend, then enter event loop.
 */
void CPluginThread::run()
{
    qDebug(log) << "CPluginThread::run() start";
    CManageBackend mb;
    bool check = false;
    check = connect(this, SIGNAL(sigNewBackend(COperate*)),
                    &mb, SLOT(slotNewBackend(COperate*)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigDeleteBackend(COperate*)),
                    &mb, SLOT(slotDeleteBackend(COperate*)));
    Q_ASSERT(check);

    exec();
    qDebug(log) << "CPluginThread::run() end";
}
