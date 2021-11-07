#ifndef CMANAGECONNECT_H
#define CMANAGECONNECT_H

#include <QObject>
#include "Connect.h"

/**
 * \~chinese 管理连接。此类仅用于 CPluginThread
 * \~english Manage CConnecter. only by used CPluginThread
 * \~
 * \see CPluginThread CConnecter
 */ 
class CManageConnect : public QObject
{
    Q_OBJECT
public:
    explicit CManageConnect(QObject *parent = nullptr);
    virtual ~CManageConnect();

public Q_SLOTS:
    void slotConnect(CConnecter* pConnecter);
    void slotDisconnect(CConnecter* pConnecter);
    
private:
    QMap<CConnecter*, CConnect*> m_Connects;    
};

#endif // CMANAGECONNECT_H
