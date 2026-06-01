// Author: Kang Lin <kl222@126.com>

#include <QMenu>
#include <QHeaderView>
#include <QDateTime>
#include <QVBoxLayout>
#include <QLoggingCategory>
#include "FrmViewServer.h"

static Q_LOGGING_CATEGORY(log, "ViewServer")
CFrmViewServer::CFrmViewServer(QWidget *parent)
    : QWidget{parent}
    , m_pToolBar(nullptr)
    , m_ptvClients(nullptr)
    , m_plbConnectCount(nullptr)
{
    bool check = false;
    qDebug(log) << Q_FUNC_INFO;
    QVBoxLayout* pLayout = new QVBoxLayout(this);
    setLayout(pLayout);
    m_pToolBar = new QToolBar(this);
    if(pLayout && m_pToolBar)
        pLayout->addWidget(m_pToolBar);
    m_ptvClients = new QTableView(this);
    if(pLayout && m_ptvClients) {
        pLayout->addWidget(m_ptvClients);

        m_ptvClients->installEventFilter(this);
        m_ptvClients->setModel(&m_ModelConnect);
        //m_ptvClients->verticalHeader()->hide();
        m_ptvClients->setSelectionMode(QAbstractItemView::SingleSelection);
        m_ptvClients->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_ptvClients->setEditTriggers(QAbstractItemView::NoEditTriggers);
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
        m_ptvClients->horizontalHeader()->setSectionResizeMode(
#if defined(DEBUG) && !defined(Q_OS_ANDROID)
            0,
#endif
            QHeaderView::Interactive);

        m_ptvClients->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
        check = connect(m_ptvClients, SIGNAL(customContextMenuRequested(const QPoint &)),
                        this, SLOT(slotContextMenuRequested(const QPoint&)));
        Q_ASSERT(check);
    }

    m_plbConnectCount = new QLabel(this);
    if(pLayout && m_plbConnectCount)
        pLayout->addWidget(m_plbConnectCount);
}

CFrmViewServer::~CFrmViewServer()
{
    qDebug(log) << Q_FUNC_INFO;
}

void CFrmViewServer::AddActionInToolBar(QAction *pAction)
{
    if(m_pToolBar)
        m_pToolBar->addAction(pAction);
}

void CFrmViewServer::slotConnectCount(int nTotal, int nConnect, int nDisconnect)
{
    QString szCount = tr("Connect count: Current: %1; Disconnect: %2; Total: %3")
    .arg(nConnect).arg(nDisconnect).arg(nTotal);
    m_plbConnectCount->setText(szCount);
}

void CFrmViewServer::slotConnected(const QString& szIp, const quint16 port)
{
    QList<QStandardItem*> lstItem;
    QStandardItem* pIP = new QStandardItem(szIp);
    lstItem << pIP;
    QStandardItem* pPort = new QStandardItem(QString::number(port));
    lstItem << pPort;
    QStandardItem* pTime = new QStandardItem(QDateTime::currentDateTime().toString(
        QLocale::system().dateTimeFormat()));
    lstItem << pTime;
    m_ModelConnect.appendRow(lstItem);
    m_ptvClients->resizeColumnToContents(0);
    m_ptvClients->resizeColumnToContents(1);
    m_ptvClients->resizeColumnToContents(2);
}

void CFrmViewServer::slotDisconnected(const QString& szIp, const quint16 port)
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

void CFrmViewServer::slotContextMenuRequested(const QPoint& pos)
{
    if(m_ModelConnect.rowCount() == 0)
        return;
    QMenu m;
    m.addAction(tr("Disconnect"), this, [this](){
        QItemSelectionModel* pSelect = m_ptvClients->selectionModel();
        QModelIndexList lstIndex = pSelect->selectedRows();
        foreach(auto index, lstIndex) {
            QString szIp = m_ModelConnect.item(index.row(), 0)->text();
            quint16 port = m_ModelConnect.item(index.row(), 1)->text().toUInt();
            emit sigDisconnect(szIp, port);
        }
    });
    QPoint p = m_ptvClients->mapToGlobal(pos);
    m.exec(p);
}