#include "FrmParameterTerminalBehavior.h"
#include "ui_FrmParameterTerminalBehavior.h"
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(Terminal)

CFrmParameterTerminalBehavior::CFrmParameterTerminalBehavior(CParameterTerminal *pPara, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFrmParameterTerminalBehavior),
    m_pPara(pPara)
{
    ui->setupUi(this);
    
    //qDebug(Terminal) << "KeyBindings" << QTermWidget::availableKeyBindings();
    ui->cbKeyBinding->addItems(QTermWidget::availableKeyBindings());
    ui->cbKeyBinding->setCurrentText(pPara->GetKeyBindings());

    foreach(auto c, QTextCodec::availableCodecs())
    {
        ui->cbTextCodecs->addItem(QString(c));
    }
    ui->cbTextCodecs->setCurrentText(pPara->GetTextCodec());
    if(0 > m_pPara->GetHistorySize())
    {
        ui->cbHistoryUnlimited->setChecked(true);
        ui->sbHistorySize->setValue(1000);
    }
    else
    {
        ui->cbHistorySize->setChecked(true);
        ui->sbHistorySize->setValue(pPara->GetHistorySize());
    }
}

CFrmParameterTerminalBehavior::~CFrmParameterTerminalBehavior()
{
    delete ui;
}

int CFrmParameterTerminalBehavior::AcceptSettings()
{
    if(!m_pPara) return -1;

    m_pPara->SetKeyBindings(ui->cbKeyBinding->currentText());
    m_pPara->SetTextCodec(ui->cbTextCodecs->currentText());
    if(ui->cbHistoryUnlimited->isChecked())
        m_pPara->SetHistorySize(-1);
    else    
        m_pPara->SetHistorySize(ui->sbHistorySize->value());
    
    return 0;
}
