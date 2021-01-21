#include "DlgSettingsTerminal.h"
#include "ui_DlgSettingsTerminal.h"

CDlgSettingsTerminal::CDlgSettingsTerminal(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSettingsTerminal)
{
    ui->setupUi(this);
}

CDlgSettingsTerminal::~CDlgSettingsTerminal()
{
    delete ui;
}

void CDlgSettingsTerminal::on_pushButton_clicked()
{
    this->accept();
}

void CDlgSettingsTerminal::on_pushButton_2_clicked()
{
    this->reject();
}
