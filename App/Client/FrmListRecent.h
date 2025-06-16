#pragma once

#include <QWidget>
#include <QStandardItemModel>
#include <QToolBar>
#include <QTableView>
#include <QToolButton>
#include "Manager.h"
#include "TitleBar.h"
#include "ParameterApp.h"

class MainWindow;

/*!
 * \~chinese 列出存在的连接配置
 *
 * \~english List the connect of be existed
 */
class CFrmListRecent : public QWidget, CManager::Handle
{
    Q_OBJECT
    
public:
    explicit CFrmListRecent(MainWindow* pMainWindow, CManager* pManager,
                            CParameterApp &parameterApp, bool bDock = false,
                            QWidget *parent = nullptr);
    virtual ~CFrmListRecent();

public:
    virtual int onProcess(const QString &id, CPlugin *pPlugin) override;
    
    RabbitCommon::CTitleBar* m_pDockTitleBar;

Q_SIGNALS:
    void sigStart(const QString &szFile, bool bOpenSettings = false);

private slots:
    void slotEditConnect();
    void slotNew();
    void slotEdit();
    void slotCopy();
    void slotDelete();
    void slotDetail();
    void slotStart();
    void slotCustomContextMenu(const QPoint &pos);
    void slotDoubleClicked(const QModelIndex& index);
    void slotLoadFiles();
    void slotStartByType();

private:
    int InsertItem(COperate* c, QString &szFile);
    QList<QStandardItem*> GetItem(COperate* c, QString &szFile);

private:
    MainWindow* m_pMainWindow;
    QMap<CPlugin::TYPE, QMenu*> m_MenuStartByType;
    CParameterApp &m_ParameterApp;
    QToolBar* m_pToolBar;
    QToolButton* m_ptbOperate;
    QMenu* m_pMenuNew;
    QAction* m_pStart;
    QAction* m_pEditOperate;
    QAction* m_pEdit;
    QAction* m_pCopy;
    QAction* m_pDelete;
    QAction* m_pDetail;
    QAction* m_pRefresh;

    QTableView* m_pTableView;
    QStandardItemModel* m_pModel;
    CManager* m_pManager;
    int m_nFileRow;
    int m_nId;
    bool m_bDock;
};
