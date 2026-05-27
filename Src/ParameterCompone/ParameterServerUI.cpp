// Author: Kang Lin <kl222@126.com>

#include <QInputDialog>
#include <QMenu>
#include <QFileDialog>
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
    bool check = false;

    ui->setupUi(this);

    m_szFilteListPrompt = tr("The IP address and the netmask must be separated by a slash (/).") + "\n\n"
                          + tr("ag:") + "\n"
                          + "- 123.123.123.123/n " + tr("where n is any value between 0 and 32") + "\n"
                          + "- 123.123.123.123/255.255.255.255" + "\n"
                          + "- <ipv6-address>/n " + tr("where n is any value between 0 and 128") + "\n\n"
                          + tr("For IP version 4, accepts as well missing trailing components") + "\n"
                          + tr("(i.e., less than 4 octets, like \"192.168.1\"), followed or not by a dot. ") + "\n"
                          + tr("If the netmask is also missing in that case,") + "\n"
                          + tr("it is set to the number of octets actually passed") + "\n"
                          + tr("(in the example above, it would be 24, for 3 octets).") + "\n\n"
                          + tr("Add IP address and the netmask:");

    ui->lvBlacklist->setModel(&m_ModelBlack);
    check = connect(ui->lvBlacklist, SIGNAL(customContextMenuRequested(const QPoint&)),
                    this, SLOT(slotBlackListContextMenuRequested(const QPoint&)));
    Q_ASSERT(check);
    ui->lvWhtelist->setModel(&m_ModelWhite);
    check = connect(ui->lvWhtelist, SIGNAL(customContextMenuRequested(const QPoint&)),
                    this, SLOT(slotWhiteListContextMenuRequested(const QPoint&)));
    Q_ASSERT(check);
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
    ui->cbAnonymousLogin->setChecked(m_pPara->GetAnonymousLogin());
    ui->cbReadOnly->setChecked(m_pPara->GetReadOnly());
    ui->leRoot->setText(m_pPara->GetRoot());
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

    foreach (auto ip, m_pPara->GetBlacklist()) {
        m_ModelBlack.appendRow(new QStandardItem(ip));
    }

    foreach (auto ip, m_pPara->GetWhitelist()) {
        m_ModelWhite.appendRow(new QStandardItem(ip));
    }

    return 0;
}

int CParameterServerUI::Accept()
{
    int nRet = 0;
    nRet = ui->wNet->Accept();
    if(nRet) return nRet;

    m_pPara->SetAnonymousLogin(ui->cbAnonymousLogin->isChecked());
    m_pPara->SetReadOnly(ui->cbReadOnly->isChecked());
    m_pPara->SetRoot(ui->leRoot->text());
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

    QStringList lstBlack;
    for (int i = 0; i < m_ModelBlack.rowCount(); ++i) {
        lstBlack << m_ModelBlack.item(i)->text();
    }
    m_pPara->SetBlacklist(lstBlack);
    QStringList lstWhite;
    for (int i = 0; i < m_ModelWhite.rowCount(); ++i) {
        lstWhite << m_ModelWhite.item(i)->text();
    }
    m_pPara->SetWhitelist(lstWhite);

    return nRet;
}

bool CParameterServerUI::CheckValidity(bool validity)
{
    bool bRet = true;
    bRet = ui->wNet->CheckValidity(validity);
    if(!bRet) return bRet;
    return bRet;
}

void CParameterServerUI::on_pbRoot_clicked()
{
    QString szDir = QFileDialog::getExistingDirectory(this, QString(), ui->leRoot->text());
    if(szDir.isEmpty())
        return;
    ui->leRoot->setText(szDir);
}

void CParameterServerUI::on_cbListenAll_checkStateChanged(const Qt::CheckState &arg1)
{
    ui->lvListen->setEnabled(Qt::Checked != arg1);
}

void CParameterServerUI::slotWhiteListContextMenuRequested(const QPoint& pos)
{
    QMenu m;
    QItemSelectionModel* pSelect = ui->lvWhtelist->selectionModel();
    QModelIndexList lstIndex = pSelect->selectedRows();
    m.addAction(tr("Add"), this, SLOT(on_pbAddWhitelist_clicked()));
    if(!lstIndex.isEmpty()) {
        m.addAction(tr("Delete"), this, SLOT(on_pbDeleteWhitelist_clicked()));
    }

    QPoint p = ui->lvWhtelist->mapToGlobal(pos);
    m.exec(p);
}

void CParameterServerUI::slotBlackListContextMenuRequested(const QPoint& pos)
{
    QMenu m;
    QItemSelectionModel* pSelect = ui->lvBlacklist->selectionModel();
    QModelIndexList lstIndex = pSelect->selectedRows();
    m.addAction(tr("Add"), this, SLOT(on_pbAddBlacklist_clicked()));
    if(!lstIndex.isEmpty()) {
        m.addAction(tr("Delete"), this, SLOT(on_pbDeleteBlacklist_clicked()));
    }

    QPoint p = ui->lvBlacklist->mapToGlobal(pos);
    m.exec(p);
}

void CParameterServerUI::on_pbAddWhitelist_clicked()
{
    QString szIp = QInputDialog::getText(this, tr("Add whilte list"), m_szFilteListPrompt);
    QStandardItem* item = new QStandardItem(szIp);
    m_ModelWhite.appendRow(item);
}

void CParameterServerUI::on_pbDeleteWhitelist_clicked()
{
    QItemSelectionModel* pSelect = ui->lvBlacklist->selectionModel();
    QModelIndexList lstIndex = pSelect->selectedRows();
    foreach(auto idx, lstIndex) {
        m_ModelWhite.removeRow(idx.row());
    }
}

void CParameterServerUI::on_pbAddBlacklist_clicked()
{
    QString szIp = QInputDialog::getText(this, tr("Add black list"), m_szFilteListPrompt);
    QStandardItem* item = new QStandardItem(szIp);
    m_ModelBlack.appendRow(item);
}

void CParameterServerUI::on_pbDeleteBlacklist_clicked()
{
    QItemSelectionModel* pSelect = ui->lvBlacklist->selectionModel();
    if(!pSelect) return;
    QModelIndexList lstIndex = pSelect->selectedRows();
    foreach(auto idx, lstIndex) {
        m_ModelBlack.removeRow(idx.row());
    }
}
