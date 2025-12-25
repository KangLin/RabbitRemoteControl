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
    m.addAction(tr("Add"), [this](){
        QString szIp = QInputDialog::getText(this, tr("Add whilte list"), tr("Add ip address:"));
        QStandardItem* item = new QStandardItem(szIp);
        m_ModelWhite.appendRow(item);
    });
    if(!lstIndex.isEmpty()) {
        m.addAction(tr("Remove"), [this, lstIndex]() {
            foreach(auto idx, lstIndex) {
                m_ModelWhite.removeRow(idx.row());
            }
        });
    }

    QPoint p = ui->lvWhtelist->mapToGlobal(pos);
    m.exec(p);
}

void CDlgSettings::slotBlackListContextMenuRequested(const QPoint& pos)
{
    QMenu m;
    QItemSelectionModel* pSelect = ui->lvBlacklist->selectionModel();
    QModelIndexList lstIndex = pSelect->selectedRows();
    m.addAction(tr("Add"), [this](){
        QString szIp = QInputDialog::getText(this, tr("Add black list"), tr("Add ip address:"));
        QStandardItem* item = new QStandardItem(szIp);
        m_ModelBlack.appendRow(item);
    });
    if(!lstIndex.isEmpty()) {
        m.addAction(tr("Remove"), [this, lstIndex]() {
            foreach(auto idx, lstIndex) {
                m_ModelBlack.removeRow(idx.row());
            }
        });
    }

    QPoint p = ui->lvBlacklist->mapToGlobal(pos);
    m.exec(p);
}
