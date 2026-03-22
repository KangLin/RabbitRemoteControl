// Author: Kang Lin <kl222@126.com>

#include <QFileDialog>
#include <QLoggingCategory>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QTableView>
#include <QLineEdit>
#include <QLabel>
#include <QProgressBar>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QDateTime>
#include <QUuid>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QCryptographicHash>

#include "FrmExtensionStore.h"
#include "FrmExtensionManager.h"
#include "ParameterWebBrowser.h"
#include "ui_FrmExtensionStore.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.ExtensionStore")

// 默认扩展商城 API 地址
const QString DEFAULT_STORE_API = "https://api.github.com/repos/KangLin/RabbitRemoteControl/contents/ExtensionStore";

CFrmExtensionStore::CFrmExtensionStore(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CFrmExtensionStore)
    , m_pNetworkManager(nullptr)
    , m_pExtensionManager(nullptr)
    , m_pPara(nullptr)
    , m_pModelExtensions(nullptr)
    , m_apiBaseUrl(DEFAULT_STORE_API)
{
    ui->setupUi(this);
    setWindowTitle(tr("Chrome Extension Store"));
    setWindowIcon(QIcon::fromTheme("store"));
    
    InitializeUI();
    SetupNetworkManager();
    SetupConnections();
    
    qDebug(log) << Q_FUNC_INFO << "Extension store initialized";
}

CFrmExtensionStore::~CFrmExtensionStore()
{
    qDebug(log) << Q_FUNC_INFO;
    
    // 取消所有下载
    for(auto reply : m_downloads) {
        if(reply) {
            reply->abort();
            reply->deleteLater();
        }
    }
    
    delete ui;
}

void CFrmExtensionStore::InitializeUI()
{
    // 创建扩展列表模型
    m_pModelExtensions = new QStandardItemModel(this);
    m_pModelExtensions->setColumnCount(7);
    
    // 设置表头
    QStringList headers;
    headers << tr("Icon") << tr("Name") << tr("Version") 
            << tr("Rating") << tr("Downloads") << tr("ID") << tr("Status");
    m_pModelExtensions->setHorizontalHeaderLabels(headers);
    
    // 配置表视图
    ui->tvExtensions->setModel(m_pModelExtensions);
    ui->tvExtensions->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tvExtensions->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tvExtensions->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tvExtensions->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tvExtensions->setAlternatingRowColors(true);
    
    // 设置列宽
    ui->tvExtensions->horizontalHeader()->setStretchLastSection(true);
    ui->tvExtensions->verticalHeader()->setDefaultSectionSize(24);
    
    // 配置搜索框
    ui->leSearch->setPlaceholderText(tr("Search extensions..."));
    
    // 配置按钮
    ui->pbSearch->setText(tr("Search"));
    ui->pbPopular->setText(tr("Popular"));
    ui->pbRecommended->setText(tr("Recommended"));
    ui->pbDownload->setText(tr("Download"));
    ui->pbInstall->setText(tr("Install"));
    ui->pbCancel->setText(tr("Cancel"));
    ui->pbDetails->setText(tr("Details"));
    ui->pbRefresh->setText(tr("Refresh"));
    ui->pbClearCache->setText(tr("Clear Cache"));
    
    // 配置详情面板
    ui->lblExtensionName->setText(tr("Extension Name"));
    ui->lblExtensionDesc->setWordWrap(true);
    
    // 进度条
    ui->progressDownload->setVisible(false);
    
    qDebug(log) << "UI initialized";
}

void CFrmExtensionStore::SetupNetworkManager()
{
    m_pNetworkManager = new QNetworkAccessManager(this);
    qDebug(log) << "Network manager setup completed";
}

