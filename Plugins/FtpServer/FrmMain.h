// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#pragma once
#include <QWidget>
#include <QStandardItemModel>

namespace Ui {
class CFrmMain;
}

class COperateFtpServer;
class CFrmMain : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmMain(COperateFtpServer* pOperate, QWidget *parent = nullptr);
    ~CFrmMain();
Q_SIGNALS:
    void sigDisconnect(const QString& szIp, const quint16 port);
public Q_SLOTS:
    void slotConnectCount(int nTotal, int nConnect, int nDisconnect);
    void slotConnected(const QString& szIp, const quint16 port);
    void slotDisconnected(const QString& szIp, const quint16 port);
private Q_SLOTS:
    void slotContextMenuRequested(const QPoint& pos);
private:
    Ui::CFrmMain *ui;
    QStandardItemModel m_ModelConnect;
};

