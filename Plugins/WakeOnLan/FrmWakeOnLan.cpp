#include "FrmWakeOnLan.h"
#include "ui_FrmWakeOnLan.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "WakeOnLan.CFrmWakeOnLan")
CFrmWakeOnLan::CFrmWakeOnLan(QWidget *parent)
    : CParameterUI(parent)
    , ui(new Ui::CFrmWakeOnLan)
    , m_pParameter(nullptr)
{
    ui->setupUi(this);
    bool check = false;
    check = connect(ui->pbCancel, SIGNAL(clicked()), this, SIGNAL(sigCancel()));
    Q_ASSERT(check);
}

CFrmWakeOnLan::~CFrmWakeOnLan()
{
    qDebug(log) << "CFrmWakeOnLan::~CFrmWakeOnLan()";
    delete ui;
}

void CFrmWakeOnLan::on_pbOK_clicked()
{
    int nRet = Accept();
    if(nRet) return;
    emit sigOk();
}

int CFrmWakeOnLan::SetParameter(CParameter *pParameter)
{
    int nRet = 0;
    m_pParameter = qobject_cast<CParameterWakeOnLan*>(pParameter);
    if(m_pParameter)
        nRet = ui->wdgWakeOnLan->SetParameter(m_pParameter);
    return nRet;
}

int CFrmWakeOnLan::Accept()
{
    bool bRet = 0;
    bRet = ui->wdgWakeOnLan->CheckValidity(true);
    if(!bRet) return -1;
    int nRet = ui->wdgWakeOnLan->Accept();
    return nRet;
}
