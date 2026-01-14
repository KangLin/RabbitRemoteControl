// Author: Kang Lin <kl222@126.com>

#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QMenu>
#include <QLoggingCategory>
#include <QToolBar>
#include <QLayout>
#include <QVBoxLayout>
#include "FrmHistory.h"
#include "ui_FrmHistory.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.History")
CFrmHistory::CFrmHistory(CHistoryDatabase *pDatabase,
                         CParameterWebBrowser *pPara,
                         QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CFrmHistory)
    , m_pModelHistory(nullptr)
    , m_pPara(pPara)
{
    ui->setupUi(this);
    ui->treeView->hide();

    setWindowTitle(tr("History"));

    QToolBar* pToolBar = new QToolBar(this);
    QAction* pRefresh = pToolBar->addAction(QIcon::fromTheme("view-refresh"), tr("Refresh"),
                        this, [&] {
        if(m_pModelHistory)
            m_pModelHistory->refresh();
    });
    if(pRefresh) {
        pRefresh->setToolTip(pRefresh->text());
        pRefresh->setStatusTip(pRefresh->text());
    }
    QAction* pSettings = pToolBar->addAction(QIcon::fromTheme("system-settings"), tr("Settings"),
                        this, [&]() {
        //TODO: add settings ui
    });
    if(pSettings) {
        pSettings->setToolTip(pSettings->text());
        pSettings->setStatusTip(pSettings->text());
    }

    QLayout* pLayout = nullptr;
    pLayout = new QVBoxLayout(this);
    pLayout->addWidget(pToolBar);
    pLayout->addWidget(ui->splitter);
    setLayout(pLayout);

    bool check = false;
    check = connect(ui->tableView, &QTableView::customContextMenuRequested,
                    this, &CFrmHistory::slotCustomContextMenuRequested);
    //ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    if(pDatabase) {
        m_pModelHistory = new CHistoryModel(pDatabase, m_pPara, this);
        if(m_pModelHistory) {
            ui->tableView->setModel(m_pModelHistory);
        }
    }

    ui->tableView->resizeColumnsToContents();

    resize(m_pPara->GetWindowSize());
}

CFrmHistory::~CFrmHistory()
{
    m_pPara->SetWindowSize(this->size());
    delete ui;
}

void CFrmHistory::on_tableView_doubleClicked(const QModelIndex &index)
{
    if(!index.isValid() || !m_pModelHistory) return;
    auto item = m_pModelHistory->getItem(index);
    emit sigOpenUrl(item.url);
}

