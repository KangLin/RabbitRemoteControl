// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QUrl>
#include <QWidget>
#include "QStandardItemModel"
#include "BookmarkDatabase.h"
#include "ParameterWebBrowser.h"

namespace Ui {
class CFrmAddBookmark;
}

class CFrmAddBookmark : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmAddBookmark(
        const QString &szTitle, const QUrl &url,
        const QIcon &icon, CParameterWebBrowser* pPara,
        CBookmarkDatabase *db, QWidget *parent = nullptr);
    ~CFrmAddBookmark();

private slots:
    void on_pbAdd_clicked();
    void on_pbCancel_clicked();
    void on_pbDelete_clicked();
    void on_pbNewFolder_clicked();
    void onCustomContextMenu(const QPoint &pos);

private:
    enum class Role {
        ID = Qt::UserRole,
        Type,
        Url
    };
    void loadFolder(int nCurrent = 0);

private:
    Ui::CFrmAddBookmark *ui;
    QString m_szTitle;
    QUrl m_Url;
    QIcon m_Icon;
    CParameterWebBrowser* m_pPara;
    CBookmarkDatabase *m_pDatabase;
    QStandardItemModel *m_pModelTree;
    QMap<int, QStandardItem*> m_folderItems;
};
