// Author: Kang Lin <kl222@126.com>

#ifndef RAILWINDOW_H
#define RAILWINDOW_H

#include "FrmViewer.h"
#include "Rail.h"

/*!
 * \~chinese 远程应用程序窗口
 * \~english Remote application window
 *
 */
class CRailWindow : public CFrmViewer
{
    Q_OBJECT
public:
    explicit CRailWindow(QWidget *parent = nullptr);

    CRailInfo m_Info;
    int Show(UINT32 state);

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;

private:
    virtual int TranslationMousePoint(QPointF inPos, QPointF &outPos);
};

#endif // RAILWINDOW_H
