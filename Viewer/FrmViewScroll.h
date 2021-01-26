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
    explicit CFrmViewScroll(QWidget *parent = nullptr);
    
    void SetAdaptWindows(ADAPT_WINDOWS aw = Original);
    ADAPT_WINDOWS AdaptWindows();
    
    CFrmViewer* GetViewer();

private:
    CFrmViewer* m_pView;
    
};

#endif // CFRMVIEWSCROLL_H
