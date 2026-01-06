#include <QLoggingCategory>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPluginLoader>
#include <QPushButton>
#include <QFileDialog>
#include "Plugin.h"
#include "RabbitCommonDir.h"
#include "FrmManagePlugins.h"
#include "CheckBoxHeader.h"
#include "ui_FrmManagePlugins.h"

static Q_LOGGING_CATEGORY(log, "Manage")
CFrmManagePlugins::CFrmManagePlugins(QWidget *parent) : CParameterUI(parent)
    , m_pPara(nullptr)
    , ui(new Ui::CFrmManagePluginsUI)
    , m_pModelFilter(nullptr)
{
    ui->setupUi(this);
    setWindowTitle(tr("Load Plugins"));

    bool check = connect(ui->lvPluginsPath, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(slotCustomContextPluginsPath(QPoint)));
    Q_ASSERT(check);
    m_pModelPluginPath = new QStandardItemModel(ui->lvPluginsPath);
    ui->lvPluginsPath->setModel(m_pModelPluginPath);

    ui->tvFilter->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tvFilter->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tvFilter->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_pModelFilter = new QStandardItemModel(ui->tvFilter);
    ui->tvFilter->setModel(m_pModelFilter);
    check = connect(m_pModelFilter, SIGNAL(itemChanged(QStandardItem*)),
                    this, SLOT(slotFilterItemChanged(QStandardItem*)));
    Q_ASSERT(check);
    SetFilterHeader();

    //必须在 setModel 后,才能应用
    /*第二个参数可以为：
    QHeaderView::Interactive     ：0 用户可设置，也可被程序设置成默认大小
    QHeaderView::Fixed           ：2 用户不可更改列宽
    QHeaderView::Stretch         ：1 根据空间，自动改变列宽，用户与程序不能改变列宽
    QHeaderView::ResizeToContents：3 根据内容改变列宽，用户与程序不能改变列宽
    */
    ui->tvFilter->horizontalHeader()->setSectionResizeMode(
#if defined(DEBUG) && !defined(Q_OS_ANDROID)
        0,
#endif
        QHeaderView::Interactive);
}

int CFrmManagePlugins::SetParameter(CParameter *pParameter)
{
    m_pPara = qobject_cast<CParameterPlugin*>(pParameter);
    bool checked = m_pPara->GetEnableSetPluginsPath();
    ui->gbPluginsPath->setChecked(checked);
    on_gbPluginsPath_clicked(checked);
    ui->cbOnlyLoadInWhitelist->setChecked(m_pPara->GetOnlyLoadInWhitelist());
    return 0;
}

int CFrmManagePlugins::Accept()
{
    QStringList lstPath;
    m_pPara->SetEnableSetPluginsPath(ui->gbPluginsPath->isChecked());
    for(int i = 0; i < m_pModelPluginPath->rowCount(); i++) {
        lstPath << m_pModelPluginPath->item(i)->text();
    }
    m_pPara->SetPluginsPath(lstPath);
    m_pPara->m_WhiteList.Clear();
    m_pPara->m_BlackList.Clear();
    for(int i = 0; i < m_pModelFilter->rowCount(); i++) {
        QString szPath = m_pModelFilter->item(i, ColumnNo::File)->text();
        if(szPath.isEmpty()) continue;
        auto pWhitelist = m_pModelFilter->item(i, ColumnNo::Whitelist);
        if(pWhitelist->checkState() == Qt::Checked)
            m_pPara->m_WhiteList.AddKey(szPath);
        auto pBlacklist = m_pModelFilter->item(i, ColumnNo::Blacklist);
        if(pBlacklist->checkState() == Qt::Checked)
            m_pPara->m_BlackList.AddKey(szPath);
    }
    m_pPara->SetOnlyLoadInWhitelist(ui->cbOnlyLoadInWhitelist->isChecked());
    return 0;
}

