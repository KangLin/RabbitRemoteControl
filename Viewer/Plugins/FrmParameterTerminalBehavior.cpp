#include "FrmParameterTerminalBehavior.h"
#include "ui_FrmParameterTerminalBehavior.h"

CFrmParameterTerminalBehavior::CFrmParameterTerminalBehavior(CParameterTerminal *pPara, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFrmParameterTerminalBehavior),
    m_pPara(pPara)
{
    ui->setupUi(this);
    
    //qDebug() << "KeyBindings" << QTermWidget::availableKeyBindings();
    ui->cbKeyBinding->addItems(QTermWidget::availableKeyBindings());
    ui->cbKeyBinding->setCurrentText(pPara->szKeyBindings);

    foreach(auto c, QTextCodec::availableCodecs())
    {
        ui->cbTextCodecs->addItem(QString(c));
    }
    ui->cbTextCodecs->setCurrentText(pPara->textCodec);
    
}

CFrmParameterTerminalBehavior::~CFrmParameterTerminalBehavior()
{
    delete ui;
}

int CFrmParameterTerminalBehavior::AcceptSettings()
{
    if(!m_pPara) return -1;

    m_pPara->szKeyBindings = ui->cbKeyBinding->currentText();
    m_pPara->textCodec = ui->cbTextCodecs->currentText();
    
    return 0;
}