void CFrmHistory::slotCustomContextMenuRequested(const QPoint &pos)
{
    auto index = ui->tableView->indexAt(pos);
    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();
    QModelIndexList selectedIndexes;
    if(selectionModel)
        selectedIndexes = selectionModel->selectedRows();

    QMenu menu(this);

    if (selectedIndexes.count() > 1) {
        // 有选中行的情况
        int selectedCount = selectedIndexes.size();

        // 批量打开
        QAction *openSelectedAction = menu.addAction(
            QIcon::fromTheme("document-open"), tr("Open the selected %1 urls").arg(selectedCount));
        if(openSelectedAction) {
            connect(openSelectedAction, &QAction::triggered, this, [this, selectedIndexes]() {
                onOpenSelectedUrls(selectedIndexes);
            });
        }

        // 批量删除
        QAction *deleteSelectedAction = menu.addAction(
            QIcon::fromTheme("edit-delete"), tr("Delete the selected %1 urls").arg(selectedCount));
        if(deleteSelectedAction) {
            connect(deleteSelectedAction, &QAction::triggered, this, [this, selectedIndexes]() {
                onDeleteSelectedItems(selectedIndexes);
            });
        }

        menu.addSeparator();

        // 全选
        QAction *selectAllAction = menu.addAction(
            QIcon(":/icons/select_all.png"), tr("All selected"));
        if(selectAllAction)
            connect(selectAllAction, &QAction::triggered, ui->tableView, &QTableView::selectAll);

        // 取消选择
        QAction *deselectAction = menu.addAction(
            QIcon(":/icons/deselect.png"), tr("Cancel selected"));
        if(deselectAction)
            connect(deselectAction, &QAction::triggered,
                    selectionModel, &QItemSelectionModel::clearSelection);
    } else if (index.isValid()) {
        // 获取当前行的数据
        QString url = ui->tableView->model()->data(
                    index.siblingAtColumn(CHistoryModel::ColumnUrl)).toString();
        QString title = ui->tableView->model()->data(
                  index.siblingAtColumn(CHistoryModel::ColumnTitle)).toString();

        // 打开操作
        QAction *openAction = menu.addAction(QIcon::fromTheme("document-open"),
                                             tr("Open"));
        if(openAction)
            connect(openAction, &QAction::triggered, this, [this, url]() {
                emit sigOpenUrl(url);
            });

        // 在新标签页中打开
        QAction *openNewTabAction = menu.addAction(
            QIcon::fromTheme("document-open"),
            tr("Open in new tab"));
        if(openNewTabAction)
            connect(openNewTabAction, &QAction::triggered, this, [this, url]() {
                emit sigOpenUrlInNewTab(url);
            });

        // 复制URL
        QAction *copyUrlAction = menu.addAction(QIcon::fromTheme("edit-copy"),
                                                tr("Copy url"));
        if(copyUrlAction)
            connect(copyUrlAction, &QAction::triggered, this, [url]() {
                QApplication::clipboard()->setText(url);
            });

        // 复制标题
        QAction *copyTitleAction = menu.addAction(
            QIcon::fromTheme("edit-copy"),
            tr("Copy title"));
        if(copyTitleAction)
            connect(copyTitleAction, &QAction::triggered, this, [title]() {
                QApplication::clipboard()->setText(title);
            });

        menu.addSeparator();

        // 删除操作
        QAction *deleteAction = menu.addAction(
            QIcon::fromTheme("edit-delete"), tr("Delete"));
        if(deleteAction)
            connect(deleteAction, &QAction::triggered, this, [this, index]() {
                onDeleteHistoryItem(index);
            });

        QAction *deleteAllAction = menu.addAction(
            QIcon::fromTheme("edit-delete"),
            tr("Delete all urls %1").arg(title.left(30)));
        if(deleteAllAction)
            connect(deleteAllAction, &QAction::triggered, this, [this, url]() {
                m_pModelHistory->removeItems(url);
            });

        // 删除所有类似网站
        QString domain = extractDomain(url);
        if (!domain.isEmpty()) {
            QAction *deleteDomainAction = menu.addAction(
                QIcon::fromTheme("edit-delete"),
                tr("Delete all urls from %1").arg(domain));
            if(deleteDomainAction)
                connect(deleteDomainAction, &QAction::triggered, this, [this, domain]() {
                    onDeleteHistoryByDomain(domain);
                });
        }

        menu.addSeparator();

        // 属性/详细信息
        QAction *propertiesAction = menu.addAction(
            QIcon::fromTheme("document-properties"), tr("Properties"));
        if(propertiesAction)
            connect(propertiesAction, &QAction::triggered, this, [this, index]() {
                onShowHistoryProperties(index);
            });
    }

    menu.addSeparator();

    // 总是显示的菜单项
    QAction *refreshAction = menu.addAction(
        QIcon::fromTheme("view-refresh"), tr("Refresh"));
    if(refreshAction)
        connect(refreshAction, &QAction::triggered, this, [&]() {
            if(m_pModelHistory)
                m_pModelHistory->refresh();
        });

    QAction *clearAllAction = menu.addAction(
        QIcon::fromTheme("edit-clear"), tr("Clear all urls"));
    if(clearAllAction)
        connect(clearAllAction, &QAction::triggered, this, [&]() {
            if(m_pModelHistory->removeRows(0, m_pModelHistory->rowCount()));
        });

    // 设置菜单
    // QAction *settingsAction = menu.addAction(
    //     QIcon::fromTheme("system-settings"), tr("Settings"));
    // connect(settingsAction, &QAction::triggered, this, &CFrmHistory::showHistorySettings);

    // 4. 显示菜单
    menu.exec(ui->tableView->viewport()->mapToGlobal(pos));
}