int CFrmManagePlugins::AddPath(const QString &szPath)
{
    auto pFind = m_pModelPluginPath->findItems(szPath);
    if(!pFind.isEmpty()) return 0;

    auto pPath = new QStandardItem(szPath);
    m_pModelPluginPath->appendRow(pPath);

    QStringList filters;
    if(filters.isEmpty())
    {
#if defined (Q_OS_WINDOWS) || defined(Q_OS_WIN)
        filters << "*Plugin*.dll";
#elif defined(Q_OS_MACOS) || defined(Q_OS_MAC)
        filters << "*Plugin*.dylib";
#else
        filters << "*Plugin*.so";
#endif
    }
    FindPlugins(szPath, filters);
    slotFilterItemChanged(m_pModelFilter->item(0, ColumnNo::Whitelist));
    slotFilterItemChanged(m_pModelFilter->item(0, ColumnNo::Blacklist));
    return 0;
}

int CFrmManagePlugins::FindPlugins(QDir dir, QStringList filters, bool bAdd)
{
    QStringList files = dir.entryList(filters, QDir::Files | QDir::NoDotAndDotDot);

    foreach (auto fileName, files) {
        QString szPath = dir.absoluteFilePath(fileName);
        if(!bAdd) {
            RemoveItem(fileName);
            continue;
        }
        auto pFind = m_pModelFilter->findItems(szPath, Qt::MatchExactly, ColumnNo::File);
        if(!pFind.isEmpty()) continue;

        QPluginLoader loader(szPath);
        QObject *plugin = loader.instance();
        if(plugin) {
            CPlugin* p = qobject_cast<CPlugin*>(plugin);
            if(p)
                AddItem(p, fileName);
            loader.unload();
        }
    }

    foreach (auto d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QDir pluginDir = dir;
        if(pluginDir.cd(d))
            FindPlugins(pluginDir, filters);
    }

    return 0;
}

int CFrmManagePlugins::AddItem(CPlugin* plugin, const QString& szPath)
{
    QList<QStandardItem*> lstItems;
    auto pWhitelist = new QStandardItem();
    pWhitelist->setCheckable(true);
    if(m_pPara->m_WhiteList.contains(szPath))
        pWhitelist->setCheckState(Qt::Checked);
    lstItems << pWhitelist;
    auto pBlacklist = new QStandardItem();
    pBlacklist->setCheckable(true);
    if(m_pPara->m_BlackList.contains(szPath))
        pBlacklist->setCheckState(Qt::Checked);
    lstItems << pBlacklist;
    auto pName = new QStandardItem(plugin->Icon(), plugin->DisplayName());
    lstItems << pName;
    auto pType = new QStandardItem(plugin->TypeName(plugin->Type()));
    lstItems << pType;
    auto pPath = new QStandardItem(szPath);
    lstItems << pPath;
    m_pModelFilter->appendRow(lstItems);

    //以下设置列宽函数必须要数据加载完成后使用,才能应用
    ui->tvFilter->resizeColumnsToContents(); //设置所有列宽度自适应内容
    return 0;
}

int CFrmManagePlugins::RemoveItem(const QString &szPath)
{
    auto pFind = m_pModelFilter->findItems(szPath, Qt::MatchExactly, ColumnNo::File);
    if(pFind.isEmpty()) return 0;
    foreach(auto item, pFind) {
        m_pModelFilter->removeRow(item->index().row());
    }
    return 0;
}

void CFrmManagePlugins::on_pbAdd_clicked()
{
    QString szPath = QFileDialog::getExistingDirectory(
        this, tr("Plugin path"),
        RabbitCommon::CDir::Instance()->GetDirPlugins());
    if(szPath.isEmpty()) return;
    AddPath(szPath);
}

void CFrmManagePlugins::on_pbRemove_clicked()
{
    QModelIndex index = ui->lvPluginsPath->currentIndex();
    if(!index.isValid())
        return;

    auto item = m_pModelPluginPath->item(index.row());
    if(item) {
        QString szPath = item->text();
        if(!szPath.isEmpty()) {
            QStringList filters;
            if(filters.isEmpty())
            {
#if defined (Q_OS_WINDOWS) || defined(Q_OS_WIN)
                filters << "*Plugin*.dll";
#elif defined(Q_OS_MACOS) || defined(Q_OS_MAC)
                filters << "*Plugin*.dylib";
#else
                filters << "*Plugin*.so";
#endif
            }
            FindPlugins(szPath, filters, false);
        }
    }
    m_pModelPluginPath->removeRow(index.row());
    slotFilterItemChanged(m_pModelFilter->item(0, ColumnNo::Whitelist));
    slotFilterItemChanged(m_pModelFilter->item(0, ColumnNo::Blacklist));
}

