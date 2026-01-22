// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QWidget>
#include <QDateEdit>
#include "HistoryModel.h"

namespace Ui {
class CFrmHistory;
}

class CFrmHistory : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmHistory(CParameterWebBrowser* pPara,
                         QWidget *parent = nullptr);
    ~CFrmHistory();

Q_SIGNALS:
    void sigOpenUrl(const QString& url);
    void sigOpenUrlInNewTab(const QString& url);

private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);
    void slotCustomContextMenuRequested(const QPoint& pos);
    void slotRefresh();
    void slotImport();
    void slotExport();
private:
    QString extractDomain(const QString &url);
    void onDeleteHistoryItem(const QModelIndex &index);
    void onDeleteHistoryByDomain(const QString &domain);
    void onShowHistoryProperties(const QModelIndex &index);
    void onOpenSelectedUrls(const QModelIndexList &indexes);
    void onDeleteSelectedItems(const QModelIndexList &indexes);

private:
    Ui::CFrmHistory *ui;
    CHistoryModel* m_pModelHistory;
    CParameterWebBrowser* m_pPara;
    QDateEdit* m_pDateStart;
    QDateEdit* m_pDateEnd;
};
