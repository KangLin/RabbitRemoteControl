#ifndef DOCKWDGFAVORITE_H
#define DOCKWDGFAVORITE_H

#include <QDockWidget>
#include <QStandardItemModel>

namespace Ui {
class CDockWdgFavorite;
}

class CDockWdgFavorite : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit CDockWdgFavorite(QWidget *parent = nullptr);
    virtual ~CDockWdgFavorite();

    int AddFavorite(const QString& szName, const QString &szFile, QString szGroup);
    
Q_SIGNALS:
    void sigConnect(const QString &szFile, bool bOpenSettings);

private slots:
    void on_tvFavorite_clicked(const QModelIndex &index);
    void on_tvFavorite_doubleClicked(const QModelIndex &index);
    void slotCustomContextMenu(const QPoint &pos);
    void slotConnect();
    void slotOpenConnect();
    void slotDelete();
    void slotNewGroup();

private:
    int Save();

private:
    Ui::CDockWdgFavorite *ui;
    QStandardItemModel* m_pModel;
    QString m_szSaveFile;
};

#endif // DOCKWDGFAVORITE_H
