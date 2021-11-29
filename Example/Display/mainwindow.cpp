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
    QImage img = CDesktop::Instance()->GetDesktop(); //0,0, CDesktop::Instance()->Width(), CDesktop::Instance()->Height());
    //QImage img = CScreen::Instance()->GetScreen();
    ui->lbImage->setPixmap(QPixmap::fromImage(img));
}
