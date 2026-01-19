// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QVector>
#include <QAbstractTableModel>
#include "RecentDatabase.h"

class CParameterApp;
class CRecentModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit CRecentModel(CParameterApp* pPara, CRecentDatabase* pDb, QObject *parent = nullptr);

    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual bool removeRows(int row, int count, const QModelIndex &parent) override;

    void refresh(int limit = -1);
    bool addItem(const CRecentDatabase::Item &item);
    CRecentDatabase::Item getItem(const QModelIndex &index);

    enum ColumnNo {
        Name = 0,
        Protocol,
        Type,
        Time,
        ID,
        File,
        ColumnCount
    };

private:
    CRecentDatabase* m_pDatabase;
    QVector<CRecentDatabase::Item> m_Items;
};
