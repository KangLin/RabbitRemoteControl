// Author: Kang Lin <kl222@126.com>

#ifndef CVIEW_H
#define CVIEW_H

#include <QWidget>

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
 * \brief The CView class
 * \note The parameters and return QWidget* is same as CConnecter::GetViewer()
 * \ingroup ViewApi
 */
class CView : public QWidget
{
    Q_OBJECT
public:
    explicit CView(QWidget *parent = nullptr);
    virtual ~CView();

    //! \note QWidget* pView must is same as CConnecter::GetViewer()
    virtual int AddView(QWidget* pView) = 0;
    //! \note QWidget* pView must is same as CConnecter::GetViewer()
    virtual int RemoveView(QWidget* pView) = 0;
    //! \note The return QWidget* must is same as CConnecter::GetViewer()
    virtual QWidget* GetCurrentView() = 0;
    virtual int SetCurrentView(QWidget* pView) = 0;
    virtual void SetWidowsTitle(QWidget* pView, const QString& szTitle,
                                const QIcon& icon, const QString& szToolTip) = 0;
    virtual int SetFullScreen(bool bFull) = 0;

Q_SIGNALS:
    //! \note The QWidget* pView must is same as CConnecter::GetViewer()
    void sigCloseView(const QWidget* pView);
    void sigCurrentChanged(const QWidget* pView);
};

#endif // CVIEW_H
