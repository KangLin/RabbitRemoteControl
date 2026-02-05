// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QTreeView>
#include <QStandardItem>

#include "TitleBar.h"
#include "FavoriteDatabase.h"
#include "FavoriteModel.h"
#include "ParameterApp.h"

class CFavoriteView : public QWidget
{
    Q_OBJECT

public:
    explicit CFavoriteView(CParameterApp* pPara, QWidget *parent = nullptr);
    virtual ~CFavoriteView();

    void Initial();
    RabbitCommon::CTitleBar* m_pDockTitleBar;
public Q_SLOTS:
    void slotAddToFavorite(const QString &szFile,
                           const QString& szName,
                           const QString& szDescription,
                           const QIcon& icon);
    void slotUpdateFavorite(const QString &szFile,
                            const QString& szName = QString(),
                            const QString& szDescription = QString(),
                            const QIcon& icon = QIcon());
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

Q_SIGNALS:
    void sigStart(const QString &szFile, bool bOpenSettings);
    void sigFavorite();

    // QWidget interface
protected:
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dragMoveEvent(QDragMoveEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;
/*
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
  */
private slots:
    void slotFavrtieClicked(const QModelIndex &index);
    void slotFavortiedoubleClicked(const QModelIndex &index);
    void slotCustomContextMenu(const QPoint &pos);
    void slotStart();
    void slotOpenStart();
    void slotEdit();
    void slotDelete();
    void slotNewGroup();
    void slotDoubleEditNode(bool bEdit);
    void slotMenu();
    void slotImport();
    void slotExport();
    void slotRefresh();

private:
    void setupUI();
    void setupToolBar(QLayout* layout);
    void setupTreeView(QLayout* layout);
    void EnableAction(const QModelIndex& index = QModelIndex());

private:
    CParameterApp* m_pParaApp;
    QTreeView* m_pTreeView;
    CFavoriteModel* m_pModel;
    CFavoriteDatabase* m_pDatabase;
    QAction *m_pStartAction;
    QAction *m_pEidtStartAction;
    QAction *m_pAddFolderAction;
    QAction *m_pEditAction;
    QAction *m_pDeleteAction;
    QAction *m_pImportAction;
    QAction *m_pExportAction;
    QAction* m_pRefresh;
    QAction* m_pShowToolbar;

    QMenu* m_pMenu;
    QPoint m_DragStartPosition;
};
