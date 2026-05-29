// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QStandardItemModel>
#include <QWidget>
#include <QToolBar>
#include <QTableView>
#include <QLabel>
#include <QAction>
#include "plugin_export.h"

/*!
 * \brief The CFrmViewServer class
 * \see COperateServer
 */
class PLUGIN_EXPORT CFrmViewServer : public QWidget
{
    Q_OBJECT
public:
    explicit CFrmViewServer(QWidget *parent = nullptr);
    virtual ~CFrmViewServer();

    void AddActionInToolBar(QAction* pAction);

Q_SIGNALS:
    void sigDisconnect(const QString& szIp, const quint16 port);

private Q_SLOTS:
    void slotConnectCount(int nTotal, int nConnect, int nDisconnect);
    void slotConnected(const QString& szIp, const quint16 port);
    void slotDisconnected(const QString& szIp, const quint16 port);
    void slotContextMenuRequested(const QPoint& pos);

private:
    QToolBar* m_pToolBar;
    QTableView* m_ptvClients;
    QStandardItemModel m_ModelConnect;
    QLabel* m_plbConnectCount;
};
