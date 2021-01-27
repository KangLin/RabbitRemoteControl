//! @author: Kang Lin(kl222@126.com)

#ifndef CVIEW_H
#define CVIEW_H

#include <QWidget>
#include "Connecter.h"
#include "FrmViewScroll.h"

/**
 * @brief The CView class
 */
class CView : public QWidget
{
    Q_OBJECT
public:
    explicit CView(QWidget *parent = nullptr);
    
    virtual int AddView(QWidget* pView) = 0;
    virtual int RemoveView(QWidget* pView) = 0;
    virtual QWidget* GetCurrentView() = 0;
    virtual int SetFullScreen(bool bFull) = 0;
    virtual void SetWidowsTitle(QWidget* pView, const QString& szTitle) = 0;
    virtual void SetAdaptWindows(CFrmViewer::ADAPT_WINDOWS aw = CFrmViewer::Original, QWidget* pView = nullptr) = 0;
    
Q_SIGNALS:
    void sigCloseView(const QWidget* pView);
    void sigAdaptWindows(const CFrmViewer::ADAPT_WINDOWS aw);
};

#endif // CVIEW_H
