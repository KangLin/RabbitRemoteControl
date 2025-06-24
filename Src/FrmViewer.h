// Author: Kang Lin <kl222@126.com>

#ifndef FRMVIEWER_H
#define FRMVIEWER_H

#pragma once

#include <QWidget>
#include <QSettings>
#include <QMutex>
#include <QImage>
#include <QTimer>
#include <QSharedPointer>
#include "plugin_export.h"

#ifdef USE_FROM_OPENGL

    #include "FrmViewerOpenGL.h"
    #define CFrmViewer CFrmViewerOpenGL

#else

class CRecordVideoThread;

/**
 * \~chinese
 * \brief 
 * 用于显示从 CConnectDesktop 输出的图像，和向 CConnectDesktop 发送键盘、鼠标事件。
 * - 当新的输出图像从 CConnectDesktop 输出时，它调用 \ref slotUpdateRect 更新显示。
 * - 当鼠标更新时，调用 \ref slotUpdateCursor ；位置更新时，调用 \ref slotUpdateCursorPosition
 *
 * \~english
 * \brief 
 * A widget which displays output image from a CConnectDesktop
 * and sends input keypresses and mouse activity
 * to the CConnectDesktop.
 *
 * - When the viewer receives new image output from the CConnectDesktop,
 *   it will update the display by calling \ref slotUpdateRect.
 * - When the mouse update, then call \ref slotUpdateCursor ;
 *   if update position, then call \ref slotUpdateCursorPosition
 * 
 *\~
 * \see  CConnecter CConnectDesktop
 * \ingroup PLUGIN_API
 */
class PLUGIN_EXPORT CFrmViewer : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double ZoomFactor READ GetZoomFactor WRITE slotSetZoomFactor)

public:
    explicit CFrmViewer(QWidget *parent = nullptr);
    virtual ~CFrmViewer() override;
    
    /**
     * \~chinese 窗口适配枚举常量
     * \~english The ADAPT_WINDOWS enum
     */
    enum class ADAPT_WINDOWS {
        Disable = 0,                  ///< \~english Disable adapt windows
                                      ///< \~chinese 禁用适配窗口
        Auto = 1,
        Original = 2,                 ///< \~english Original desktop size, the left-top of the desktop is aligned with the left-top of the window
                                      ///< \~chinese 原始桌面大小，桌面的左上点与窗口的左上点对齐
        OriginalCenter = 3,           ///< \~english Original desktop size, the center of the desktop is aligned with the center of the window
                                      ///< \~chinese 原始桌面大小，桌面中心点与窗口中心点对齐
        Zoom = 4,                     ///< \~english zoom windows = desktop size * factor
                                      ///< \~chinese 缩放窗口大小等于桌面大小 * 系数
        ZoomToWindow = 5,             ///< \~english Desktop adapt to windows
                                      ///< \~chinese 桌面缩放到窗口大小，窗口是固定的
        KeepAspectRationToWindow = 6, ///< \~english Keep desktop aspectration adapt to windows
                                      ///< \~chinese 保持长宽比缩放到窗口大小,窗口是固定的
    };
    Q_ENUM(ADAPT_WINDOWS)
    ADAPT_WINDOWS GetAdaptWindows();
    
    /*!
     * \~chinese 调整缩放系数。
     *  调整完成后需要调用 SetAdaptWindows(FrmViewer::Zoom) 缩放窗口大小。
     * \~english Adjust the zoom factor. After the adjustment is completed,
     *  you need to call SetAdaptWindows(FrmViewer::Zoom) to zoom the window size. 
     * \return 
     */
    double GetZoomFactor() const;

    QSize GetDesktopSize();

    enum LED_STATE{
        Unknown = -1,
        ScrollLock = 1,
        NumLock = 1 << 1,
        CapsLock = 1 << 2,
    };
    Q_ENUM(LED_STATE)

public Q_SLOTS:
    void slotSetAdaptWindows(CFrmViewer::ADAPT_WINDOWS aw = ADAPT_WINDOWS::Original);
    int slotSetZoomFactor(double newZoomFactor);
    
    /*================== Internal calls ==================*/
    /*!
     * \brief Update desktop size
     * \param width
     * \param height
     * \note The plugin is use CConnectDesktop::sigSetDesktopSize
     */
    void slotSetDesktopSize(int width, int height);
    /*!
     * \brief Update desktop name
     * \param szName
     */
    void slotSetName(const QString& szName);
    /*!
     * \brief Update image
     * \param r: image rect
     * \param image: image
     */
    void slotUpdateRect(const QRect& r, const QImage& image);
    void slotUpdateRect(const QImage& image);
    /*!
     * \brief Update cursor
     * \param cursor
     */
    void slotUpdateCursor(const QCursor& cursor);
    /*!
     * \brief Update cursor position
     * \param pos
     */
    void slotUpdateCursorPosition(const QPoint& pos);
    void slotUpdateLedState(unsigned int state);
    /*!
     * \brief Enable mouse and keyboard
     */
    void slotRunning();

Q_SIGNALS:
    void sigMousePressEvent(QMouseEvent* event, QPoint pos);
    void sigMouseReleaseEvent(QMouseEvent* event, QPoint pos);
    void sigMouseMoveEvent(QMouseEvent* event, QPoint pos);
    void sigMouseMoveEvent(QMouseEvent* event);
    void sigWheelEvent(QWheelEvent *event, QPoint pos);
    void sigKeyPressEvent(QKeyEvent *event);
    void sigKeyReleaseEvent(QKeyEvent *event);

    // Please use CConnecter::sigServerName
    void sigServerName(const QString &szName);

    /*!
     * \~chinese 视图获得焦点
     * \~english The view is focus
     * \param pView
     */
    void sigViewerFocusIn(QWidget* pView);

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) override;

    virtual void focusInEvent(QFocusEvent *event) override;
    virtual void focusOutEvent(QFocusEvent *event) override;

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;

private:
    QImage m_Desktop;
    QSize m_DesktopSize;

    ADAPT_WINDOWS m_AdaptWindows;
    double m_dbZoomFactor;

    int ReSize(int width, int height);
    void paintDesktop();
    int TranslationMousePoint(QPointF inPos, QPointF &outPos);
    QRectF GetAspectRationRect();

public:
    virtual QImage GrabImage(int x = 0, int y = 0, int w = -1, int h = -1);

public Q_SLOTS:
    void slotRecordVideo(bool bRecord, qreal nRate = 0);
    void slotRecordVideo();
Q_SIGNALS:
    void sigRecordVideo(const QImage& img);
private:
    QTimer m_TimerRecordVideo;
    bool m_bRecordVideo;
};

#endif // #ifdef USE_FROM_OPENGL

#endif // FRMVIEWER_H
