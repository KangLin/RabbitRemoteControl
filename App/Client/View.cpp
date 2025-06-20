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
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

CView::~CView()
{
    qDebug(log) << Q_FUNC_INFO;
}
