#pragma once

#include <QWidget>
#include <QTableView>
#include <QToolBar>
#include <QToolButton>
#include <QStandardItemModel>
#include <QVector>
#include "Operate.h"
#include "TitleBar.h"
#include "RabbitRecentMenu.h"
#include "ParameterApp.h"

class CFrmActive : public QWidget
{
    Q_OBJECT
public:
    explicit CFrmActive(QVector<COperate*> &operates,
                            CParameterApp &parameterApp,
                            QMenu* pOperate,
                            QAction* pStop,
                            RabbitCommon::CRecentMenu* pRecentMenu,
                            QWidget *parent = nullptr);
    virtual ~CFrmActive();
    RabbitCommon::CTitleBar* m_pDockTitleBar;
public Q_SLOTS:
    void slotLoad();
    void slotViewChanged(const QWidget* pView);
Q_SIGNALS:
    void sigChanged(COperate* c);
    void sigAddToFavorite(const QString& szName, const QString& szDescription, const QIcon& icon, const QString &szFile);

private Q_SLOTS:
    void slotCustomContextMenu(const QPoint &pos);
    void slotClicked(const QModelIndex& index);
    void slotAddToFavorite();
private:
    QVector<COperate*> &m_Operates;
    CParameterApp &m_ParameterApp;
    QMenu* m_pOperate;
    QAction* m_pStop;
    RabbitCommon::CRecentMenu* m_pRecentMenu;
    QAction* m_pAddToFavorite;
    QTableView* m_pTableView;
    QStandardItemModel* m_pModel;
    QMenu* m_pMenu;
    QToolBar* m_pToolBar;
    int m_nId;
};
