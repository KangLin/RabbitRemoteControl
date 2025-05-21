#include "ParameterSSHTunnelUI.h"
#include "ui_ParameterSSHTunnelUI.h"

CParameterSSHTunnelUI::CParameterSSHTunnelUI(QWidget *parent)
    : CParameterUI(parent)
    , ui(new Ui::CParameterSSHTunnelUI)
    , m_pParameters(nullptr)
{
    ui->setupUi(this);
}

CParameterSSHTunnelUI::~CParameterSSHTunnelUI()
{
    delete ui;
}

int CParameterSSHTunnelUI::SetParameter(CParameter *pParameter)
{
    m_pParameters = qobject_cast<CParameterSSHTunnel*>(pParameter);
    // Call net ui SetParameter
    ui->wNet->SetParameter(&m_pParameters->m_Net);
    ui->leSourceHost->setText(m_pParameters->GetSourceHost());
    ui->sbSourcePort->setValue(m_pParameters->GetSourcePort());
    ui->lePcapFile->setText(m_pParameters->GetPcapFile());
    return 0;
}

int CParameterSSHTunnelUI::Accept()
{
    int nRet = 0;
    m_pParameters->SetSourceHost(ui->leSourceHost->text());
    m_pParameters->SetSourcePort(ui->sbSourcePort->value());
    m_pParameters->SetPcapFile(ui->lePcapFile->text());
    nRet = ui->wNet->Accept();
    return nRet;
}

bool CParameterSSHTunnelUI::CheckValidity(bool validity)
{
    return ui->wNet->CheckValidity(validity);
}
