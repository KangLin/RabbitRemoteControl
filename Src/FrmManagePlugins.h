// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QListView>
#include <QTableView>
#include <QDir>
#include <QStandardItemModel>
#include "ParameterUI.h"
#include "ParameterPlugin.h"

class CPlugin;
namespace Ui {
class CFrmManagePluginsUI;
}
class CFrmManagePlugins : public CParameterUI
{
    Q_OBJECT
public:
    CFrmManagePlugins(QWidget *parent = nullptr);

    // CParameterUI interface
public:
    virtual int SetParameter(CParameter *pParameter) override;
    virtual int Accept() override;
private slots:
    void on_pbAdd_clicked();
    void on_pbRemove_clicked();
    void slotCustomContextPluginsPath(const QPoint& pos);
    void on_gbPluginsPath_clicked(bool checked);
    void slotFilterItemChanged(QStandardItem *item);

private:
    int SetFilterHeader();
    int AddPath(const QString &szPath);
    int FindPlugins(QDir dir, QStringList filters, bool bAdd = true);
    int AddItem(CPlugin* plugin, const QString& szPath);
    int RemoveItem(const QString& szPath);
private:
    Ui::CFrmManagePluginsUI* ui;
    enum ColumnNo {
        Whitelist,
        Blacklist,
        Name,
        Type,
        Path
    };
    CParameterPlugin* m_pPara;
    QStandardItemModel* m_pModelPluginPath;
    QStandardItemModel* m_pModelFilter;
};

