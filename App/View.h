//! @author: Kang Lin(kl222@126.com)

#ifndef CVIEW_H
#define CVIEW_H

#include <QWidget>
#include "Connecter.h"

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
    
Q_SIGNALS:
    void sigCloseView(const QWidget* pView);
};

#endif // CVIEW_H
