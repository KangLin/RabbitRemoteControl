// Author: Kang Lin <kl222@126.com>

#ifndef RAILWINDOW_H
#define RAILWINDOW_H

#include "FrmViewer.h"
#include "RAIL.h"

/*!
 * \~chinese 远程应用程序窗口
 * \note 它运行在主线程中（UI线程）
 * \~english Remote application window
 * \note It is run on main thread(UI thread)
 */
class CRAILWindow : public CFrmViewer
{
    Q_OBJECT
public:
    explicit CRAILWindow(QWidget *parent = nullptr);

    CRAIL* m_pRail;
    CRAILManageWindows* m_pManage;

    int Show(UINT32 state);

    UINT32 GetWindowId() const;
    void SetWindowId(UINT32 newWindowId);
    INT32 GetWindowOffsetX() const;
    void SetWindowOffsetX(INT32 newWindowOffsetX);
    INT32 GetWindowOffsetY() const;
    void SetWindowOffsetY(INT32 newWindowOffsetY);
    UINT32 GetWindowWidth() const;
    void SetWindowWidth(UINT32 newWindowWidth);
    UINT32 GetWindowHeight() const;
    void SetWindowHeight(UINT32 newWindowHeight);
    UINT32 GetShowState() const;
    void SetShowState(UINT32 newShowState);

    INT32 GetVisibleOffsetX() const;
    void SetVisibleOffsetX(INT32 newVisibleOffsetX);
    INT32 GetVisibleOffsetY() const;
    void SetVisibleOffsetY(INT32 newVisibleOffsetY);

    void SetServerExit(bool bExit = true);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual void closeEvent(QCloseEvent *event) override;
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual void focusOutEvent(QFocusEvent *event) override;

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;

private:
    virtual int TranslationMousePoint(QPointF inPos, QPointF &outPos);

private:
    UINT32 m_WindowId;
    /*!
     * | Value | Meaning                                         |
     * |:------|:------------------------------------------------|
     * | 0x00  |Do not show the window.                          |
     * | 0x02  |Show the window minimized.                       |
     * | 0x03  |Show the window maximized.                       |
     * | 0x05  |Show the window in its current size and position.|
    */
    UINT32 m_ShowState;
    INT32 m_WindowOffsetX;
    INT32 m_WindowOffsetY;
    UINT32 m_WindowWidth;
    UINT32 m_WindowHeight;
    INT32 m_VisibleOffsetX;
    INT32 m_VisibleOffsetY;
    bool m_bServerExit;
};

#endif // RAILWINDOW_H
