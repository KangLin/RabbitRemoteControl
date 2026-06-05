// Author: Kang Lin <kl222@126.com>

#include "FrmViewerTemplateQtEvent.h"
#include "ui_FrmViewerTemplateQtEvent.h"

CFrmViewerTemplateQtEvent::CFrmViewerTemplateQtEvent(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CFrmViewerTemplateQtEvent)
{
    ui->setupUi(this);
}

CFrmViewerTemplateQtEvent::~CFrmViewerTemplateQtEvent()
{
    delete ui;
}
