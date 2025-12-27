// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#include <QFileDialog>
#include <QLoggingCategory>
#include <QStandardItem>
#include <QNetworkInterface>
#include <QMenu>
#include <QInputDialog>
#include "DlgSettings.h"
#include "ui_DlgSettings.h"

static Q_LOGGING_CATEGORY(log, "FtpServer.DlgSettings")
CDlgSettings::CDlgSettings(QSharedPointer<CParameterFtpServer> para, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgSettings)
    , m_Para(para)
{
    bool check = false;
    ui->setupUi(this);
    Q_ASSERT(m_Para);
    
    ui->sbPort->setValue(m_Para->GetPort());
    ui->leUser->setText(m_Para->GetUser());
    ui->lePassword->setText(m_Para->GetPassword());
    ui->leRoot->setText(m_Para->GetRoot());
    ui->cbAnonmousLogin->setChecked(m_Para->GetAnonymousLogin());
    ui->cbReadOnly->setChecked(m_Para->GetReadOnly());
    ui->sbConnectCount->setValue(m_Para->GetConnectCount());
    ui->sbConnectCount->setToolTip(tr("-1: Enable all\n 0: Disable all\n>0: Connect count"));

    ui->cbListenAll->setChecked(m_Para->GetListenAll());
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
    foreach(auto ip, m_Para->GetListen()) {
        for (int row = 0; row < m_ModelNetWorkInterface.rowCount(); row++) {
            QModelIndex index = m_ModelNetWorkInterface.index(row, 0);
            QString szIp = m_ModelNetWorkInterface.data(index).toString();
            if (szIp  == ip) {
                m_ModelNetWorkInterface.item(row)->setCheckState(Qt::Checked);
                break;
            }
        }
    }

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
    foreach (auto ip, m_Para->GetBlacklist()) {
        m_ModelBlack.appendRow(new QStandardItem(ip));
    }
    ui->lvWhtelist->setModel(&m_ModelWhite);
    check = connect(ui->lvWhtelist, SIGNAL(customContextMenuRequested(const QPoint&)),
                    this, SLOT(slotWhiteListContextMenuRequested(const QPoint&)));
    foreach (auto ip, m_Para->GetWhitelist()) {
        m_ModelWhite.appendRow(new QStandardItem(ip));
    }
    Q_ASSERT(check);
}

CDlgSettings::~CDlgSettings()
{
    delete ui;
}

void CDlgSettings::on_pbRoot_clicked()
{
    QString szDir = QFileDialog::getExistingDirectory(this, QString(), ui->leRoot->text());
    if(szDir.isEmpty())
        return;
    ui->leRoot->setText(szDir);
}

void CDlgSettings::accept()
{
    m_Para->SetPort(ui->sbPort->value());
    m_Para->SetUser(ui->leUser->text());
    m_Para->SetPassword(ui->lePassword->text());
    m_Para->SetRoot(ui->leRoot->text());
    m_Para->SetAnonymousLogin(ui->cbAnonmousLogin->isChecked());
    m_Para->SetReadOnly(ui->cbReadOnly->isChecked());
    m_Para->SetConnectCount(ui->sbConnectCount->value());

    m_Para->SetListenAll(ui->cbListenAll->isChecked());
    QStringList lstInterface;
    for (int row = 0; row < m_ModelNetWorkInterface.rowCount(); row++) {
        auto item = m_ModelNetWorkInterface.item(row);
        if(item->checkState() == Qt::Checked) {
            lstInterface << item->text();
        }
    }
    m_Para->SetListen(lstInterface);

    QStringList lstBlack;
    for (int i = 0; i < m_ModelBlack.rowCount(); ++i) {
       lstBlack << m_ModelBlack.item(i)->text();
    }
    m_Para->SetBlacklist(lstBlack);
    QStringList lstWhite;
    for (int i = 0; i < m_ModelWhite.rowCount(); ++i) {
        lstWhite << m_ModelWhite.item(i)->text();
    }
    m_Para->SetWhitelist(lstWhite);

    QDialog::accept();
}

void CDlgSettings::on_cbAnonmousLogin_checkStateChanged(const Qt::CheckState &arg1)
{
    bool bEnable = (Qt::Checked != arg1);
    ui->lePassword->setEnabled(bEnable);
    ui->leUser->setEnabled(bEnable);
}

void CDlgSettings::on_cbListenAll_checkStateChanged(const Qt::CheckState &arg1)
{
    ui->lvListen->setEnabled(Qt::Checked != arg1);
}

void CDlgSettings::slotWhiteListContextMenuRequested(const QPoint& pos)
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

void CDlgSettings::slotBlackListContextMenuRequested(const QPoint& pos)
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

void CDlgSettings::on_pbAddWhitelist_clicked()
{
    QString szIp = QInputDialog::getText(this, tr("Add whilte list"), m_szFilteListPrompt);
    QStandardItem* item = new QStandardItem(szIp);
    m_ModelWhite.appendRow(item);
}

void CDlgSettings::on_pbDeleteWhitelist_clicked()
{
    QItemSelectionModel* pSelect = ui->lvBlacklist->selectionModel();
    QModelIndexList lstIndex = pSelect->selectedRows();
    foreach(auto idx, lstIndex) {
        m_ModelWhite.removeRow(idx.row());
    }
}

void CDlgSettings::on_pbAddBlacklist_clicked()
{
    QString szIp = QInputDialog::getText(this, tr("Add black list"), m_szFilteListPrompt);
    QStandardItem* item = new QStandardItem(szIp);
    m_ModelBlack.appendRow(item);
}

void CDlgSettings::on_pbDeleteBlacklist_clicked()
{
    QItemSelectionModel* pSelect = ui->lvBlacklist->selectionModel();
    if(!pSelect) return;
    QModelIndexList lstIndex = pSelect->selectedRows();
    foreach(auto idx, lstIndex) {
        m_ModelBlack.removeRow(idx.row());
    }
}