QString CFrmHistory::extractDomain(const QString &url)
{
    QUrl qurl(url);
    if (qurl.isValid()) {
        QString host = qurl.host();
        // 移除 www. 前缀
        if (host.startsWith("www.")) {
            host = host.mid(4);
        }
        return host;
    }
    return QString();
}

void CFrmHistory::onDeleteHistoryItem(const QModelIndex &index)
{
    if (!index.isValid() || !m_pModelHistory) {
        return;
    }

    QString title = ui->tableView->model()->data(
                 index.siblingAtColumn(CHistoryModel::ColumnTitle)).toString();
    if(title.isEmpty()) {
        title =  ui->tableView->model()->data(
                   index.siblingAtColumn(CHistoryModel::ColumnUrl)).toString();
    }
    // 确认对话框
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Delete the url"),
        tr("Are you sure you want to delete the url \"%1\"?").arg(title),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        // 从模型中删除
        if (m_pModelHistory->removeRow(index.row())) {
            qDebug(log) << "History item deleted";
        }
    }
}

void CFrmHistory::onDeleteHistoryByDomain(const QString &domain)
{
    if (domain.isEmpty() || !m_pModelHistory) {
        return;
    }

    // 确认对话框
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Delete the url"),
        tr("Are you sure you want to delete all url from \"%1\"?").arg(domain),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        m_pModelHistory->removeDomainItems(domain);
    }
}

void CFrmHistory::onShowHistoryProperties(const QModelIndex &index)
{
    if (!index.isValid() || !m_pModelHistory) {
        return;
    }

    // 获取详细数据
    auto item = m_pModelHistory->getItem(index);
    // 创建详细信息对话框
    QString details = QString(
                          "<h3>%1</h3>"
                          "<table border='0' cellspacing='5'>"
                          "<tr><td><b>%2</b></td><td>%3</td></tr>"
                          "<tr><td><b>%4</b></td><td>%5</td></tr>"
                          "<tr><td><b>%6</b></td><td>%7</td></tr>"
                          "<tr><td><b>%8</b></td><td>%9</td></tr>"
                          "</table>")
                          .arg(item.title.toHtmlEscaped())
                          .arg(tr("Url:"))
                          .arg(item.url.toHtmlEscaped())
                          .arg(tr("Last Visit Time:"))
                          .arg(item.lastVisitTime.toString(QLocale::system().dateFormat()))
                          .arg(tr("Visit Time:"))
                          .arg(item.visitTime.toString(QLocale::system().dateFormat()))
                          .arg(tr("Visit Count:"))
                          .arg(item.visitCount)
                          ;

    QMessageBox::information(this, tr("Properties"), details);
}

void CFrmHistory::onOpenSelectedUrls(const QModelIndexList &indexes)
{
    if (indexes.isEmpty()) return;
    for (const QModelIndex &index : indexes) {
        QString url = ui->tableView->model()->data(
                                                index.siblingAtColumn(CHistoryModel::ColumnUrl)).toString();
        if (!url.isEmpty()) {
            emit sigOpenUrlInNewTab(url);
        }
    }
}

void CFrmHistory::onDeleteSelectedItems(const QModelIndexList &indexes)
{
    if (indexes.isEmpty() || !m_pModelHistory) return;

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Delete the urls"),
        tr("Are you sure you want to delete the selected %1 urls?").arg(indexes.size()),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        // 按行号降序排序，这样删除时索引不会乱
        QList<int> rows;
        for (const QModelIndex &index : indexes) {
            rows.append(index.row());
        }

        // 降序排序
        std::sort(rows.begin(), rows.end(), std::greater<int>());

        // 批量删除
        for (int row : rows) {
            m_pModelHistory->removeRow(row);
        }

        qDebug(log) << "Deleted" << indexes.size() << "history items";
    }
}
