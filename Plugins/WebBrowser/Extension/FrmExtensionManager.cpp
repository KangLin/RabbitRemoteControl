// Author: Kang Lin <kl222@126.com>

#include <QClipboard>
#include <QApplication>
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
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QCryptographicHash>
#include <QDesktopServices>
#include <QUrl>
#include <QMenu>
#include <QToolBar>

#include "FrmExtensionManager.h"
#include "ParameterWebBrowser.h"
#include "ui_FrmExtensionManager.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.ExtensionManager")

CFrmExtensionManager::CFrmExtensionManager(
    QWebEngineProfile *profile, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CFrmExtensionManager)
    , m_pProfile(nullptr)
    , m_pModelExtensions(nullptr)
    , m_pInstall(nullptr)
    , m_pLoad(nullptr)
    , m_pUninstall(nullptr)
    , m_pRefresh(nullptr)
    , m_pCopyUrl(nullptr)
    , m_pCopyPath(nullptr)
    , m_pOpenFolder(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;

    ui->setupUi(this);
    setWindowTitle(tr("Chrome Extension Manager"));
    setWindowIcon(QIcon::fromTheme("extension"));

    InitializeUI();
    SetupConnections();

    SetProfile(profile);
}

CFrmExtensionManager::~CFrmExtensionManager()
{
    qDebug(log) << Q_FUNC_INFO;
    delete ui;
}

void CFrmExtensionManager::InitializeUI()
{
    QToolBar* pToolBar = ui->toolBar;
    m_pInstall = pToolBar->addAction(
        QIcon::fromTheme("system-software-install"), tr("Install Extension"),
        this, &CFrmExtensionManager::on_pbInstall_clicked);
    m_pLoad = pToolBar->addAction(
        QIcon::fromTheme("system-software-update"), tr("Load Extension"),
        this, &CFrmExtensionManager::on_pbLoad_clicked);
    m_pUninstall = pToolBar->addAction(
        QIcon::fromTheme("edit-delete"), tr("Uninstall"),
        this, &CFrmExtensionManager::on_pbUninstall_clicked);
    m_pRefresh = pToolBar->addAction(QIcon::fromTheme("view-refresh"), tr("Refresh"),
                                     this, &CFrmExtensionManager::on_pbRefresh_clicked);
    m_pCopyUrl = pToolBar->addAction(QIcon::fromTheme("edit-copy"), tr("Copy popup url to clipboard"),
                                     this, &CFrmExtensionManager::slotCopyPopupUrl);
    m_pCopyPath = pToolBar->addAction(QIcon::fromTheme("edit-copy"), tr("Copy path to clipboard"),
                                      this, &CFrmExtensionManager::slotCopyPath);
    m_pDetails = pToolBar->addAction(QIcon::fromTheme("dialog-information"), tr("Details"),
                this, &CFrmExtensionManager::on_pbDetails_clicked);
    m_pOpenFolder = pToolBar->addAction(QIcon::fromTheme("folder-open"), tr("Open Folder"),
                this, &CFrmExtensionManager::on_pbOpenExtensionPath_clicked);

    // 创建扩展列表模型
    m_pModelExtensions = new QStandardItemModel(this);

    // 设置表头
    QStringList headers;
    headers << tr("Name") << tr("Enabled") << tr("Description")
            << tr("Popup url") << tr("ID") << tr("Path");
    m_pModelExtensions->setHorizontalHeaderLabels(headers);
    
    // 配置表视图
    ui->tvExtensions->setModel(m_pModelExtensions);
    ui->tvExtensions->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tvExtensions->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tvExtensions->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tvExtensions->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tvExtensions->setAlternatingRowColors(true);
    
    // 设置列宽自适应
    ui->tvExtensions->horizontalHeader()->setStretchLastSection(true);
    ui->tvExtensions->verticalHeader()->setDefaultSectionSize(24);

    slotSelectionChanged();
    qDebug(log) << "UI initialized";
}

void CFrmExtensionManager::SetupConnections()
{
    bool check = true;
    // 模型和表视图连接
    check = connect(m_pModelExtensions, &QStandardItemModel::itemChanged,
                     this, &CFrmExtensionManager::slotExtensionItemChanged);
    Q_ASSERT(check);
    check = connect(ui->tvExtensions->selectionModel(), &QItemSelectionModel::selectionChanged,
                     this, &CFrmExtensionManager::slotSelectionChanged);
    Q_ASSERT(check);
    check = connect(ui->tvExtensions, &QTableView::customContextMenuRequested,
                     this, &CFrmExtensionManager::slotCustomContextMenu);
    Q_ASSERT(check);
}

