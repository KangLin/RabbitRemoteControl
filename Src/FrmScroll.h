// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QScrollArea>
#include <QMouseEvent>
#include "FrmViewer.h"

/*!
 * \brief The scroll form class
 * \ingroup gOperateDesktop
 */
class CFrmScroll : public QScrollArea
{
    Q_OBJECT
public:
    explicit CFrmScroll(CFrmViewer* pView, QWidget *parent = nullptr);
    virtual ~CFrmScroll();

    CFrmViewer::ADAPT_WINDOWS AdaptWindows();
    CFrmViewer* GetViewer();

public Q_SLOTS:
    void slotSetAdaptWindows(
        CFrmViewer::ADAPT_WINDOWS aw = CFrmViewer::ADAPT_WINDOWS::Original);

private Q_SLOTS:
    void slotMouseMoveEvent(QMouseEvent *event);
};
