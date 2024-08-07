#include "ParameterNetUI.h"
#include "ui_ParameterNetUI.h"
#include <QLoggingCategory>
#include <QMessageBox>

static Q_LOGGING_CATEGORY(log, "Client.Parameter.Net.UI")

CParameterNetUI::CParameterNetUI(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::CParameterNetUI),
    m_pNet(nullptr)
{
    ui->setupUi(this);
}

CParameterNetUI::~CParameterNetUI()
{
    delete ui;
}

//! [Set Parameter]
int CParameterNetUI::SetParameter(CParameterNet *pParameter)
{
    if(!pParameter) return -1;
    
    m_pNet = pParameter;
    
    ui->leHost->setText(m_pNet->GetHost());
    ui->spPort->setValue(m_pNet->GetPort());

    // Call user UI SetParameter
    ui->wUser->SetParameter(&m_pNet->m_User);

    return 0;
}
//! [Set Parameter]

//! [slotAccept]
int CParameterNetUI::slotAccept(bool validity)
{
    if(validity && ui->leHost->text().isEmpty()) {
        QMessageBox::critical(this, tr("Error"),
                              m_pNet->GetPrompt());
        qCritical(log) << "The host is empty";
        ui->leHost->setFocus();
        return -1;
    }

    m_pNet->SetHost(ui->leHost->text());
    m_pNet->SetPort(ui->spPort->value());
    
    ui->wUser->slotAccept();
    return 0;
}
//! [slotAccept]

void CParameterNetUI::on_leHost_editingFinished()
{
    auto s = ui->leHost->text().split(":");
    if(s.size() == 2) // IPV4
    {
        ui->spPort->setValue(s[1].toUInt());
        ui->leHost->setText(s[0]);
    }
}