void CFrmExtensionStore::SetupConnections()
{
    bool check = true;
    
    // 按钮连接
    check = connect(ui->pbSearch, &QPushButton::clicked,
                     this, &CFrmExtensionStore::on_pbSearch_clicked);
    check = connect(ui->pbPopular, &QPushButton::clicked,
                     this, &CFrmExtensionStore::on_pbPopular_clicked);
    check = connect(ui->pbRecommended, &QPushButton::clicked,
                     this, &CFrmExtensionStore::on_pbRecommended_clicked);
    check = connect(ui->pbDownload, &QPushButton::clicked,
                     this, &CFrmExtensionStore::on_pbDownload_clicked);
    check = connect(ui->pbInstall, &QPushButton::clicked,
                     this, &CFrmExtensionStore::on_pbInstall_clicked);
    check = connect(ui->pbCancel, &QPushButton::clicked,
                     this, &CFrmExtensionStore::on_pbCancel_clicked);
    check = connect(ui->pbDetails, &QPushButton::clicked,
                     this, &CFrmExtensionStore::on_pbDetails_clicked);
    check = connect(ui->pbRefresh, &QPushButton::clicked,
                     this, &CFrmExtensionStore::on_pbRefresh_clicked);
    check = connect(ui->pbClearCache, &QPushButton::clicked,
                     this, &CFrmExtensionStore::on_pbClearCache_clicked);
    
    // 表视图连接
    check = connect(ui->tvExtensions->selectionModel(), &QItemSelectionModel::selectionChanged,
                     this, &CFrmExtensionStore::slotExtensionSelected);
    check = connect(ui->tvExtensions, &QTableView::customContextMenuRequested,
                     this, &CFrmExtensionStore::slotCustomContextMenu);
    
    // 回车搜索
    check = connect(ui->leSearch, &QLineEdit::returnPressed,
                     this, &CFrmExtensionStore::on_pbSearch_clicked);
    
    Q_ASSERT(check);
    qDebug(log) << "Connections setup completed";
}

int CFrmExtensionStore::SetExtensionManager(CFrmExtensionManager *manager)
{
    m_pExtensionManager = manager;
    if(!m_pExtensionManager) {
        qWarning(log) << "Failed to set extension manager: manager is null";
        return -1;
    }
    
    qDebug(log) << "Extension manager set successfully";
    return 0;
}

void CFrmExtensionStore::SetAPIBaseUrl(const QString &baseUrl)
{
    m_apiBaseUrl = baseUrl;
    qDebug(log) << "API base URL set to:" << m_apiBaseUrl;
}

void CFrmExtensionStore::SearchExtensions(const QString &keyword)
{
    qDebug(log) << Q_FUNC_INFO << "Keyword:" << keyword;
    
    if(!m_pNetworkManager) return;
    
    // 构建搜索 URL
    QString searchUrl = m_apiBaseUrl + "/search?q=" + keyword;

    QNetworkRequest request(QUrl(searchUrl));
/*
    request.setHeader(QNetworkRequest::UserAgentHeader, "RabbitRemoteControl");

    QNetworkReply* reply = m_pNetworkManager->get(request);

    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, &CFrmExtensionStore::slotNetworkError);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if(reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            auto extensions = ParseExtensionList(data);
            
            ClearExtensionList();
            
            for(const auto &ext : extensions) {
                AddExtensionItem(ext.toObject());
            }
            
            qDebug(log) << "Search completed, found:" << extensions.count();
        }
        reply->deleteLater();
    });
*/
}

void CFrmExtensionStore::GetPopularExtensions()
{
    qDebug(log) << Q_FUNC_INFO;
    
    if(!m_pNetworkManager) return;
    
    QString url = m_apiBaseUrl + "/popular";
    /*
    QNetworkRequest request(QUrl(url));
    request.setHeader(QNetworkRequest::UserAgentHeader, "RabbitRemoteControl");

    QNetworkReply* reply = m_pNetworkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if(reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            auto extensions = ParseExtensionList(data);
            
            ClearExtensionList();
            
            for(const auto &ext : extensions) {
                AddExtensionItem(ext.toObject());
            }
            
            qDebug(log) << "Popular extensions loaded:" << extensions.count();
        }
        reply->deleteLater();
    });
    */
}

