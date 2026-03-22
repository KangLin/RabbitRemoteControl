// Author: Kang Lin <kl222@126.com>
// Chrome Extension Manager for QWebEngine

#pragma once

#include <QWidget>
#include <QStandardItemModel>
#include <QWebEngineProfile>
#include <QWebEngineExtensionInfo>
#include <QWebEngineExtensionManager>

namespace Ui {
class CFrmExtensionManager;
}

/*!
 * \brief 扩展管理器界面
 * 
 * - Qt 版本要求：必须使用 Qt 6.10 或更高版本
 * - 扩展格式：只支持 Manifest V3 格式的扩展
 * - 配置文件：不能使用 off-the-record 的配置文件
 *
 * 查找 Chrome 已安装的扩展：
 * - 在 Chrmoe 地址栏中输入： `chrome://version/`
 * - 在其中找到“个人资料路径”
 * - Chrome 扩展位于“个人资料路径”下的“Extensions”目录下
 */
class CFrmExtensionManager : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmExtensionManager(
        QWebEngineProfile *profile, QWidget *parent = nullptr);
    virtual ~CFrmExtensionManager();

    /*!
     * \brief 设置 WebEngine Profile
     * \param profile - QWebEngineProfile 对象
     * \return 成功返回 0，失败返回 -1
     */
    int SetProfile(QWebEngineProfile *profile);

    /*!
     * \brief 安装或加载 Chrome 扩展
     * \param path - 扩展文件路径 (.crx 文件或解包目录)
     * \note
     * - Qt 版本要求：必须使用 Qt 6.10 或更高版本
     * - 扩展格式：只支持 Manifest V3 格式的扩展
     * - 配置文件：不能使用 off-the-record 的配置文件
     */
    void InstallExtension(const QString &path);

    /*!
     * \brief 卸载扩展
     * \param extensionId - 扩展 ID
     */
    void UninstallExtension(const QString &id);
    
    /*!
     * \brief 启用扩展
     * \param extensionId - 扩展 ID
     */
    void EnableExtension(const QString &id, bool bEnable);

    /*!
     * \brief 获取已安装的扩展列表
     * \return 扩展信息列表
     */
    QList<QWebEngineExtensionInfo> GetInstalledExtensions() const;

    /*!
     * \brief 获取扩展详细信息
     * \param extensionId - 扩展 ID
     */
    QWebEngineExtensionInfo GetExtensionInfo(const QString &id) const;

private slots:
    void on_pbInstall_clicked();
    void on_pbLoad_clicked();
    void on_pbUninstall_clicked();
    void on_pbRefresh_clicked();
    void on_pbDetails_clicked();
    void on_pbOpenDevTools_clicked();
    void on_pbOpenExtensionPath_clicked();

    void slotExtensionItemChanged(QStandardItem *item);
    void slotSelectionChanged();
    void slotCustomContextMenu(const QPoint &pos);

    void slotLoadFinished(const QWebEngineExtensionInfo &extension);
    void slotInstallFinished(const QWebEngineExtensionInfo &extension);
    void slotUnloadFinished(const QWebEngineExtensionInfo &extension);
    void slotUninstallFinished(const QWebEngineExtensionInfo &extension);
    
    void slotCopyPopupUrl();
    void slotCopyPath();
private:
    // 初始化函数
    void InitializeUI();
    void SetupConnections();

    // 扩展操作函数
    int RefreshExtensionList();
    int AddExtensionItem(const QWebEngineExtensionInfo &info);
    int RemoveExtensionItem(const QString &extensionId);

private:
    Ui::CFrmExtensionManager* ui;
    QWebEngineProfile* m_pProfile;
    QStandardItemModel* m_pModelExtensions;

    QAction* m_pInstall;
    QAction* m_pLoad;
    QAction* m_pUninstall;
    QAction* m_pRefresh;
    QAction* m_pCopyUrl;
    QAction* m_pCopyPath;
    QAction* m_pDetails;
    QAction* m_pOpenFolder;

    // 表列号枚举
    enum ColumnNo {
        Name = 0,
        Enabled,
        Description,
        PopupUrl,
        ID,
        Path
    };
};
