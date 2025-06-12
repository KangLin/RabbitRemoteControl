#ifndef CMANAGECONNECT_H
#define CMANAGECONNECT_H

#include <QObject>
#include "ConnecterConnect.h"

/**
 * \~chinese 管理连接。此类仅用于 CPluginThread
 * \~english Manage CConnecter. only used by CPluginThread
 * \~
 * \see CPluginThread CPluginClientThread CConnecterConnect
 * \ingroup LIBAPI_THREAD
 */ 
class CManageConnect : public QObject
{
    Q_OBJECT
public:
    explicit CManageConnect(QObject *parent = nullptr);
    virtual ~CManageConnect();

public Q_SLOTS:
    void slotConnect(CConnecterConnect *pConnecter);
    void slotDisconnect(CConnecterConnect* pConnecter);

private:
    QMap<CConnecterConnect*, CConnect*> m_Connects;
};

#endif // CMANAGECONNECT_H
