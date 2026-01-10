// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QAbstractTableModel>
#include "HistoryDatabase.h"
#include "ParameterHistory.h"

class CHistoryModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit CHistoryModel(CHistoryDatabase* pDatabase, CParameterHistory* pPara, QObject *parent = nullptr);
    ~CHistoryModel();

    enum Column {
        ColumnVisitTime = 0,
        ColumnTitle,
        ColumnUrl,
        ColumnCount,

        ColumnLastVisitTime,
        ColumnVisitCount,
    };
    Q_ENUM(Column)

    // Header
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // 自定义方法
    void refresh();    
    HistoryItem getItem(const QModelIndex &index) const;
    bool removeDomainItems(const QString& szDomain);
    bool removeItems(const QString& url);

private:
    QList<HistoryItem> m_historyItems;
    CHistoryDatabase* m_pDatabase;
    CParameterHistory* m_pPara;
};