void CFrmExtensionStore::GetRecommendedExtensions()
{
    qDebug(log) << Q_FUNC_INFO;
    
    if(!m_pNetworkManager) return;
    
    QString url = m_apiBaseUrl + "/recommended";
    /*
    QNetworkRequest request(QUrl(url));
    request.setHeader(QNetworkRequest::UserAgentHeader, "RabbitRemoteControl");

    QNetworkReply* reply = m_pNetworkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if(reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            auto extensions = ParseExtensionList(data);
            
            ClearExtensionList();
            
            for(const auto &ext : extensions) {
                AddExtensionItem(ext.toObject());
            }
            
            qDebug(log) << "Recommended extensions loaded:" << extensions.count();
        }
        reply->deleteLater();
    });
    */
}

QString CFrmExtensionStore::DownloadExtension(const QString &extensionId)
{
    qDebug(log) << Q_FUNC_INFO << "Extension ID:" << extensionId;
    
    if(!m_pNetworkManager) return QString();
    
    // 生成下载 ID
    QString downloadId = GenerateDownloadId();
    
    // 获取扩展下载 URL
    QString downloadUrl = GetExtensionFileUrl(extensionId);
    /*
    QNetworkRequest request(QUrl(downloadUrl));
    request.setHeader(QNetworkRequest::UserAgentHeader, "RabbitRemoteControl");

    QNetworkReply* reply = m_pNetworkManager->get(request);
    
    // 保存下载关系
    m_downloads[downloadId] = reply;
    m_downloadExtensionId[downloadId] = extensionId;
    
    // 连接进度信号
    connect(reply, QOverload<qint64, qint64>::of(&QNetworkReply::downloadProgress),
            this, &CFrmExtensionStore::slotDownloadProgress);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply, downloadId, extensionId]() {
        if(reply->error() == QNetworkReply::NoError) {
            // 保存文件
            QByteArray data = reply->readAll();
            QString filePath = GetDownloadPath() + "/" + extensionId + ".crx";
            
            QFile file(filePath);
            if(file.open(QIODevice::WriteOnly)) {
                file.write(data);
                file.close();
                
                qDebug(log) << "Extension downloaded:" << filePath;
                QMessageBox::information(this, tr("Success"), 
                    tr("Extension downloaded successfully!\nPath: %1").arg(filePath));
                
                ProcessDownloadedFile(filePath, extensionId);
            }
        } else {
            qWarning(log) << "Download failed:" << reply->errorString();
            QMessageBox::warning(this, tr("Error"), 
                tr("Failed to download extension: %1").arg(reply->errorString()));
        }
        
        reply->deleteLater();
        m_downloads.remove(downloadId);
        m_downloadExtensionId.remove(downloadId);
    });
    */
    return downloadId;
}

void CFrmExtensionStore::CancelDownload(const QString &downloadId)
{
    qDebug(log) << Q_FUNC_INFO << "Download ID:" << downloadId;
    
    if(m_downloads.contains(downloadId)) {
        QNetworkReply* reply = m_downloads[downloadId];
        if(reply) {
            reply->abort();
            reply->deleteLater();
        }
        m_downloads.remove(downloadId);
        m_downloadExtensionId.remove(downloadId);
    }
}

int CFrmExtensionStore::GetDownloadProgress(const QString &downloadId) const
{
    // 可以添加进度跟踪逻辑
    return 0;
}

void CFrmExtensionStore::on_pbSearch_clicked()
{
    QString keyword = ui->leSearch->text();
    if(keyword.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Please enter a search keyword"));
        return;
    }
    
    SearchExtensions(keyword);
}

void CFrmExtensionStore::on_pbPopular_clicked()
{
    GetPopularExtensions();
}

