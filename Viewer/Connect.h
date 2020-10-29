#ifndef CCONNECT_H
#define CCONNECT_H

#pragma once

#include <QObject>
#include <QPoint>
#include "FrmViewer.h"
#include <QtPlugin>

class RABBITREMOTECONTROL_EXPORT CConnect : public QObject
{
    Q_OBJECT

public:
    explicit CConnect(CFrmViewer* pView = nullptr, QObject *parent = nullptr);
    virtual ~CConnect() override;

    virtual QString GetDescription();
    
    int SetViewer(CFrmViewer* pView);
    
    virtual int SetServer(const QString& szHost, const int nPort);
    // 由子类解析成 IP 和 端口
    virtual int SetServerName(const QString& serverName);

    virtual int SetUser(const QString &szUser, const QString &szPassword);
    virtual int SetParamter(void *pPara);

public Q_SLOTS:
    virtual int Initialize();
    virtual int Connect() = 0;
    virtual int Disconnect() = 0;
    virtual int Process() = 0;
    
Q_SIGNALS:
    void sigConnected();
    void sigDisconnected();
    
    void sigSetDesktopSize(int width, int height);
    void sigSetDesktopName(const QString& szName);
    void sigSetCursor(int width, int height, const QPoint& hotspot,
                      void* data, void* mask);
    //void sigBell();
    void sigServerCutText(const QString& szText);
    void sigUpdateRect(const QRect& r, const QImage& image);
    void sigError(const int nError, const QString &szError);
    
public Q_SLOTS:
    virtual void slotMousePressEvent(QMouseEvent*);
    virtual void slotMouseReleaseEvent(QMouseEvent*);
    virtual void slotMouseMoveEvent(QMouseEvent*);
    virtual void slotWheelEvent(QWheelEvent*);
    virtual void slotKeyPressEvent(QKeyEvent*);
    virtual void slotKeyReleaseEvent(QKeyEvent*);
    
protected:
    QString m_szServerName;
    QString m_szHost;
    int m_nPort;
    
    QString m_szUser;
    QString m_szPassword;

    CFrmViewer* m_pView;
};

#endif // CCONNECT_H
