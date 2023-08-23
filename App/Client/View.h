// Author: Kang Lin <kl222@126.com>

#ifndef CVIEW_H
#define CVIEW_H

#include <QWidget>
#include "Connecter.h"
#include "ViewFrmScroll.h"

/** 
 *  \~english \defgroup ViewApi The view API
 *  The view API
 *  \~chinese \defgroup ViewApi 视图应用接口
 *  视图应用接口
 *  
 *  \~
 *  \ingroup VIEWER_APP
 */

/**
 * @brief The CView class
 * @note The parameters and return QWidget* is same as CConnecter::GetViewer()
 * @ingroup ViewApi
 */
class CView : public QWidget
{
    Q_OBJECT
public:
    explicit CView(QWidget *parent = nullptr);
    
    /// QWidget* pView must is same as CConnecter::GetViewer()
    virtual int AddView(QWidget* pView) = 0;
    /// QWidget* pView must is same as CConnecter::GetViewer()
    virtual int RemoveView(QWidget* pView) = 0;
    /// @note The return QWidget* must is same as CConnecter::GetViewer()
    virtual QWidget* GetCurrentView() = 0;
    virtual int SetFullScreen(bool bFull) = 0;
    /// The QWidget* pView must is same as CConnecter::GetViewer()
    virtual void SetWidowsTitle(QWidget* pView, const QString& szTitle, const QIcon& icon, const QString& szToolTip) = 0;
    /// The QWidget* pView must is same as CConnecter::GetViewer()
    virtual void SetAdaptWindows(CFrmViewer::ADAPT_WINDOWS aw = CFrmViewer::Auto, QWidget* pView = nullptr) = 0;
    
    virtual double GetZoomFactor() = 0;
    /*!
     * \param szFile: save file
     * \param bRemoteDesktop: slot remote desktop
     * \return 
     */
    virtual int Screenslot(const QString& szFile, bool bRemoteDesktop = true) = 0;
    virtual QSize GetDesktopSize() = 0;

public Q_SLOTS:
    virtual void slotSystemCombination() = 0;
    virtual void slotZoomIn() = 0;
    virtual void slotZoomOut() = 0;
    virtual void slotZoomFactor(double v) = 0;

Q_SIGNALS:
    // @note The QWidget* pView must is same as CConnecter::GetViewer()
    void sigCloseView(const QWidget* pView);
    void sigAdaptWindows(const CFrmViewer::ADAPT_WINDOWS aw);
};

#endif // CVIEW_H
