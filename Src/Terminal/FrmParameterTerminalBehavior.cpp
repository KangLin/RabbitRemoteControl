#include <QLoggingCategory>

#include "FrmParameterTerminalBehavior.h"
#include "ui_FrmParameterTerminalBehavior.h"

static Q_LOGGING_CATEGORY(log, "Terminal")

CFrmParameterTerminalBehavior::CFrmParameterTerminalBehavior(QWidget *parent) :
    CParameterUI(parent),
    ui(new Ui::CFrmParameterTerminalBehavior),
    m_pPara(nullptr)
{
    ui->setupUi(this);
}

CFrmParameterTerminalBehavior::~CFrmParameterTerminalBehavior()
{
    delete ui;
}

int CFrmParameterTerminalBehavior::SetParameter(CParameter *pParameter)
{
    m_pPara = qobject_cast<CParameterTerminal*>(pParameter);
    if(!m_pPara) return -1;

    //qDebug(log) << "KeyBindings" << QTermWidget::availableKeyBindings();
    ui->cbKeyBinding->addItems(QTermWidget::availableKeyBindings());
    ui->cbKeyBinding->setCurrentText(m_pPara->GetKeyBindings());
    
    ui->cbTextCodecs->setCurrentText(m_pPara->GetTextCodec());
    if(0 > m_pPara->GetHistorySize())
    {
        ui->cbHistoryUnlimited->setChecked(true);
        ui->sbHistorySize->setValue(1000);
    }
    else
    {
        ui->cbHistorySize->setChecked(true);
        ui->sbHistorySize->setValue(m_pPara->GetHistorySize());
    }
    return 0;
}

int CFrmParameterTerminalBehavior::Accept()
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