void CFrmExtensionStore::on_pbRecommended_clicked()
{
    GetRecommendedExtensions();
}

void CFrmExtensionStore::on_pbDownload_clicked()
{
    auto indexes = ui->tvExtensions->selectionModel()->selectedRows();
    if(indexes.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select an extension"));
        return;
    }
    
    int row = indexes.at(0).row();
    auto idItem = m_pModelExtensions->item(row, ColumnNo::ID);
    auto nameItem = m_pModelExtensions->item(row, ColumnNo::Name);
    
    if(!idItem || !nameItem) return;
    
    QString extensionId = idItem->text();
    
    int ret = QMessageBox::question(this, tr("Confirm"),
        tr("Do you want to download '%1'?").arg(nameItem->text()),
        QMessageBox::Yes | QMessageBox::No);
    
    if(ret == QMessageBox::Yes) {
        QString downloadId = DownloadExtension(extensionId);
        if(!downloadId.isEmpty()) {
            ui->progressDownload->setVisible(true);
            ui->progressDownload->setValue(0);
        }
    }
}

void CFrmExtensionStore::on_pbInstall_clicked()
{
    qDebug(log) << Q_FUNC_INFO;
    
    if(!m_pExtensionManager) {
        QMessageBox::warning(this, tr("Error"), 
            tr("Extension manager not set"));
        return;
    }
    
    // 打开文件选择对话框
    QString filter = tr("Chrome Extension (*.crx);;Extension Folder;;All Files (*)");
    QString path = QFileDialog::getOpenFileName(
        this, tr("Select Downloaded Extension"), GetDownloadPath(), filter);
    
    if(!path.isEmpty()) {
        m_pExtensionManager->InstallExtension(path);
    }
}

void CFrmExtensionStore::on_pbCancel_clicked()
{
    auto indexes = ui->tvExtensions->selectionModel()->selectedRows();
    if(indexes.isEmpty()) return;
    
    // 取消所有下载
    for(auto downloadId : m_downloads.keys()) {
        CancelDownload(downloadId);
    }
    
    ui->progressDownload->setVisible(false);
    QMessageBox::information(this, tr("Info"), tr("All downloads cancelled"));
}

void CFrmExtensionStore::on_pbDetails_clicked()
{
    qDebug(log) << Q_FUNC_INFO;
    
    auto indexes = ui->tvExtensions->selectionModel()->selectedRows();
    if(indexes.isEmpty()) return;
    
    int row = indexes.at(0).row();
    auto idItem = m_pModelExtensions->item(row, ColumnNo::ID);
    
    if(!idItem) return;
    
    QString extensionId = idItem->text();
    
    if(m_extensionCache.contains(extensionId)) {
        DisplayExtensionDetails(m_extensionCache[extensionId]);
    }
}

void CFrmExtensionStore::on_pbRefresh_clicked()
{
    GetPopularExtensions();
}

void CFrmExtensionStore::on_pbClearCache_clicked()
{
    qDebug(log) << Q_FUNC_INFO;
    
    QString cachePath = GetCachePath();
    QDir dir(cachePath);
    
    if(dir.exists()) {
        dir.removeRecursively();
        qDebug(log) << "Cache cleared:" << cachePath;
        QMessageBox::information(this, tr("Success"), tr("Cache cleared"));
    }
}

void CFrmExtensionStore::slotExtensionSelected()
{
    auto indexes = ui->tvExtensions->selectionModel()->selectedRows();
    if(indexes.isEmpty()) return;
    
    int row = indexes.at(0).row();
    auto idItem = m_pModelExtensions->item(row, ColumnNo::ID);
    
    if(idItem && m_extensionCache.contains(idItem->text())) {
        DisplayExtensionDetails(m_extensionCache[idItem->text()]);
    }
}

void CFrmExtensionStore::slotCustomContextMenu(const QPoint &pos)
{
    qDebug(log) << Q_FUNC_INFO;
    // TODO: 实现右键菜单
}

