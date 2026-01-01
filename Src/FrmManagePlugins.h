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
private:
    int FindPlugins(QDir dir, QStringList filters);
    int AddItem(CPlugin* plugin, const QString& szPath);
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

