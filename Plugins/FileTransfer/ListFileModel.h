#ifndef LISTFILEMODEL_H
#define LISTFILEMODEL_H

#include <QAbstractTableModel>

class CListFileModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit CListFileModel(QObject *parent = nullptr);
    
    enum class ColumnValue {
        LocalFiles = 0,
        Direction,
        RemoteFiles,
        FileSize,
        Speed,
        State,
        Time,
        Priority,
        Explanation,
        End
    };
    Q_ENUM(ColumnValue)

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    int m_Columns;
    int m_Rows;
};

#endif // LISTFILEMODEL_H