void CFrmExtensionStore::slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if(bytesTotal > 0) {
        int progress = (bytesReceived * 100) / bytesTotal;
        ui->progressDownload->setValue(progress);
        
        qDebug(log) << "Download progress:" << progress << "%";
    }
}

void CFrmExtensionStore::slotSearchFinished()
{
    qDebug(log) << Q_FUNC_INFO;
}

void CFrmExtensionStore::slotDownloadFinished()
{
    qDebug(log) << Q_FUNC_INFO;
}

void CFrmExtensionStore::slotNetworkError(QNetworkReply::NetworkError error)
{
    qWarning(log) << "Network error:" << error;
    QMessageBox::warning(this, tr("Network Error"), 
        tr("Failed to connect to extension store"));
}

void CFrmExtensionStore::RefreshExtensionList()
{
    GetPopularExtensions();
}

int CFrmExtensionStore::AddExtensionItem(const QJsonObject &extInfo)
{
    if(!m_pModelExtensions) return -1;
    
    QString extensionId = extInfo.value("id").toString();
    QString name = extInfo.value("name").toString();
    QString version = extInfo.value("version").toString("1.0.0");
    double rating = extInfo.value("rating").toDouble(4.5);
    int downloads = extInfo.value("downloads").toInt(0);
    
    // 保存到缓存
    m_extensionCache[extensionId] = extInfo;
    
    int row = m_pModelExtensions->rowCount();
    m_pModelExtensions->insertRow(row);
    
    // 图标
    auto itemIcon = new QStandardItem();
    itemIcon->setEditable(false);
    m_pModelExtensions->setItem(row, ColumnNo::Icon, itemIcon);
    
    // 名称
    auto itemName = new QStandardItem(name);
    itemName->setEditable(false);
    m_pModelExtensions->setItem(row, ColumnNo::Name, itemName);
    
    // 版本
    auto itemVersion = new QStandardItem(version);
    itemVersion->setEditable(false);
    m_pModelExtensions->setItem(row, ColumnNo::Version, itemVersion);
    
    // 评分
    auto itemRating = new QStandardItem(QString::number(rating, 'f', 1));
    itemRating->setEditable(false);
    m_pModelExtensions->setItem(row, ColumnNo::Rating, itemRating);
    
    // 下载数
    auto itemDownloads = new QStandardItem(QString::number(downloads));
    itemDownloads->setEditable(false);
    m_pModelExtensions->setItem(row, ColumnNo::Downloads, itemDownloads);
    
    // ID
    auto itemId = new QStandardItem(extensionId);
    itemId->setEditable(false);
    m_pModelExtensions->setItem(row, ColumnNo::ID, itemId);
    
    // 状态
    QString status = tr("Not Installed");
    if(m_pExtensionManager) {
        auto installed = m_pExtensionManager->GetInstalledExtensions();
        foreach (auto e, installed) {
            if(e.id() == extensionId)
                status = tr("Installed");
        }
    }
    
    auto itemStatus = new QStandardItem(status);
    itemStatus->setEditable(false);
    m_pModelExtensions->setItem(row, ColumnNo::Status, itemStatus);
    
    qDebug(log) << "Added extension:" << name << "ID:" << extensionId;
    return 0;
}

void CFrmExtensionStore::DisplayExtensionDetails(const QJsonObject &extInfo)
{
    QString name = extInfo.value("name").toString();
    QString description = extInfo.value("description").toString();
    QString version = extInfo.value("version").toString();
    double rating = extInfo.value("rating").toDouble(4.5);
    int downloads = extInfo.value("downloads").toInt(0);
    QString author = extInfo.value("author").toString();
    
    ui->lblExtensionName->setText(name);
    ui->lblExtensionDesc->setText(description);
    
    QString details = QString(
        "<b>%1</b><br>"
        "Version: %2<br>"
        "Rating: %3 ⭐<br>"
        "Downloads: %4<br>"
        "Author: %5"
    ).arg(name, version, QString::number(rating, 'f', 1), 
          QString::number(downloads), author);
    
    ui->lblExtensionDesc->setText(details);
}

