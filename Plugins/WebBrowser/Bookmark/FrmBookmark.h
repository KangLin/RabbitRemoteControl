// Author: Kang Lin <kl222@126.com>

#pragma once
#include <QWidget>
#include <QStandardItem>
#include <QToolBar>
#include <QTreeView>
#include "BookmarkDatabase.h"
#include "ParameterWebBrowser.h"

namespace Ui {
class CFrmBookmark;
}

class CFrmBookmark : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmBookmark(CBookmarkDatabase *db, CParameterWebBrowser* pPara, QWidget *parent = nullptr);
    ~CFrmBookmark();

    void refresh();

signals:
    void openUrlRequested(const QString &url);

private slots:
    void onAddBookmark();
    void onAddFolder();
    void onEditBookmark();
    void onDeleteBookmark();
    void onSetFavorite();
    void onImportBookmarks();
    void onExportBookmarks();
    void onSearchTextChanged(const QString &text);
    void onTreeViewDoubleClicked(const QModelIndex &index);
    void onCustomContextMenu(const QPoint &pos);

private:
    void setupUI();
    void setupToolBar();
    void setupTreeView();
    void loadBookmarks();

    enum Role {
        ID = Qt::UserRole,
        Type,
        Url
    };

private:
    Ui::CFrmBookmark *ui;
    CBookmarkDatabase *m_pDatabase;
    CParameterWebBrowser* m_pPara;
    QTreeView *m_pTreeView;
    QLineEdit *m_pSearchEdit;
    QToolBar *m_pToolBar;
    QStandardItemModel *m_pModel;
    QMap<int, QStandardItem*> m_folderItems;
};

