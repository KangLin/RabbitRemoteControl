// Author: Kang Lin <kl222@126.com>
// Chrome Extension Store - Download Extensions from Web Store

#pragma once

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStandardItemModel>
#include <QMap>
#include <QJsonObject>

namespace Ui {
class CFrmExtensionStore;
}

class CParameterWebBrowser;
class CFrmExtensionManager;

/**
 * @brief Chrome 扩展商城
 * 
 * 功能：
 * - 从 Chrome Web Store 搜索扩展
 * - 下载和安装扩展
 * - 管理下载队列
 * - 显示扩展详情和评分
 * - 支持离线扩展列表缓存
 */
class CFrmExtensionStore : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmExtensionStore(QWidget *parent = nullptr);
    virtual ~CFrmExtensionStore();

    /**
     * @brief 设置扩展管理器
     * @param manager - 扩展管理器指针
     * @return 成功返回 0，失败返回 -1
     */
    int SetExtensionManager(CFrmExtensionManager *manager);

    /**
     * @brief 设置基础 URL (可以指向自建服务器)
     * @param baseUrl - 服务器地址，如 "https://api.example.com"
     */
    void SetAPIBaseUrl(const QString &baseUrl);

    /**
     * @brief 搜索扩展
     * @param keyword - 搜索关键词
     */
    void SearchExtensions(const QString &keyword);

    /**
     * @brief 获取热门扩展列表
     */
    void GetPopularExtensions();

    /**
     * @brief 获取推荐扩展列表
     */
    void GetRecommendedExtensions();

    /**
     * @brief 下载扩展
     * @param extensionId - 扩展 ID
     * @return 成功返回下载 ID，失败返回空字符串
     */
    QString DownloadExtension(const QString &extensionId);

    /**
     * @brief 取消下载
     * @param downloadId - 下载 ID
     */
    void CancelDownload(const QString &downloadId);

    /**
     * @brief 获取下载进度
     * @param downloadId - 下载 ID
     * @return 返回进度百分比 (0-100)
     */
    int GetDownloadProgress(const QString &downloadId) const;

private slots:
    // UI 槽函数
    void on_pbSearch_clicked();
    void on_pbPopular_clicked();
    void on_pbRecommended_clicked();
    void on_pbDownload_clicked();
    void on_pbInstall_clicked();
    void on_pbCancel_clicked();
    void on_pbDetails_clicked();
    void on_pbRefresh_clicked();
    void on_pbClearCache_clicked();

    // 表视图槽函数
    void slotExtensionSelected();
    void slotCustomContextMenu(const QPoint &pos);
    void slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    // 网络槽函数
    void slotSearchFinished();
    void slotDownloadFinished();
    void slotNetworkError(QNetworkReply::NetworkError error);

private:
    // 初始化函数
    void InitializeUI();
    void SetupConnections();
    void SetupNetworkManager();

    // 扩展列表处理
    void RefreshExtensionList();
    int AddExtensionItem(const QJsonObject &extInfo);
    void DisplayExtensionDetails(const QJsonObject &extInfo);
    void ClearExtensionList();

    // 下载管理
    int ProcessDownloadedFile(const QString &filePath, const QString &extensionId);
    QString GetDownloadPath() const;
    QString GetCachePath() const;

    // 缓存管理
    int SaveToCache(const QString &key, const QJsonObject &data);
    QJsonObject LoadFromCache(const QString &key) const;
    bool IsCacheValid(const QString &key) const;

    // 数据解析
    QJsonArray ParseExtensionList(const QByteArray &data) const;
    QJsonObject ParseExtensionDetails(const QByteArray &data) const;

    // 工具函数
    QString GenerateDownloadId() const;
    bool IsExtensionIdValid(const QString &id) const;
    QString GetExtensionFileUrl(const QString &extensionId) const;
    QString GetChromeWebStoreUrl(const QString &extensionId) const;

private:
    Ui::CFrmExtensionStore* ui;
    QNetworkAccessManager* m_pNetworkManager;
    CFrmExtensionManager* m_pExtensionManager;
    CParameterWebBrowser* m_pPara;
    QStandardItemModel* m_pModelExtensions;
    
    // API 基础 URL
    QString m_apiBaseUrl;
    
    // 下载管理
    QMap<QString, QNetworkReply*> m_downloads;        // downloadId -> reply
    QMap<QString, QString> m_downloadExtensionId;     // downloadId -> extensionId
    QMap<QString, QJsonObject> m_extensionCache;      // extensionId -> extensionInfo
    
    // 表列号
    enum ColumnNo {
        Icon = 0,           // 图标
        Name = 1,           // 名称
        Version = 2,        // 版本
        Rating = 3,         // 评分
        Downloads = 4,      // 下载数
        ID = 5,             // 扩展 ID
        Status = 6          // 状态(未安装/已安装/下载中)
    };

    // 缓存设置
    static const int CACHE_VALIDITY_HOURS = 24;
};
