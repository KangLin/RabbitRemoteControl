#ifndef CCONNECT_H
#define CCONNECT_H

#pragma once

#include <QObject>
#include <QPoint>
#include "FrmViewer.h"

class CConnect : public QObject
{
    Q_OBJECT

public:
    explicit CConnect(CFrmViewer* pView = nullptr, QObject *parent = nullptr);
    virtual ~CConnect() override;
    
    int SetViewer(CFrmViewer* pView);
    
    virtual int SetServer(const QString& szHost, const int nPort);
    // 由子类解析成 IP 和 端口
    virtual int SetServerName(const QString& serverName);

    virtual int SetUser(const QString &szUser, const QString &szPassword);
    virtual int SetParamter(void *pPara);

    virtual int Initialize();
    virtual int Connect();
    virtual int Disconnect();
    virtual int Exec();
    
    virtual int SetShared(bool shared = true);
    virtual bool GetShared();
    virtual int SetReDesktopSize(bool re = true);
    virtual bool GetReDesktopSize();
    virtual int SetUseLocalCursor(bool u = true);
    virtual bool GetUserLocalCursor();
    
Q_SIGNALS:
    void sigConnected();
    void sigDisconnect();
    
    void sigSetDesktopSize(int width, int height);
    void sigSetName(const QString& szName);
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
    bool m_bShared;
    bool m_bReDesktopSize;
    bool m_bUseLocalCursor;
    CFrmViewer* m_pView;
    
    bool m_bExit;
};

#endif // CCONNECT_H
