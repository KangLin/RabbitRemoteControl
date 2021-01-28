#include "FrmTermWidget.h"
#include <QDebug>

CFrmTermWidget::CFrmTermWidget(QWidget *parent) : QTermWidget(0, parent)
{}

CFrmTermWidget::~CFrmTermWidget()
{
    qDebug() << "CFrmTermWidget::~CFrmTermWidget()";
}
