#ifndef CFAVORITEVIEW_H
#define CFAVORITEVIEW_H

#include <QTreeView>
#include <QStandardItem>
#include "TitleBar.h"

class CFavoriteView : public QTreeView
{
    Q_OBJECT
public:
    explicit CFavoriteView(QWidget *parent = nullptr);
    virtual ~CFavoriteView();
    
    RabbitCommon::CTitleBar* m_pDockTitleBar;
    int AddFavorite(const QString& szName, const QString &szFile);

Q_SIGNALS:
    void sigConnect(const QString &szFile, bool bOpenSettings);
    void sigFavorite();
    
    // QWidget interface
protected:
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dragMoveEvent(QDragMoveEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    
private slots:
    void slotFavrtieClicked(const QModelIndex &index);
    void slotFavortiedoubleClicked(const QModelIndex &index);
    void slotCustomContextMenu(const QPoint &pos);
    void slotConnect();
    void slotOpenConnect();
    void slotDelete();
    void slotNewGroup();
    void slotDoubleEditNode(bool bEdit);
    void slotMenu();

private:
    QStandardItemModel* m_pModel;
    QString m_szSaveFile;

    QPoint m_DragStartPosition;

    QMenu* m_pMenu;

    QStandardItem* NewItem(const QModelIndex &index);
    int Save();
};

#endif // CFAVORITEVIEW_H
