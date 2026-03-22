// Author: Kang Lin <kl222@126.com>
// QWebEngine Plugin Manager Form

#pragma once

#include <QWidget>
#include <QStandardItemModel>
#include <QWebEngineProfile>
#include <QVariant>

namespace Ui {
class CFrmQWebEnginePluginManagerUI;
}

class CParameterWebBrowser;

/**
 * @brief CFrmQWebEnginePluginManager - QWebEngine 插件管理窗体
 * 
 * 功能：
 * - 显示已安装的 QWebEngine 插件列表
 * - ��用/禁用插件
 * - 查看插件详细信息
 * - 导入导出插件配置
 */
class CFrmQWebEnginePluginManager : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmQWebEnginePluginManager(QWidget *parent = nullptr);
    virtual ~CFrmQWebEnginePluginManager();

    /**
     * @brief 设置 WebEngine 参数
     * @param pPara - WebBrowser 参数对象
     * @return 成功返回 0，失败返回 -1
     */
    int SetParameter(CParameterWebBrowser *pPara);

    /**
     * @brief 获取插件管理器配置
     * @return 返回配置字典
     */
    QMap<QString, QVariant> GetPluginConfiguration() const;

    /**
     * @brief 设置插件管理器配置
     * @param config - 配置字典
     * @return 成功返回 0，失败返回 -1
     */
    int SetPluginConfiguration(const QMap<QString, QVariant> &config);

private slots:
    // UI 操作槽函数
    void on_pbEnableAll_clicked();
    void on_pbDisableAll_clicked();
    void on_pbAdd_clicked();
    void on_pbRemove_clicked();
    void on_pbDetails_clicked();
    void on_pbExport_clicked();
    void on_pbImport_clicked();
    void on_pbApply_clicked();
    void on_pbReset_clicked();

    // 模型槽函数
    void slotPluginItemChanged(QStandardItem *item);
    void slotSelectionChanged();
    void slotCustomContextMenu(const QPoint &pos);

private:
    // 初始化函数
    void InitializeUI();
    void LoadPluginList();
    void SetupConnections();
    
    // 插件操作函数
    int RefreshPluginList();
    int AddPluginItem(const QString &pluginName, const QString &pluginPath, bool enabled);
    int RemovePluginItem(int row);
    int UpdatePluginItem(int row, bool enabled);
    
    // 配置管理
    int SavePluginConfiguration();
    int LoadPluginConfiguration();
    
    // 工具函数
    QString GetPluginInfo(const QString &pluginPath) const;
    bool IsValidPlugin(const QString &pluginPath) const;

private:
    Ui::CFrmQWebEnginePluginManagerUI* ui;
    CParameterWebBrowser* m_pPara;
    QStandardItemModel* m_pModelPlugins;
    
    // 模型列号枚举
    enum ColumnNo {
        Enabled = 0,      // 启用状态复选框
        Name = 1,         // 插件名称
        Type = 2,         // 插件类型
        Version = 3,      // 版本
        Path = 4,         // 文件路径
        Description = 5   // 描述
    };
    
    // 插件配置缓存
    QMap<QString, QVariant> m_pluginConfig;
};