int CFrmExtensionManager::SetProfile(QWebEngineProfile *profile)
{
    m_pProfile = profile;
    if(!m_pProfile) {
        qWarning(log) << "Failed to set profile: profile is null";
        return -1;
    }
    
    bool check = false;
    auto m = m_pProfile->extensionManager();
    m->disconnect(this);
    check = connect(m, &QWebEngineExtensionManager::installFinished,
                    this, &CFrmExtensionManager::slotInstallFinished);
    Q_ASSERT(check);
    check = connect(m, &QWebEngineExtensionManager::uninstallFinished,
                    this, &CFrmExtensionManager::slotUninstallFinished);
    Q_ASSERT(check);
    check = connect(m, &QWebEngineExtensionManager::loadFinished,
                    this, &CFrmExtensionManager::slotLoadFinished);
    Q_ASSERT(check);
    check = connect(m, &QWebEngineExtensionManager::unloadFinished,
                    this, &CFrmExtensionManager::slotUnloadFinished);
    Q_ASSERT(check);

    RefreshExtensionList();

    return 0;
}

void CFrmExtensionManager::InstallExtension(const QString &path)
{
    //qDebug(log) << Q_FUNC_INFO << "Path:" << path;

    if(!m_pProfile || !m_pProfile->extensionManager()) {
        qWarning(log) << "Profile is not set";
        return;
    }

    QFileInfo fi(path);
    if(!fi.exists()) {
        qCritical(log) << "The path is not exist" << path;
        return;
    }

    auto m = m_pProfile->extensionManager();
    if(fi.isFile())
        m->installExtension(path);
    else if(fi.isDir()){
        m->loadExtension(path);
    }
}

void CFrmExtensionManager::UninstallExtension(const QString &id)
{   
    if(!m_pProfile || !m_pProfile->extensionManager()) {
        qWarning(log) << "Profile is not set";
        return;
    }

    auto m = m_pProfile->extensionManager();
    foreach (auto e, m->extensions()) {
        if(e.id() == id)
            m->uninstallExtension(e);
    }
    
    //qDebug(log) << "Extension uninstalled:" << id;
}

void CFrmExtensionManager::EnableExtension(const QString &id, bool bEnable)
{
    qDebug(log) << Q_FUNC_INFO << "ID:" << id;
    if(id.isEmpty()) return;
    if(!m_pProfile || !m_pProfile->extensionManager()) {
        qWarning(log) << "Profile is not set";
        return;
    }

    auto m = m_pProfile->extensionManager();
    foreach (auto e, m->extensions()) {
        if(e.id() == id && e.isEnabled() != bEnable)
            m->setExtensionEnabled(e, bEnable);
    }
}

QList<QWebEngineExtensionInfo> CFrmExtensionManager::GetInstalledExtensions() const
{
    if(!m_pProfile || !m_pProfile->extensionManager()) {
        qWarning(log) << "Profile is not set";
        return QList<QWebEngineExtensionInfo>();
    }

    auto m = m_pProfile->extensionManager();
    return m->extensions();
}

QWebEngineExtensionInfo CFrmExtensionManager::GetExtensionInfo(const QString &id) const
{
    if(!m_pProfile || !m_pProfile->extensionManager()) {
        qWarning(log) << "Profile is not set";
        return QWebEngineExtensionInfo();
    }

    auto m = m_pProfile->extensionManager();
    foreach(auto e, m->extensions()) {
        if(e.id() == id)
            return e;
    }
    return QWebEngineExtensionInfo();
}

int CFrmExtensionManager::RefreshExtensionList()
{
    if(!m_pModelExtensions || !m_pProfile || !m_pProfile->extensionManager())
        return -1;

    m_pModelExtensions->removeRows(0, m_pModelExtensions->rowCount());

    qDebug(log) << "Refreshing extension list";

    auto extensions = m_pProfile->extensionManager()->extensions();
    foreach(auto e, extensions) {
        AddExtensionItem(e);
    }
    
    qDebug(log) << "Extension list refreshed, total:" << m_pModelExtensions->rowCount();
    return 0;
}

int CFrmExtensionManager::AddExtensionItem(const QWebEngineExtensionInfo &info)
{
    if(!m_pModelExtensions) return -1;

    int row = m_pModelExtensions->rowCount();
    m_pModelExtensions->insertRow(row);

    auto itemId = new QStandardItem(info.id());
    itemId->setEditable(false);
    m_pModelExtensions->setItem(row, ColumnNo::ID, itemId);

    auto itemName = new QStandardItem(info.name());
    itemName->setEditable(false);
    m_pModelExtensions->setItem(row, ColumnNo::Name, itemName);

    auto itemEnabled = new QStandardItem();
    itemEnabled->setEditable(false);
    itemEnabled->setCheckable(true);
    itemEnabled->setCheckState(info.isEnabled() ? Qt::Checked : Qt::Unchecked);
    m_pModelExtensions->setItem(row, ColumnNo::Enabled, itemEnabled);

    auto itemPath = new QStandardItem(info.path());
    itemPath->setEditable(false);
    m_pModelExtensions->setItem(row, ColumnNo::Path, itemPath);

    auto itemPopupUrl = new QStandardItem(info.actionPopupUrl().toString());
    itemPopupUrl->setEditable(false);
    m_pModelExtensions->setItem(row, ColumnNo::PopupUrl, itemPopupUrl);

    auto itemDesc = new QStandardItem(info.description());
    itemDesc->setEditable(false);
    m_pModelExtensions->setItem(row, ColumnNo::Description, itemDesc);

    qDebug(log) << "Added extension:" << info.name();
    return 0;
}