void CFrmExtensionStore::ClearExtensionList()
{
    if(m_pModelExtensions) {
        m_pModelExtensions->removeRows(0, m_pModelExtensions->rowCount());
    }
}

int CFrmExtensionStore::ProcessDownloadedFile(const QString &filePath, 
                                              const QString &extensionId)
{
    qDebug(log) << Q_FUNC_INFO << "File:" << filePath;
    
    if(!m_pExtensionManager) return -1;
    
    // 自动安装下载的扩展
    int ret = QMessageBox::question(this, tr("Install Now?"),
        tr("Do you want to install this extension now?"),
        QMessageBox::Yes | QMessageBox::No);
    
    if(ret == QMessageBox::Yes) {
        m_pExtensionManager->InstallExtension(filePath);
    }
    
    return -1;
}

QString CFrmExtensionStore::GetDownloadPath() const
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)
                   + "/RabbitRemoteControl/Extensions";
    
    QDir dir;
    if(!dir.exists(path)) {
        dir.mkpath(path);
    }
    
    return path;
}

QString CFrmExtensionStore::GetCachePath() const
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::CacheLocation)
                   + "/ExtensionStore";
    
    QDir dir;
    if(!dir.exists(path)) {
        dir.mkpath(path);
    }
    
    return path;
}

int CFrmExtensionStore::SaveToCache(const QString &key, const QJsonObject &data)
{
    QString cachePath = GetCachePath() + "/" + key + ".json";
    
    QJsonDocument doc(data);
    QFile file(cachePath);
    
    if(file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        qDebug(log) << "Cached:" << key;
        return 0;
    }
    
    return -1;
}

QJsonObject CFrmExtensionStore::LoadFromCache(const QString &key) const
{
    QString cachePath = GetCachePath() + "/" + key + ".json";
    
    QFile file(cachePath);
    if(file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        
        QJsonDocument doc = QJsonDocument::fromJson(data);
        return doc.object();
    }
    
    return QJsonObject();
}

bool CFrmExtensionStore::IsCacheValid(const QString &key) const
{
    QString cachePath = GetCachePath() + "/" + key + ".json";
    
    QFile file(cachePath);
    if(!file.exists()) return false;
    
    QFileInfo fi(cachePath);
    QDateTime lastModified = fi.lastModified();
    QDateTime now = QDateTime::currentDateTime();
    
    int hours = lastModified.secsTo(now) / 3600;
    return hours < CACHE_VALIDITY_HOURS;
}

QJsonArray CFrmExtensionStore::ParseExtensionList(const QByteArray &data) const
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    
    if(doc.isArray()) {
        return doc.array();
    } else if(doc.isObject()) {
        QJsonObject obj = doc.object();
        if(obj.contains("extensions")) {
            return obj.value("extensions").toArray();
        }
    }
    
    return QJsonArray();
}

QJsonObject CFrmExtensionStore::ParseExtensionDetails(const QByteArray &data) const
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    return doc.object();
}

QString CFrmExtensionStore::GenerateDownloadId() const
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

bool CFrmExtensionStore::IsExtensionIdValid(const QString &id) const
{
    return !id.isEmpty() && id.length() >= 32;
}

QString CFrmExtensionStore::GetExtensionFileUrl(const QString &extensionId) const
{
    // 从 GitHub 或自定义服务器下载
    return m_apiBaseUrl + "/extensions/" + extensionId + ".crx";
}

QString CFrmExtensionStore::GetChromeWebStoreUrl(const QString &extensionId) const
{
    return "https://chromewebstore.google.com/detail/" + extensionId;
}
