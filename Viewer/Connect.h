//! @author: Kang Lin (kl222@126.com)
  
#ifndef CCONNECT_H
#define CCONNECT_H

#pragma once

#include <QObject>
#include <QPoint>
#include <QtPlugin>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include "Connecter.h"
#include <QMimeData>
#include "FrmViewer.h"

/**
 * @brief The CConnect class. It is finished connect in background thread.
 * @note The interface only is implemented by plugin
 * @see  CConnecter CFrmViewer
 * @addtogroup PLUGIN_API
 */
class RABBITREMOTECONTROL_EXPORT CConnect : public QObject
{
    Q_OBJECT

public:
    explicit CConnect(CConnecter* pConnecter, QObject *parent = nullptr);
    virtual ~CConnect() override;

protected:
    virtual int SetConnecter(CConnecter* pConnecter);
    virtual int SetViewer(CFrmViewer* pView);
    virtual int SetParamter(void *pPara);

public Q_SLOTS:
    virtual int Initialize();
    virtual int Clean();
    virtual int Connect() = 0;
    virtual int Disconnect() = 0;
    /**
     * @brief Process
     * @return 0: continue
     *         1: exit
     *       < 0: error
     */
    virtual int Process() = 0;
    
    virtual void slotClipBoardChange() = 0;
    
Q_SIGNALS:
    void sigConnected();
    void sigDisconnected();
    
    void sigSetDesktopSize(int width, int height);
    void sigServerName(const QString& szName);
    
    void sigUpdateRect(const QRect& r, const QImage& image);
    void sigUpdateCursor(const QCursor& cursor);
    void sigSetClipboard(QMimeData* data);
    
    void sigError(const int nError, const QString &szError);
    void sigInformation(const QString& szInfo);

public Q_SLOTS:
    virtual void slotMousePressEvent(QMouseEvent*);
    virtual void slotMouseReleaseEvent(QMouseEvent*);
    virtual void slotMouseMoveEvent(QMouseEvent*);
    virtual void slotWheelEvent(QWheelEvent*);
    virtual void slotKeyPressEvent(QKeyEvent*);
    virtual void slotKeyReleaseEvent(QKeyEvent*);

private:
    CFrmViewer* m_pView;
};

#endif // CCONNECT_H
