// Author: Kang Lin <kl222@126.com>

#ifndef CVIEWFRMSCROLL_H
#define CVIEWFRMSCROLL_H

#include "FrmViewer.h"
#include <QScrollArea>
#include <QMouseEvent>

/**
 * @brief The CFrmViewScroll class
 *        This class is only used for GUI views.
 *        Not used in the terminal view,
 *        the terminal view comes with a scroll bar 
 * @ingroup ViewApi
 */
class CViewFrmScroll : public QScrollArea
{
    Q_OBJECT
public:
    explicit CViewFrmScroll(CFrmViewer* pView, QWidget *parent = nullptr);
    virtual ~CViewFrmScroll();
    
    void SetAdaptWindows(CFrmViewer::ADAPT_WINDOWS aw = CFrmViewer::Original);
    CFrmViewer::ADAPT_WINDOWS AdaptWindows();
    
    CFrmViewer* GetViewer();

private Q_SLOTS:
    void slotMouseMoveEvent(QMouseEvent *event);
};

#endif // CVIEWFRMSCROLL_H
