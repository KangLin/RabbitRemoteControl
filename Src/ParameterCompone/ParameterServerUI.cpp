// Author: Kang Lin <kl222@126.com>

#include <QInputDialog>
#include <QMenu>
#include <QStandardItem>
#include <QNetworkInterface>
#include <QLoggingCategory>
#include "ParameterServerUI.h"
#include "ui_ParameterServerUI.h"

static Q_LOGGING_CATEGORY(log, "Parameter.ServerUI")
CParameterServerUI::CParameterServerUI(QWidget *parent)
    : CParameterUI(parent)
    , ui(new Ui::CParameterServerUI)
    , m_pPara(nullptr)
{
    ui->setupUi(this);
    setWindowIcon(QIcon::fromTheme("network-wired"));
    setWindowTitle(tr("Connect"));
    m_pBlacklist = new CParameterFilterUI(ui->tbFilter);
    if(m_pBlacklist) {
        ui->tbFilter->addTab(m_pBlacklist, tr("Blacklist"));
    }
    m_pWhitelist = new CParameterFilterUI(ui->tbFilter);
    if(m_pWhitelist) {
        ui->tbFilter->addTab(m_pWhitelist, tr("Whitelist"));
    }
}

CParameterServerUI::~CParameterServerUI()
{
    delete ui;
}

int CParameterServerUI::SetParameter(CParameter *pParameter)
{
    m_pPara = qobject_cast<CParameterServer*>(pParameter);
    Q_ASSERT(m_pPara);

    m_pPara->m_Net.SetHost(tr("Use the following \"Enable listen at all network interface\""));
    ui->wNet->SetParameter(&m_pPara->m_Net);
    ui->sbConnectCount->setValue(m_pPara->GetConnectCount());
    ui->sbConnectCount->setToolTip(tr("-1: Enable all\n 0: Disable all\n>0: Connect count"));
    ui->spAuthenticateAttempts->setValue(m_pPara->GetAuthenticateAttempts());
    ui->tmAuthenticateTimeOut->setTime(QTime(0,0).addMSecs(m_pPara->GetAuthenticateTime()));

    ui->cbListenAll->setChecked(m_pPara->GetListenAll());
    ui->lvListen->setModel(&m_ModelNetWorkInterface);
    foreach(auto iface, QNetworkInterface::allInterfaces()) {
        qDebug(log) << iface;
        auto entry = iface.addressEntries();
        if(iface.flags() & QNetworkInterface::IsLoopBack)
            continue;
        /*if(!(iface.flags() & QNetworkInterface::CanBroadcast))
            continue;//*/
        foreach(auto e, entry) {
            if(!e.broadcast().isNull()) {
                QStandardItem* item = new QStandardItem(e.ip().toString());
                item->setCheckable(true);
                m_ModelNetWorkInterface.appendRow(item);
            }
        }
    }
    foreach(auto ip, m_pPara->GetListen()) {
        for (int row = 0; row < m_ModelNetWorkInterface.rowCount(); row++) {
            QModelIndex index = m_ModelNetWorkInterface.index(row, 0);
            QString szIp = m_ModelNetWorkInterface.data(index).toString();
            if (szIp  == ip) {
                m_ModelNetWorkInterface.item(row)->setCheckState(Qt::Checked);
                break;
            }
        }
    }

    if(m_pWhitelist)
        m_pWhitelist->SetParameter(&m_pPara->m_WhiteFilter);
    if(m_pBlacklist)
        m_pBlacklist->SetParameter(&m_pPara->m_BlackFilter);

    return 0;
}

int CParameterServerUI::Accept()
{
    int nRet = 0;
    nRet = ui->wNet->Accept();
    if(nRet) return nRet;

    m_pPara->SetConnectCount(ui->sbConnectCount->value());
    m_pPara->SetAuthenticateAttempts(ui->spAuthenticateAttempts->value());
    m_pPara->SetAuthenticateTime(QTime(0,0).msecsTo(ui->tmAuthenticateTimeOut->time()));
    m_pPara->SetListenAll(ui->cbListenAll->isChecked());
    QStringList lstInterface;
    for (int row = 0; row < m_ModelNetWorkInterface.rowCount(); row++) {
        auto item = m_ModelNetWorkInterface.item(row);
        if(item->checkState() == Qt::Checked) {
            lstInterface << item->text();
        }
    }
    m_pPara->SetListen(lstInterface);

    if(m_pWhitelist)
        m_pWhitelist->Accept();
    if(m_pBlacklist)
        m_pBlacklist->Accept();

    return nRet;
}

bool CParameterServerUI::CheckValidity(bool validity)
{
    bool bRet = true;
    bRet = ui->wNet->CheckValidity(validity);
    if(!bRet) return bRet;
    return bRet;
}

void CParameterServerUI::on_cbListenAll_checkStateChanged(const Qt::CheckState &arg1)
{
    ui->lvListen->setEnabled(Qt::Checked != arg1);
}
