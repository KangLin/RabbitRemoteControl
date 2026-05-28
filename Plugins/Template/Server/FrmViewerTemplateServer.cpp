#include "FrmViewerTemplateServer.h"
#include "ui_FrmViewerTemplateServer.h"

CFrmViewerTemplateServer::CFrmViewerTemplateServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CFrmViewerTemplateServer)
{
    ui->setupUi(this);
}

CFrmViewerTemplateServer::~CFrmViewerTemplateServer()
{
    delete ui;
}
