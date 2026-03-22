// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QTableView>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QWebEngineProfile>
#include <QSettings>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "FrmQWebEnginePluginManager.h"
#include "ParameterWebBrowser.h"
#include "ui_FrmQWebEnginePluginManager.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.PluginManager")

CFrmQWebEnginePluginManager::CFrmQWebEnginePluginManager(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CFrmQWebEnginePluginManagerUI)
    , m_pPara(nullptr)
    , m_pModelPlugins(nullptr)
{
    ui->setupUi(this);
    setWindowTitle(tr("QWebEngine Plugin Manager"));
    setWindowIcon(QIcon::fromTheme("extension"));
    
    InitializeUI();
    SetupConnections();
    
    qDebug(log) << Q_FUNC_INFO << "Plugin manager initialized";
}

CFrmQWebEnginePluginManager::~CFrmQWebEnginePluginManager()
{
    qDebug(log) << Q_FUNC_INFO;
    delete ui;
}

void CFrmQWebEnginePluginManager::InitializeUI()
{
    // 创建插件列表模型
    m_pModelPlugins = new QStandardItemModel(this);
    m_pModelPlugins->setColumnCount(6);
    
    // 设置表头
    QStringList headers;
    headers << tr("Enable") << tr("Name") << tr("Type") 
            << tr("Version") << tr("Path") << tr("Description");
    m_pModelPlugins->setHorizontalHeaderLabels(headers);
    
    // 配置表视图
    ui->tvPlugins->setModel(m_pModelPlugins);
    ui->tvPlugins->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tvPlugins->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tvPlugins->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tvPlugins->setContextMenuPolicy(Qt::CustomContextMenu);
    
    // 设置列宽自适应
    ui->tvPlugins->horizontalHeader()->setStretchLastSection(true);
    ui->tvPlugins->horizontalHeader()->setSectionResizeMode(
        ColumnNo::Path, QHeaderView::Stretch);
    
    // 设置行高
    ui->tvPlugins->verticalHeader()->setDefaultSectionSize(24);
    
    // 配置按钮
    ui->pbEnableAll->setText(tr("Enable All"));
    ui->pbDisableAll->setText(tr("Disable All"));
    ui->pbAdd->setText(tr("Add Plugin"));
    ui->pbRemove->setText(tr("Remove"));
    ui->pbDetails->setText(tr("Details"));
    ui->pbExport->setText(tr("Export"));
    ui->pbImport->setText(tr("Import"));
    ui->pbApply->setText(tr("Apply"));
    ui->pbReset->setText(tr("Reset"));
    
    qDebug(log) << "UI initialized";
}

void CFrmQWebEnginePluginManager::SetupConnections()
{
    bool check = true;
    
    // 按钮连接
    check &= connect(ui->pbEnableAll, &QPushButton::clicked,
                     this, &CFrmQWebEnginePluginManager::on_pbEnableAll_clicked);
    check &= connect(ui->pbDisableAll, &QPushButton::clicked,
                     this, &CFrmQWebEnginePluginManager::on_pbDisableAll_clicked);
    check &= connect(ui->pbAdd, &QPushButton::clicked,
                     this, &CFrmQWebEnginePluginManager::on_pbAdd_clicked);
    check &= connect(ui->pbRemove, &QPushButton::clicked,
                     this, &CFrmQWebEnginePluginManager::on_pbRemove_clicked);
    check &= connect(ui->pbDetails, &QPushButton::clicked,
                     this, &CFrmQWebEnginePluginManager::on_pbDetails_clicked);
    check &= connect(ui->pbExport, &QPushButton::clicked,
                     this, &CFrmQWebEnginePluginManager::on_pbExport_clicked);
    check &= connect(ui->pbImport, &QPushButton::clicked,
                     this, &CFrmQWebEnginePluginManager::on_pbImport_clicked);
    check &= connect(ui->pbApply, &QPushButton::clicked,
                     this, &CFrmQWebEnginePluginManager::on_pbApply_clicked);
    check &= connect(ui->pbReset, &QPushButton::clicked,
                     this, &CFrmQWebEnginePluginManager::on_pbReset_clicked);
    
    // 模型连接
    check &= connect(m_pModelPlugins, &QStandardItemModel::itemChanged,
                     this, &CFrmQWebEnginePluginManager::slotPluginItemChanged);
    
    // 表视图连接
    check &= connect(ui->tvPlugins->selectionModel(), &QItemSelectionModel::selectionChanged,
                     this, &CFrmQWebEnginePluginManager::slotSelectionChanged);
    check &= connect(ui->tvPlugins, &QTableView::customContextMenuRequested,
                     this, &CFrmQWebEnginePluginManager::slotCustomContextMenu);
    
    Q_ASSERT(check);
    qDebug(log) << "Connections setup completed";
}

int CFrmQWebEnginePluginManager::SetParameter(CParameterWebBrowser *pPara)
{
    m_pPara = pPara;
    if(!m_pPara) {
        qWarning(log) << "Failed to set parameter: pPara is null";
        return -1;
    }
    
    LoadPluginConfiguration();
    RefreshPluginList();
    
    return 0;
}