void CFrmManagePlugins::slotCustomContextPluginsPath(const QPoint &pos)
{
    QMenu m;
    m.addAction(tr("Add"), this, SLOT(on_pbAdd_clicked()));
    m.addAction(tr("Remove"), this, SLOT(on_pbRemove_clicked()));
    m.exec(this->mapToGlobal(pos));
}

void CFrmManagePlugins::on_gbPluginsPath_clicked(bool checked)
{
    m_pModelPluginPath->clear();
    m_pModelFilter->clear();
    SetFilterHeader();

    QStringList lstPaths;
    if(checked)
        lstPaths = m_pPara->GetPluginsPath();
    else
        lstPaths << RabbitCommon::CDir::Instance()->GetDirPlugins();
    foreach(auto szPath, lstPaths) {
        AddPath(szPath);
    }
}

int CFrmManagePlugins::SetFilterHeader()
{
    if(!m_pModelFilter) return -1;
    auto pWhitelist = new QStandardItem(tr("Whitelist"));
    pWhitelist->setToolTip(pWhitelist->text());
    pWhitelist->setCheckable(true);
    m_pModelFilter->setHorizontalHeaderItem(ColumnNo::Whitelist, pWhitelist);
    auto pBlacklist = new QStandardItem(tr("Blacklist"));
    pBlacklist->setToolTip(pBlacklist->text());
    pBlacklist->setCheckable(true);
    m_pModelFilter->setHorizontalHeaderItem(ColumnNo::Blacklist, pBlacklist);
    m_pModelFilter->setHorizontalHeaderItem(ColumnNo::Name, new QStandardItem(tr("Name")));
    m_pModelFilter->setHorizontalHeaderItem(ColumnNo::Type, new QStandardItem(tr("Type")));
    m_pModelFilter->setHorizontalHeaderItem(ColumnNo::File, new QStandardItem(tr("File")));
    
    if(qobject_cast<CCheckBoxHeader*>(ui->tvFilter->horizontalHeader()))
        return 0;

    auto pHeader = new CCheckBoxHeader(Qt::Horizontal, ui->tvFilter);
    if(pHeader) {
        ui->tvFilter->setHorizontalHeader(pHeader);
        bool check = false;
        check = connect(pHeader, &CCheckBoxHeader::sigCheckStateChanged,
                        this, [this](int index, Qt::CheckState state) {
            qDebug(log) << "CCheckBoxHeader::sigCheckStateChanged";
            for(int row = 0; row < m_pModelFilter->rowCount(); row++) {
                auto item = m_pModelFilter->item(row, index);
                switch(state)
                {
                case Qt::PartiallyChecked: {
                    QString szPath = m_pModelFilter->item(row, ColumnNo::File)->text();
                    if(ColumnNo::Whitelist == index) {
                        if(m_pPara->m_WhiteList.contains(szPath))
                            item->setCheckState(Qt::Checked);
                        else
                            item->setCheckState(Qt::Unchecked);
                    }
                    if(ColumnNo::Blacklist == index) {
                        if(m_pPara->m_BlackList.contains(szPath))
                            item->setCheckState(Qt::Checked);
                        else
                            item->setCheckState(Qt::Unchecked);
                    }
                    break;
                }
                default:
                    item->setCheckState(state);
                    break;
                }
            }
        });
        Q_ASSERT(check);
    }

    return 0;
}

void CFrmManagePlugins::slotFilterItemChanged(QStandardItem *item)
{
    qDebug(log) << Q_FUNC_INFO;
    if(!m_pModelFilter) return;
    if(!item) return;
    if(!item->isCheckable()) return;
    for(int i = 0; i < m_pModelFilter->rowCount(); i++) {
        if(i == item->row()) continue;
        auto state = m_pModelFilter->item(i, item->column())->checkState();
        if(state != item->checkState()) {
            m_pModelFilter->horizontalHeaderItem(item->column())->setCheckState(Qt::PartiallyChecked);
            return;
        }
    }
    m_pModelFilter->horizontalHeaderItem(item->column())->setCheckState(item->checkState());
}
