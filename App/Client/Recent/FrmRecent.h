// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QWidget>
#include <QStandardItemModel>
#include <QToolBar>
#include <QTableView>
#include <QToolButton>
#include "Manager.h"
#include "TitleBar.h"
#include "ParameterApp.h"
#include "RecentModel.h"

class MainWindow;

/*!
 * \~chinese 列出存在的连接配置
 *
 * \~english List the connect of be existed
 */
class CFrmRecent : public QWidget, CManager::Handle
{
    Q_OBJECT
    
public:
    explicit CFrmRecent(MainWindow* pMainWindow, CManager* pManager,
                        CRecentDatabase* pDb, CParameterApp &parameterApp,
                        bool bDock = false, QWidget *parent = nullptr);
    virtual ~CFrmRecent();

public:
    virtual int onProcess(const QString &id, CPlugin *pPlugin) override;

    RabbitCommon::CTitleBar* m_pDockTitleBar;

    int Init();

public Q_SLOTS:
    void slotRefresh();

Q_SIGNALS:
    void sigStart(const QString &szFile, bool bOpenSettings = false);
    void sigAddToFavorite(const QString &szFile,
                          const QString& szName,
                          const QString& szDescription,
                          const QIcon& icon);

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
    void slotStartByType();
    void slotAddToFavorite();
    void slotExport();
    void slotImport();

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
    QAction* m_pAddToFavorite;
    QAction* m_pExport;
    QAction* m_pImport;

    QTableView* m_pTableView;
    CRecentDatabase* m_pDatabase;
    CRecentModel* m_pModel;
    CManager* m_pManager;
    bool m_bDock;
};
