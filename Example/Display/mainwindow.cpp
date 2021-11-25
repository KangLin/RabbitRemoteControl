#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "Display.h"
#include "Screen.h"

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
    //QImage img = CDisplay::Instance()->GetDisplay();
    QImage img = CScreen::Instance()->GetScreen();
    ui->lbImage->setPixmap(QPixmap::fromImage(img));
}
