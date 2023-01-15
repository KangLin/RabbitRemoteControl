#ifndef FRMOPENCONNECT_H
#define FRMOPENCONNECT_H

#include <QWidget>
#include <QStandardItemModel>
#include <QToolBar>
#include <QTableView>
#include "Client.h"

namespace Ui {
class CFrmOpenConnect;
}

class CFrmOpenConnect : public QWidget
{
    Q_OBJECT
    
public:
    explicit CFrmOpenConnect(CClient* pClient, QWidget *parent = nullptr);
    virtual ~CFrmOpenConnect();

Q_SIGNALS:
    void sigConnect(const QString &szFile, bool bOpenSettings = false);

private slots:
    void slotCancel();
    void slotEditConnect();
    void slotEdit();
    void slotCopy();
    void slotDelete();
    void on_pbAdd_clicked();
    void slotConnect();
    void slotCustomContextMenu(const QPoint &pos);
    void slotDoubleClicked(const QModelIndex& index);
private:
    int LoadFiles();

private:
    Ui::CFrmOpenConnect *ui;
    QToolBar* m_pToolBar;
    QTableView* m_pTableView;
    QStandardItemModel* m_pModel;
    CClient* m_pClient;
    int m_nFileRow;
};

#endif // FRMOPENCONNECT_H
