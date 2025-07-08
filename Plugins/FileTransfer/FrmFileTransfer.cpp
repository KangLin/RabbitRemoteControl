// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include "FrmFileTransfer.h"
#include "ui_FrmFileTransfer.h"

CFrmFileTransfer::CFrmFileTransfer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CFrmFileTransfer)
{
    ui->setupUi(this);
}

CFrmFileTransfer::~CFrmFileTransfer()
{
    delete ui;
}
