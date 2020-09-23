#include "FrmViewer.h"
#include "ui_FrmViewer.h"

CFrmViewer::CFrmViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFrmViewer)
{
    ui->setupUi(this);
}

CFrmViewer::~CFrmViewer()
{
    delete ui;
}

void CFrmViewer::resizeEvent(QResizeEvent *event)
{
    
}

void CFrmViewer::mousePressEvent(QMouseEvent *event)
{
    emit sigMousePressEvent(event);
}

void CFrmViewer::mouseReleaseEvent(QMouseEvent *event)
{
    emit sigMouseReleaseEvent(event);
}

void CFrmViewer::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit sigMouseDoubleClickEvent(event);
}

void CFrmViewer::mouseMoveEvent(QMouseEvent *event)
{
    emit sigMouseMoveEvent(event);
}

void CFrmViewer::wheelEvent(QWheelEvent *event)
{
    emit sigWheelEvent(event);
}

void CFrmViewer::keyPressEvent(QKeyEvent *event)
{
    emit sigKeyPressEvent(event);
}

void CFrmViewer::keyReleaseEvent(QKeyEvent *event)
{
    emit sigKeyReleaseEvent(event);
}
