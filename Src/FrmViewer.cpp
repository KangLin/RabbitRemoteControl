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
