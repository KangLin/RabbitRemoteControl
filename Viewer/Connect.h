// Author: Kang Lin <kl222@126.com>
  
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
 * \~chinese
 * \brief 具体连接接口
 * \note 这个接口仅由插件实现
 * 
 * \~english
 * \brief Connect interface
 * \note The interface only is implemented by plugin
 * 
 * \~
 * \see  CConnecter CFrmViewer
 * \ingroup VIEWER_PLUGIN_API
 */
class VIEWER_EXPORT CConnect : public QObject
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
    
    /**
     * \return 
     *     \li < 0 : error
     *     \li = 0 : emit sigConnected
     *     \li = 1 : emit sigConnected in CConnect
     */
    virtual int Connect() = 0;
    virtual int Disconnect() = 0;
    /**
     * \return 
     *       \li >= 0: continue, Interval call time (msec)
     *       \li <  0: error
     * \see slotTimeOut()
     */
    virtual int Process();

    virtual void slotClipBoardChange() = 0;

protected Q_SLOTS:
    ///
    /// \brief Timing start Process()
    ///
    virtual void slotTimeOut();
    
Q_SIGNALS:
    void sigConnected();
    void sigDisconnected();

    void sigSetDesktopSize(int width, int height);
    void sigServerName(const QString& szName);

    void sigUpdateRect(const QRect& r, const QImage& image);
    void sigUpdateCursor(const QCursor& cursor);
    void sigUpdateCursorPosition(const QPoint& pos);
    void sigSetClipboard(QMimeData* data);

    void sigError(const int nError, const QString &szError);
    void sigInformation(const QString& szInfo);

public Q_SLOTS:
    virtual void slotMousePressEvent(Qt::MouseButtons, QPoint);
    virtual void slotMouseReleaseEvent(Qt::MouseButton, QPoint);
    virtual void slotMouseMoveEvent(Qt::MouseButtons, QPoint);
    virtual void slotWheelEvent(Qt::MouseButtons buttons, QPoint pos, QPoint angleDelta);
    virtual void slotKeyPressEvent(int key, Qt::KeyboardModifiers modifiers);
    virtual void slotKeyReleaseEvent(int key, Qt::KeyboardModifiers modifiers);

private:
    CFrmViewer* m_pView;
};

#endif // CCONNECT_H
