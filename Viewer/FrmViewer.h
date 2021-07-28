// Author: Kang Lin <kl222@126.com>

#ifndef FRMVIEWER_H
#define FRMVIEWER_H

#pragma once

#include <QWidget>
#include "viewer_export.h"

class CConnecter;

namespace Ui {
class CFrmViewer;
}

/**
 * \~english
 * \brief 
 * A widget which displays output image from a CConnect
 * and sends input keypresses and mouse activity
 * to the CConnect.
 *
 * When the viewer receives new output from the CConnect,
 * it will update the display by calling slotUpdateRect().
 * 
 *\~
 * \see  CConnecter CConnect
 * \ingroup VIEWER_PLUGIN_API
 */
class VIEWER_EXPORT CFrmViewer : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmViewer(QWidget *parent = nullptr);
    virtual ~CFrmViewer() override;

    enum ADAPT_WINDOWS {
        Disable,         ///< \~english Disable adapt windows
        Auto,
        Original,        ///< \~english Original desktop size, the left-top of the desktop is aligned with the left-top of the window
        OriginalCenter,  ///< \~english Original desktop size, the center of the desktop is aligned with the center of the window
        Zoom,            ///< \~english Desktop adapt to windows
        AspectRation,    ///< \~english Keep desktop aspectration adapt to windows
    };
    void SetAdaptWindows(ADAPT_WINDOWS aw = Original);
    ADAPT_WINDOWS AdaptWindows();

public Q_SLOTS:
    void slotSetDesktopSize(int width, int height);
    void slotSetName(const QString& szName);
    void slotUpdateRect(const QRect& r, const QImage& image);
    void slotUpdateCursor(const QCursor& cursor);
    void slotUpdateCursorPosition(const QPoint& pos);
    void slotSystemCombination();
    
Q_SIGNALS:
    void sigMousePressEvent(QMouseEvent *event);
    void sigMouseReleaseEvent(QMouseEvent *event);
    void sigMouseMoveEvent(QMouseEvent *event);
    void sigWheelEvent(QWheelEvent *event);
    void sigKeyPressEvent(int key, Qt::KeyboardModifiers modify);
    void sigKeyReleaseEvent(int key, Qt::KeyboardModifiers modify);
    
    // Please use CConnecter::sigServerName
    void sigServerName(const QString &szName);
    
private:
    Ui::CFrmViewer *ui;

    void paintDesktop();
    int TranslationMousePoint(QPointF inPos, QPointF &outPos);
    QRectF GetAspectRationRect();
    
protected:
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;

private:
    QImage m_Desktop;

    ADAPT_WINDOWS m_AdaptWindows;
};

#endif // FRMVIEWER_H
