#ifndef WAKEONLANMODEL_H
#define WAKEONLANMODEL_H

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <vector>

#include "ParameterWakeOnLan.h"

class CWakeOnLanModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit CWakeOnLanModel(QObject *parent = nullptr);
    virtual ~CWakeOnLanModel();

    int AddItem(QSharedPointer<CParameterWakeOnLan> para);
    int Load(QSettings &set, CParameterClient *pClient);
    int Save(QSettings &set);

    QSharedPointer<CParameterWakeOnLan> GetData(const QModelIndex &index);

    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual int columnCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual bool removeRows(int row, int count, const QModelIndex &parent) override;
    virtual void sort(int column, Qt::SortOrder order) override;

    std::vector<QSharedPointer<CParameterWakeOnLan> > m_Data;
    QMap<int, Qt::SortOrder> m_Sort;

private Q_SLOTS:
    void slotHostStateChanged();

private:
    int m_Colume;
};

#endif // WAKEONLANMODEL_H
