// Author: Kang Lin <kl222@126.com>
#include <QLoggingCategory>

#include "View.h"

static Q_LOGGING_CATEGORY(log, "App.View")
CView::CView(QWidget *parent) : QWidget(parent)
{
    qDebug(log) << Q_FUNC_INFO;
    setFocusPolicy(Qt::NoFocus);
}

CView::~CView()
{
    qDebug(log) << Q_FUNC_INFO;
}
