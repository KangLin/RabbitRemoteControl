#ifndef CMANAGECONNECT_H
#define CMANAGECONNECT_H

#include <QObject>
#include "ConnecterDesktop.h"

/**
 * \~chinese 管理连接。此类仅用于 CPluginThread
 * \~english Manage CConnecter. only used by CPluginThread
 * \~
 * \see CPluginThread CPluginClientThread CConnecterDesktop
 * \ingroup LIBAPI_THREAD
 */ 
class CManageConnect : public QObject
{
    Q_OBJECT
public:
    explicit CManageConnect(QObject *parent = nullptr);
    virtual ~CManageConnect();

public Q_SLOTS:
    void slotConnect(CConnecterDesktop *pConnecter);
    void slotDisconnect(CConnecterDesktop* pConnecter);
    
private:
    QMap<CConnecterDesktop*, CConnect*> m_Connects;    
};

#endif // CMANAGECONNECT_H
