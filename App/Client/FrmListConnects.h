#ifndef FRMOPENCONNECT_H
#define FRMOPENCONNECT_H

#include <QWidget>
#include <QStandardItemModel>
#include <QToolBar>
#include <QTableView>
#include <QToolButton>
#include "Client.h"

/*!
 * \~chinese 列出存在的连接配置
 *
 * \~english List the connect of be existed
 */
class CFrmListConnects : public QWidget, CClient::Handle
{
    Q_OBJECT
    
public:
    explicit CFrmListConnects(CClient* pClient,
                              bool bClose = true,
                              QWidget *parent = nullptr);
    virtual ~CFrmListConnects();

public:
    virtual int onProcess(const QString &id, CPluginClient *pPlug) override;

Q_SIGNALS:
    void sigConnect(const QString &szFile, bool bOpenSettings = false);

private slots:
    void slotEditConnect();
    void slotNew();
    void slotEdit();
    void slotCopy();
    void slotDelete();
    void slotConnect();
    void slotCustomContextMenu(const QPoint &pos);
    void slotDoubleClicked(const QModelIndex& index);
    void slotLoadFiles();

private:
    int InsertItem(CConnecter* c, QString &szFile);

private:
    QToolBar* m_pToolBar;
    QToolButton* m_ptbConnect;
    QMenu* m_pMenuNew;

    QTableView* m_pTableView;
    QStandardItemModel* m_pModel;
    CClient* m_pClient;
    int m_nFileRow;
    
    bool m_bClose;
};

#endif // FRMOPENCONNECT_H
