#include "FrmViewerTemplateBase.h"
#include "ui_FrmViewerTemplateBase.h"

CFrmViewerTemplateBase::CFrmViewerTemplateBase(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CFrmViewerTemplateBase)
{
    ui->setupUi(this);
}

CFrmViewerTemplateBase::~CFrmViewerTemplateBase()
{
    delete ui;
}