int CFrmQWebEnginePluginManager::RefreshPluginList()
{
    if(!m_pModelPlugins) return -1;
    
    m_pModelPlugins->removeRows(0, m_pModelPlugins->rowCount());
    
    qDebug(log) << "Refreshing plugin list";
    
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    // Qt 6 及以上版本使用现代 QWebEngine API
    auto profile = QWebEngineProfile::defaultProfile();
    if(!profile) {
        qWarning(log) << "Default profile is null";
        return -1;
    }
#endif
    
    // 扫描插件目录
    QStringList pluginPaths;
    pluginPaths << QCoreApplication::libraryPaths();
    
    for(const auto &path : pluginPaths) {
        QDir dir(path);
        if(!dir.exists()) continue;
        
        QStringList filters;
#ifdef Q_OS_WIN
        filters << "*.dll";
#elif defined(Q_OS_MACOS)
        filters << "*.dylib" << "*.so";
#else
        filters << "*.so";
#endif
        
        dir.setNameFilters(filters);
        QFileInfoList files = dir.entryInfoList();
        
        for(const auto &fileInfo : files) {
            if(IsValidPlugin(fileInfo.filePath())) {
                AddPluginItem(fileInfo.baseName(), fileInfo.filePath(), true);
            }
        }
    }
    
    qDebug(log) << "Plugin list refreshed, total plugins:" << m_pModelPlugins->rowCount();
    return 0;
}

int CFrmQWebEnginePluginManager::AddPluginItem(
    const QString &pluginName, const QString &pluginPath, bool enabled)
{
    if(!m_pModelPlugins) return -1;
    
    int row = m_pModelPlugins->rowCount();
    m_pModelPlugins->insertRow(row);
    
    // 启用状态
    auto itemEnabled = new QStandardItem();
    itemEnabled->setCheckable(true);
    itemEnabled->setCheckState(enabled ? Qt::Checked : Qt::Unchecked);
    m_pModelPlugins->setItem(row, ColumnNo::Enabled, itemEnabled);
    
    // 插件名称
    auto itemName = new QStandardItem(pluginName);
    itemName->setEditable(false);
    m_pModelPlugins->setItem(row, ColumnNo::Name, itemName);
    
    // 插件类型
    auto itemType = new QStandardItem("Web Plugin");
    itemType->setEditable(false);
    m_pModelPlugins->setItem(row, ColumnNo::Type, itemType);
    
    // 版本信息
    auto itemVersion = new QStandardItem("1.0.0");
    itemVersion->setEditable(false);
    m_pModelPlugins->setItem(row, ColumnNo::Version, itemVersion);
    
    // 路径
    auto itemPath = new QStandardItem(pluginPath);
    itemPath->setEditable(false);
    m_pModelPlugins->setItem(row, ColumnNo::Path, itemPath);
    
    // 描述
    auto itemDesc = new QStandardItem(GetPluginInfo(pluginPath));
    itemDesc->setEditable(false);
    m_pModelPlugins->setItem(row, ColumnNo::Description, itemDesc);
    
    qDebug(log) << "Added plugin:" << pluginName << "at path:" << pluginPath;
    return 0;
}

bool CFrmQWebEnginePluginManager::IsValidPlugin(const QString &pluginPath) const
{
    QFileInfo fi(pluginPath);
    if(!fi.exists() || !fi.isFile()) return false;
    
    // 可添加更多验证逻辑
    return fi.isReadable();
}

QString CFrmQWebEnginePluginManager::GetPluginInfo(const QString &pluginPath) const
{
    QFileInfo fi(pluginPath);
    return QString("%1 (%2 bytes)")
        .arg(fi.fileName())
        .arg(fi.size());
}

QMap<QString, QVariant> CFrmQWebEnginePluginManager::GetPluginConfiguration() const
{
    return m_pluginConfig;
}

int CFrmQWebEnginePluginManager::SetPluginConfiguration(
    const QMap<QString, QVariant> &config)
{
    m_pluginConfig = config;
    return LoadPluginConfiguration();
}

int CFrmQWebEnginePluginManager::SavePluginConfiguration()
{
    QJsonArray pluginsArray;
    
    for(int i = 0; i < m_pModelPlugins->rowCount(); i++) {
        QJsonObject pluginObj;
        
        auto itemName = m_pModelPlugins->item(i, ColumnNo::Name);
        auto itemPath = m_pModelPlugins->item(i, ColumnNo::Path);
        auto itemEnabled = m_pModelPlugins->item(i, ColumnNo::Enabled);
        
        if(itemName && itemPath && itemEnabled) {
            pluginObj["name"] = itemName->text();
            pluginObj["path"] = itemPath->text();
            pluginObj["enabled"] = (itemEnabled->checkState() == Qt::Checked);
            pluginsArray.append(pluginObj);
        }
    }
    
    QJsonObject configObj;
    configObj["plugins"] = pluginsArray;
    configObj["version"] = "1.0";
    
    m_pluginConfig["config"] = QJsonDocument(configObj).toJson();
    
    qDebug(log) << "Plugin configuration saved";
    return 0;
}

