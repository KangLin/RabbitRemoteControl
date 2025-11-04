#include "DlgScreenCapture.h"
#include "ui_DlgScreenCapture.h"

CDlgScreenCapture::CDlgScreenCapture(const QWebEngineDesktopMediaRequest request, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgScreenCapture)
{
    ui->setupUi(this);
    for(int i = 0; i < request.screensModel()->rowCount(); i++) {
        QModelIndex index;
        auto model = request.screensModel();
        index = model->index(i, 0);
        ui->cbScreen->addItem(QString::number(i) + ": " + model->data(index).toString());
    }
    for(int w = 0; w < request.windowsModel()->rowCount(); w++) {
        QModelIndex index;
        auto model = request.windowsModel();
        index = model->index(w, 0);
        ui->cbWindow->addItem(QString::number(w) + ": " + model->data(index).toString());
    }

    if(ui->cbScreen->count()> 0)
        ui->rbScreens->setChecked(true);
}

CDlgScreenCapture::~CDlgScreenCapture()
{
    delete ui;
}

void CDlgScreenCapture::on_cbScreen_currentIndexChanged(int index)
{
    ui->rbScreens->setChecked(true);
}

void CDlgScreenCapture::on_cbWindow_currentIndexChanged(int index)
{
    ui->rbWindows->setChecked(true);
}

int CDlgScreenCapture::GetIndex(Type &type, int &id)
{
    if(ui->rbScreens->isChecked()) {
        type = Type::Screen;
        id = ui->cbScreen->currentIndex();
    }
    if(ui->rbWindows->isChecked()) {
        type = Type::Window;
        id = ui->cbWindow->currentIndex();
    }
    if(id < 0)
        return -1;
    return 0;
}
