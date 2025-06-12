// Author: Kang Lin <kl222@126.com>

#ifndef CFrmScroll_H_2024_11_01
#define CFrmScroll_H_2024_11_01

#pragma once

#include <QScrollArea>
#include <QMouseEvent>
#include "FrmViewer.h"

/*!
 * \brief The scroll form class
 * \ingroup CLIENT_PLUGIN_API
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

#endif // CFrmScroll_H_2024_11_01
