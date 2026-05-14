#include "FrmViewerSftpServer.h"
#include "ui_FrmViewerSftpServer.h"

CFrmViewerSftpServer::CFrmViewerSftpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CFrmViewerSftpServer)
{
    ui->setupUi(this);
}

CFrmViewerSftpServer::~CFrmViewerSftpServer()
{
    delete ui;
}
