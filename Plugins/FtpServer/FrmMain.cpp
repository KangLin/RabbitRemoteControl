#include <QLocale>
#include <QLoggingCategory>

#include "FrmMain.h"
#include "ui_FrmMain.h"
#include "OperateFtpServer.h"

static Q_LOGGING_CATEGORY(log, "FtpServer.FrmMain")
CFrmMain::CFrmMain(COperateFtpServer *pOperate, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CFrmMain)
    , m_ModelConnect(this)
{
    bool check = false;
    ui->setupUi(this);

    ui->tvConnect->setModel(&m_ModelConnect);
    //ui->tvConnect->verticalHeader()->hide();
    ui->tvConnect->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->tvConnect->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tvConnect->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ModelConnect.setHorizontalHeaderItem(0, new QStandardItem(tr("IP")));
    m_ModelConnect.setHorizontalHeaderItem(1, new QStandardItem(tr("Port")));
    m_ModelConnect.setHorizontalHeaderItem(2, new QStandardItem(tr("Time")));
    //必须在 setModel 后,才能应用
    /*第二个参数可以为：
    QHeaderView::Interactive     ：0 用户可设置，也可被程序设置成默认大小
    QHeaderView::Fixed           ：2 用户不可更改列宽
    QHeaderView::Stretch         ：1 根据空间，自动改变列宽，用户与程序不能改变列宽
    QHeaderView::ResizeToContents：3 根据内容改变列宽，用户与程序不能改变列宽
    */
    ui->tvConnect->horizontalHeader()->setSectionResizeMode(
#if defined(DEBUG) && !defined(Q_OS_ANDROID)
        0,
#endif
        QHeaderView::Interactive);

    check = connect(ui->tvConnect, SIGNAL(customContextMenuRequested(const QPoint &)),
                    this, SLOT(slotContextMenuRequested(const QPoint&)));
    Q_ASSERT(check);
    ui->pbSettings->setText(pOperate->m_pActionSettings->text());
    ui->pbSettings->setIcon(pOperate->m_pActionSettings->icon());
    check = connect(ui->pbSettings, &QPushButton::clicked,
                    pOperate->m_pActionSettings, &QAction::trigger);
    Q_ASSERT(check);
    ui->pbStart->setText(pOperate->m_pStart->text());
    ui->pbStart->setIcon(pOperate->m_pStart->icon());
    check = connect(ui->pbStart, &QPushButton::clicked,
                    pOperate->m_pStart, &QAction::trigger);
    Q_ASSERT(check);
    check = connect(pOperate->m_pStart, &QAction::toggled,
                    this, [this, pOperate](bool checked) {
        ui->pbStart->setText(pOperate->m_pStart->text());
        ui->pbStart->setIcon(pOperate->m_pStart->icon());
        ui->pbStart->setChecked(checked);
    });
    Q_ASSERT(check);
    slotConnectCount(0, 0, 0);
}

CFrmMain::~CFrmMain()
{
    delete ui;
}

void CFrmMain::slotConnectCount(int nTotal, int nConnect, int nDisconnect)
{
    QString szCount = tr("Connect count: Current: %1; Disconnect: %2; Total: %3")
                          .arg(nConnect).arg(nDisconnect).arg(nTotal);
    ui->lbConnectCount->setText(szCount);
}

void CFrmMain::slotConnected(const QString& szIp, const quint16 port)
{
    QList<QStandardItem*> lstItem;
    QStandardItem* pIP = new QStandardItem(szIp);
    lstItem << pIP;
    QStandardItem* pPort = new QStandardItem(QString::number(port));
    lstItem << pPort;
    QStandardItem* pTime = new QStandardItem(QTime::currentTime().toString(
        QLocale::system().timeFormat()));
    lstItem << pTime;
    m_ModelConnect.appendRow(lstItem);
    ui->tvConnect->resizeColumnToContents(0);
    ui->tvConnect->resizeColumnToContents(1);
    ui->tvConnect->resizeColumnToContents(2);
}

void CFrmMain::slotDisconnected(const QString& szIp, const quint16 port)
{
    for (int row = 0; row < m_ModelConnect.rowCount(); row++) {
        QModelIndex index = m_ModelConnect.index(row, 0);
        QString ip = m_ModelConnect.data(index).toString();
        QModelIndex indexPort = m_ModelConnect.index(row, 1);
        QString szPort = m_ModelConnect.data(indexPort).toString();
        if (szIp  == ip && szPort == QString::number(port)) {
            m_ModelConnect.removeRows(row, 1);
            break;
        }
    }
}

void CFrmMain::slotContextMenuRequested(const QPoint& pos)
{
    QMenu m;
    m.addAction(tr("Disconnect"), [this](){
        QItemSelectionModel* pSelect = ui->tvConnect->selectionModel();
        QModelIndexList lstIndex = pSelect->selectedRows();
        foreach(auto index, lstIndex) {
            QString szIp = m_ModelConnect.item(index.row(), 0)->text();
            quint16 port = m_ModelConnect.item(index.row(), 1)->text().toUInt();
            emit sigDisconnect(szIp, port);
        }
    });
    QPoint p = ui->tvConnect->mapToGlobal(pos);
    m.exec(p);
}
