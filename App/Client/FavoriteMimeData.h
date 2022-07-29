#ifndef CFAVORITEMIMEDATA_H_KL_2021_12_17
#define CFAVORITEMIMEDATA_H_KL_2021_12_17

#include <QMimeData>
#include <QModelIndexList>

class CFavoriteMimeData : public QMimeData
{
    Q_OBJECT
    
public:
    CFavoriteMimeData(QObject *parent = nullptr);
    QModelIndexList m_Items;
};

#endif // CFAVORITEMIMEDATA_H_KL_2021_12_17
