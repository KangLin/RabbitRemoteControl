#ifndef FRMOPENCONNECT_H
#define FRMOPENCONNECT_H

#include <QWidget>
#include <QStandardItemModel>
#include <QToolBar>
#include <QTableView>
#include <QToolButton>
#include "Client.h"

namespace Ui {
class CFrmOpenConnect;
}

class CFrmOpenConnect : public QWidget, CClient::Handle
{
    Q_OBJECT
    
public:
    explicit CFrmOpenConnect(CClient* pClient, QWidget *parent = nullptr);
    virtual ~CFrmOpenConnect();

public:
    virtual int onProcess(const QString &id, CPluginClient *pPlug) override;

Q_SIGNALS:
    void sigConnect(const QString &szFile, bool bOpenSettings = false);

private slots:
    void slotCancel();
    void slotEditConnect();
    void slotNew();
    void slotEdit();
    void slotCopy();
    void slotDelete();
    void slotConnect();
    void slotCustomContextMenu(const QPoint &pos);
    void slotDoubleClicked(const QModelIndex& index);

private:
    int LoadFiles();

private:
    Ui::CFrmOpenConnect *ui;
    QToolBar* m_pToolBar;
    QToolButton* m_ptbConnect;
    QMenu* m_pMenuNew;

    QTableView* m_pTableView;
    QStandardItemModel* m_pModel;
    CClient* m_pClient;
    int m_nFileRow;
};

#endif // FRMOPENCONNECT_H
