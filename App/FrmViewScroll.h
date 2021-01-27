#ifndef CFRMVIEWSCROLL_H
#define CFRMVIEWSCROLL_H

#include "FrmViewer.h"
#include <QScrollArea>

/**
 * @brief The CFrmViewScroll class
 * @addtogroup API
 */
class CFrmViewScroll : public QScrollArea
{
    Q_OBJECT
public:
    explicit CFrmViewScroll(CFrmViewer* pView, QWidget *parent = nullptr);
    virtual ~CFrmViewScroll();
    
    void SetAdaptWindows(CFrmViewer::ADAPT_WINDOWS aw = CFrmViewer::Original);
    CFrmViewer::ADAPT_WINDOWS AdaptWindows();

    CFrmViewer* GetViewer();
};

#endif // CFRMVIEWSCROLL_H