int CFrmExtensionManager::RemoveExtensionItem(const QString &extensionId)
{
    for(int i = 0; i < m_pModelExtensions->rowCount(); i++) {
        auto item = m_pModelExtensions->item(i, ColumnNo::ID);
        if(item && item->text() == extensionId) {
            m_pModelExtensions->removeRow(i);
            return 0;
        }
    }
    return -1;
}

void CFrmExtensionManager::on_pbInstall_clicked()
{
    QString filter = tr("Chrome Extension (*.crx);;All Files (*)");
    QString path = QFileDialog::getOpenFileName(
        this, tr("Install Chrome Extension"), "", filter);
    
    if(!path.isEmpty()) {
        InstallExtension(path);
    }
}

void CFrmExtensionManager::on_pbLoad_clicked()
{
    QString path = QFileDialog::getExistingDirectory(
        this, tr("Load Chrome Extension"));
    if(!path.isEmpty()) {
        InstallExtension(path);
    }
}

void CFrmExtensionManager::on_pbUninstall_clicked()
{
    qDebug(log) << Q_FUNC_INFO;

    auto indexes = ui->tvExtensions->selectionModel()->selectedRows();
    if(indexes.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), 
            tr("Please select an extension to uninstall"));
        return;
    }

    int row = indexes.at(0).row();
    auto idItem = m_pModelExtensions->item(row, ColumnNo::ID);
    auto nameItem = m_pModelExtensions->item(row, ColumnNo::Name);

    if(!idItem || !nameItem) return;

    int ret = QMessageBox::question(this, tr("Confirm"),
        tr("Do you want to uninstall '%1'?").arg(nameItem->text()),
        QMessageBox::Yes | QMessageBox::No);

    if(ret == QMessageBox::Yes) {
        UninstallExtension(idItem->text());
    }
}

void CFrmExtensionManager::on_pbRefresh_clicked()
{
    qDebug(log) << Q_FUNC_INFO;
    RefreshExtensionList();
}

void CFrmExtensionManager::on_pbDetails_clicked()
{
    qDebug(log) << Q_FUNC_INFO;

    auto indexes = ui->tvExtensions->selectionModel()->selectedRows();
    if(indexes.isEmpty()) return;

    int row = indexes.at(0).row();
    auto idItem = m_pModelExtensions->item(row, ColumnNo::ID);
    if(!idItem) return;

    auto info = GetExtensionInfo(idItem->text());

    QString details = tr("Extension Details:") + "\n";
    details += "  - " + tr("Name:") + " " + info.name() + "\n";
    details += "  - " + tr("Description:") + " " + info.description() + "\n";
    details += "  - " + tr("Enable:") + " " + (info.isEnabled() ? tr("TRUE") : tr("FALSE")) + "\n";
    details += "  - " + tr("ID:") + " " + info.id() + "\n";
    details += "  - " + tr("Popup url:") + " " + info.actionPopupUrl().toString() + "\n";
    details += "  - " + tr("Path:") + " " + info.path() + "\n";

    QMessageBox::information(this, tr("Extension Details"), details);
}

void CFrmExtensionManager::on_pbOpenDevTools_clicked()
{
    qDebug(log) << Q_FUNC_INFO;
    // TODO: 打开扩展开发者工具
    QMessageBox::information(this, tr("Info"),
        tr("Developer tools for extensions not yet implemented"));
}

void CFrmExtensionManager::on_pbOpenExtensionPath_clicked()
{
    qDebug(log) << Q_FUNC_INFO;

    auto indexes = ui->tvExtensions->selectionModel()->selectedRows();
    if(indexes.isEmpty()) return;

    int row = indexes.at(0).row();
    auto pathItem = m_pModelExtensions->item(row, ColumnNo::Path);
    if(pathItem && !pathItem->text().isEmpty()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(pathItem->text()));
    }
}

void CFrmExtensionManager::slotExtensionItemChanged(QStandardItem *item)
{
    if(!item) return;
    //qDebug(log) << "Extension item changed:" << item->text();
    if(!item->index().isValid()) return;
    if(item->index().column() == ColumnNo::Enabled) {
        auto idItem = m_pModelExtensions->item(item->index().row(), ColumnNo::ID);
        if(idItem) {
            EnableExtension(idItem->text(), item->checkState() == Qt::Checked);
        }
    }
}

