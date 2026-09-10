// Author: Kang Lin <kl222@126.com>
#include <QLoggingCategory>

#include "View.h"

static Q_LOGGING_CATEGORY(log, "App.View")
CView::CView(CParameterApp *pPara, QWidget *parent) : QWidget(parent)
    , m_pParameterApp(pPara)
{
    qDebug(log) << Q_FUNC_INFO;
    Q_ASSERT(m_pParameterApp);
    //setFocusPolicy(Qt::NoFocus);
    //setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //setMinimumSize(200, 100);
}

CView::~CView()
{
    qDebug(log) << Q_FUNC_INFO;
}

int CView::SetFullScreen(bool bFull)
{
    int nRet = 0;
    nRet = OnFullScreen(bFull);
    //! Call the void OnFullScreen(bool) of the current view
    //! \see CFrmScroll::OnFullScreen
    //! \snippet Src/FrmScroll.cpp Full Screen
    auto pWin = GetCurrentView();
    if(pWin) {
        if(-1 < pWin->metaObject()->indexOfMethod("OnFullScreen(bool)")) {
            int nRet = 0;
            bool bRet = QMetaObject::invokeMethod(
                pWin, "OnFullScreen",
                Qt::DirectConnection,
                Q_RETURN_ARG(int, nRet),
                Q_ARG(bool, bFull));
            if(!bRet || nRet) {
                qCritical(log) << QString("Call %1:OnFullScreen(bool) fail")
                                      .arg(pWin->metaObject()->className());
            }
        }
    }
    return nRet;
}