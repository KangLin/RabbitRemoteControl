#pragma once

#include <QWidget>
#include <QTableView>
#include <QToolBar>
#include <QToolButton>
#include <QStandardItemModel>
#include <QVector>
#include "Connecter.h"
#include "TitleBar.h"
#include "RabbitRecentMenu.h"
#include "ParameterApp.h"

class CFrmActive : public QWidget
{
    Q_OBJECT
public:
    explicit CFrmActive(QVector<CConnecter*> &Connecters,
                            CParameterApp &parameterApp,
                            QMenu* pOperate,
                            QAction* pStop,
                            RabbitCommon::CRecentMenu* pRecentMenu,
                            QWidget *parent = nullptr);
    virtual ~CFrmActive();
    RabbitCommon::CTitleBar* m_pDockTitleBar;
public Q_SLOTS:
    void slotLoadConnecters();
    void slotViewChanged(const QWidget* pView);
Q_SIGNALS:
    void sigConnecterChanged(CConnecter* c);
private Q_SLOTS:
    void slotCustomContextMenu(const QPoint &pos);
    void slotClicked(const QModelIndex& index);
private:
    QVector<CConnecter*> &m_Connecters;
    CParameterApp &m_ParameterApp;
    QMenu* m_pOperate;
    QAction* m_pStop;
    RabbitCommon::CRecentMenu* m_pRecentMenu;
    QTableView* m_pTableView;
    QStandardItemModel* m_pModel;
    QMenu* m_pMenu;
    QToolBar* m_pToolBar;
    int m_nId;
};