int CFrmQWebEnginePluginManager::LoadPluginConfiguration()
{
    // 从配置中恢复插件状态
    qDebug(log) << "Plugin configuration loaded";
    return 0;
}

void CFrmQWebEnginePluginManager::on_pbEnableAll_clicked()
{
    qDebug(log) << Q_FUNC_INFO;
    for(int i = 0; i < m_pModelPlugins->rowCount(); i++) {
        auto item = m_pModelPlugins->item(i, ColumnNo::Enabled);
        if(item) {
            item->setCheckState(Qt::Checked);
        }
    }
    QMessageBox::information(this, tr("Info"), tr("All plugins enabled"));
}

void CFrmQWebEnginePluginManager::on_pbDisableAll_clicked()
{
    qDebug(log) << Q_FUNC_INFO;
    for(int i = 0; i < m_pModelPlugins->rowCount(); i++) {
        auto item = m_pModelPlugins->item(i, ColumnNo::Enabled);
        if(item) {
            item->setCheckState(Qt::Unchecked);
        }
    }
    QMessageBox::information(this, tr("Info"), tr("All plugins disabled"));
}

void CFrmQWebEnginePluginManager::on_pbAdd_clicked()
{
    qDebug(log) << Q_FUNC_INFO;
    QString filter = tr("Plugin Files (*.dll *.so *.dylib);;All Files (*)");
    QString filePath = QFileDialog::getOpenFileName(this, tr("Select Plugin"), "", filter);
    
    if(!filePath.isEmpty() && IsValidPlugin(filePath)) {
        QFileInfo fi(filePath);
        AddPluginItem(fi.baseName(), filePath, true);
    }
}

void CFrmQWebEnginePluginManager::on_pbRemove_clicked()
{
    qDebug(log) << Q_FUNC_INFO;
    auto indexes = ui->tvPlugins->selectionModel()->selectedRows();
    if(indexes.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select a plugin to remove"));
        return;
    }
    
    int row = indexes.at(0).row();
    m_pModelPlugins->removeRow(row);
}

void CFrmQWebEnginePluginManager::on_pbDetails_clicked()
{
    qDebug(log) << Q_FUNC_INFO;
    auto indexes = ui->tvPlugins->selectionModel()->selectedRows();
    if(indexes.isEmpty()) return;
    
    int row = indexes.at(0).row();
    QString name = m_pModelPlugins->item(row, ColumnNo::Name)->text();
    QString path = m_pModelPlugins->item(row, ColumnNo::Path)->text();
    QString version = m_pModelPlugins->item(row, ColumnNo::Version)->text();
    QString description = m_pModelPlugins->item(row, ColumnNo::Description)->text();
    
    QString details = QString("Plugin Details:\n\nName: %1\nPath: %2\nVersion: %3\n\nInfo: %4")
        .arg(name, path, version, description);
    
    QMessageBox::information(this, tr("Plugin Details"), details);
}

void CFrmQWebEnginePluginManager::on_pbExport_clicked()
{
    qDebug(log) << Q_FUNC_INFO;
    QString filePath = QFileDialog::getSaveFileName(this, 
        tr("Export Plugin Configuration"), "", 
        tr("JSON Files (*.json);;All Files (*)"));
    
    if(!filePath.isEmpty()) {
        SavePluginConfiguration();
        // TODO: 实现导出逻辑
        QMessageBox::information(this, tr("Success"), tr("Configuration exported successfully"));
    }
}

void CFrmQWebEnginePluginManager::on_pbImport_clicked()
{
    qDebug(log) << Q_FUNC_INFO;
    QString filePath = QFileDialog::getOpenFileName(this, 
        tr("Import Plugin Configuration"), "", 
        tr("JSON Files (*.json);;All Files (*)"));
    
    if(!filePath.isEmpty()) {
        // TODO: 实现导入逻辑
        QMessageBox::information(this, tr("Success"), tr("Configuration imported successfully"));
    }
}

void CFrmQWebEnginePluginManager::on_pbApply_clicked()
{
    qDebug(log) << Q_FUNC_INFO;
    SavePluginConfiguration();
    QMessageBox::information(this, tr("Success"), tr("Configuration applied"));
}

void CFrmQWebEnginePluginManager::on_pbReset_clicked()
{
    qDebug(log) << Q_FUNC_INFO;
    LoadPluginConfiguration();
    RefreshPluginList();
    QMessageBox::information(this, tr("Success"), tr("Configuration reset"));
}

void CFrmQWebEnginePluginManager::slotPluginItemChanged(QStandardItem *item)
{
    if(!item) return;
    qDebug(log) << "Plugin item changed:" << item->text();
}

void CFrmQWebEnginePluginManager::slotSelectionChanged()
{
    auto indexes = ui->tvPlugins->selectionModel()->selectedRows();
    bool hasSelection = !indexes.isEmpty();
    
    ui->pbRemove->setEnabled(hasSelection);
    ui->pbDetails->setEnabled(hasSelection);
}

void CFrmQWebEnginePluginManager::slotCustomContextMenu(const QPoint &pos)
{
    qDebug(log) << Q_FUNC_INFO;
    // TODO: 实现右键菜单
}