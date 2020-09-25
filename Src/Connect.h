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
    virtual ~CConnect();
    
    virtual int SetServer(const QString &szIp, const int port);
    virtual int SetUser(const QString &szUser, const QString &szPassword);
    virtual int SetParamter(void *pPara);
    
    virtual int Initialize();
    virtual int Connect();
    virtual int Disconnect();
    virtual int Exec();

    virtual QString GetServerName();
    
Q_SIGNALS:
    void sigConnected();
    void sigDisconnect();
    void sigSetDesktopSize(int width, int height);
    void sigSetCursor(int width, int height, const QPoint& hotspot,
                      void* data, void* mask);
    void sigBell();
    void sigServerCutText(const QString& szText);
    void slotUpdateRect(const QRect& r, const QImage& image);
    void sigError(const int nError, const QString &szError);
    
public slots:
        
protected:
    QString m_szIp;
    int m_nPort;
    QString m_szUser;
    QString m_szPassword;
    CFrmViewer* m_pView;
    
    bool m_bExit;
};

#endif // CCONNECT_H
