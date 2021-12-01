#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "Desktop.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbCapture_clicked()
{
    QImage img = CDesktop::Instance()->GetDesktop();
    ui->lbImage->setPixmap(QPixmap::fromImage(img));
}
