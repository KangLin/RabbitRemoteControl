#ifndef FRMCONNECTERS_H_KL_2025_04_03
#define FRMCONNECTERS_H_KL_2025_04_03

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

class CFrmConnecters : public QWidget
{
    Q_OBJECT
public:
    explicit CFrmConnecters(QVector<CConnecter*> &Connecters,
                            CParameterApp &parameterApp,
                            QMenu* pConnect,
                            QAction* pDisconnect,
                            RabbitCommon::CRecentMenu* pRecentMenu,
                            QWidget *parent = nullptr);
    virtual ~CFrmConnecters();
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
    QMenu* m_pConnect;
    QAction* m_pDisconnect;
    RabbitCommon::CRecentMenu* m_pRecentMenu;
    QTableView* m_pTableView;
    QStandardItemModel* m_pModel;
    QMenu* m_pMenu;
    QToolBar* m_pToolBar;
    int m_nId;
};

#endif // FRMCONNECTERS_H_KL_2025_04_03