void CFrmExtensionManager::slotSelectionChanged()
{
    auto indexes = ui->tvExtensions->selectionModel()->selectedRows();
    bool hasSelection = !indexes.isEmpty();

    m_pUninstall->setEnabled(hasSelection);
    m_pCopyUrl->setEnabled(hasSelection);
    m_pDetails->setEnabled(hasSelection);
    m_pOpenFolder->setEnabled(hasSelection);
}

void CFrmExtensionManager::slotCustomContextMenu(const QPoint &pos)
{
    qDebug(log) << Q_FUNC_INFO;
    QMenu m(this);
    auto indexes = ui->tvExtensions->selectionModel()->selectedRows();
    if(!indexes.isEmpty()) {
        m.addAction(m_pCopyUrl);
        m.addAction(m_pCopyPath);
        m.addAction(m_pDetails);
        m.addAction(m_pOpenFolder);
        m.addSeparator();
        m.addAction(m_pUninstall);
    }
    m.addAction(m_pInstall);
    m.addAction(m_pLoad);
    m.addAction(m_pRefresh);
    auto pw = qobject_cast<QWidget*>(sender());
    if(pw)
        m.exec(pw->mapToGlobal(pos));
}

void CFrmExtensionManager::slotInstallFinished(const QWebEngineExtensionInfo &extension)
{
    if(extension.isInstalled()) {
        QMessageBox::information(
            this, tr("Successful"),
            tr("Successfully installed extension") + " \"" +  extension.name() + "\"\n\n"
                + tr("Path:") + " " + extension.path());
        AddExtensionItem(extension);
    } else
        QMessageBox::critical(
            this, tr("Failed"),
            tr("Failed to install extension.") + "\n\n"
                + tr("Path:") + " " + extension.path() + "\n\n"
                + tr("Error:") + " " + extension.error());
}

void CFrmExtensionManager::slotUninstallFinished(const QWebEngineExtensionInfo &extension)
{
    if(!extension.isInstalled()) {
        QMessageBox::information(this, tr("Successful"),
            tr("Successfully uninstalled extension") + " \"" +  extension.name() + "\"");
        RemoveExtensionItem(extension.id());
    } else
        QMessageBox::critical(
            this, tr("Failed"),
            tr("Failed to uninstall extension.") + "\n\n"
                + tr("Path:") + " " + extension.path() + "\n\n"
                + tr("Error:") + " " + extension.error());
}

void CFrmExtensionManager::slotLoadFinished(const QWebEngineExtensionInfo &extension)
{
    if(extension.isLoaded()) {
        QMessageBox::information(
            this, tr("Successful"),
            tr("Successfully loaded extension") + " \"" +  extension.name() + "\"\n\n"
                + tr("Path:") + " " + extension.path());
        AddExtensionItem(extension);
    } else
        QMessageBox::critical(
            this, tr("Failed"),
            tr("Failed to load extension.") + "\n\n"
                + tr("Path:") + " " + extension.path() + "\n\n"
                + tr("Error:") + " " + extension.error());
}

void CFrmExtensionManager::slotUnloadFinished(const QWebEngineExtensionInfo &extension)
{
    if(!extension.isLoaded()) {
        QMessageBox::information(this, tr("Successful"),
                                 tr("Successfully unloaded extension") + " \"" +  extension.name() + "\"");
        RemoveExtensionItem(extension.id());
    } else
        QMessageBox::critical(
            this, tr("Failed"),
            tr("Failed to unload extension.") + "\n\n"
                + tr("Path:") + " " + extension.path() + "\n\n"
                + tr("Error:") + " " + extension.error());
}

void CFrmExtensionManager::slotCopyPopupUrl()
{
    if(!m_pModelExtensions) return;
    auto indexes = ui->tvExtensions->selectionModel()->selectedRows();
    if(indexes.isEmpty()) return;
    
    int row = indexes.at(0).row();
    auto item = m_pModelExtensions->item(row, ColumnNo::PopupUrl);
    
    QClipboard* clipboard = QApplication::clipboard();
    if(clipboard && item && !item->text().isEmpty()) {
        clipboard->setText(item->text());
    }
}

void CFrmExtensionManager::slotCopyPath()
{
    if(!m_pModelExtensions) return;
    auto indexes = ui->tvExtensions->selectionModel()->selectedRows();
    if(indexes.isEmpty()) return;
    
    int row = indexes.at(0).row();
    auto item = m_pModelExtensions->item(row, ColumnNo::Path);
    
    QClipboard* clipboard = QApplication::clipboard();
    if(clipboard && item && !item->text().isEmpty()) {
        clipboard->setText(item->text());
    }
}
