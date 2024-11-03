// Author: Kang Lin <kl222@126.com>
#include <QLoggingCategory>

#include "View.h"

static Q_LOGGING_CATEGORY(log, "App.View")
CView::CView(QWidget *parent) : QWidget(parent)
{
    qDebug(log) << __FUNCTION__;
    setFocusPolicy(Qt::NoFocus);
}

CView::~CView()
{
    qDebug(log) << __FUNCTION__;
}
