// Author: Kang Lin <kl222@126.com>

#include "FavoriteView.h"

#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include <QHeaderView>
#include <QSettings>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSettings>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QMessageBox>
#include <QDir>
#include <QLoggingCategory>
#include <QVBoxLayout>

#include "FavoriteMimeData.h"
#include "TitleBar.h"

static Q_LOGGING_CATEGORY(log, "Favorite")

CFavoriteView::CFavoriteView(QWidget *parent) : QWidget(parent)
    , m_pDockTitleBar(nullptr)
    , m_pTreeView(nullptr)
    , m_pModel(nullptr)
    , m_pDatabase(nullptr)
    , m_pToolBar(nullptr)
{
    bool check = false;
    //setFocusPolicy(Qt::NoFocus);

    setWindowTitle(tr("Favorite"));

    m_pDatabase = new CFavoriteDatabase(this);
    if(m_pDatabase)
        m_pDatabase->OpenDatabase("favorite");

    setupUI();

    m_pDockTitleBar = new RabbitCommon::CTitleBar(parent);
    // Create tools pushbutton in title bar
    m_pMenu = new QMenu(tr("Tools"), m_pDockTitleBar);
    check = connect(m_pMenu, SIGNAL(aboutToShow()), this, SLOT(slotMenu()));
    Q_ASSERT(check);
    QPushButton* pTools = m_pDockTitleBar->CreateSmallPushButton(
        QIcon::fromTheme("tools"), m_pDockTitleBar);
    pTools->setToolTip(tr("Tools"));
    pTools->setMenu(m_pMenu);
    QList<QWidget*> lstWidget;
    lstWidget << pTools;
    m_pDockTitleBar->AddWidgets(lstWidget);
}

CFavoriteView::~CFavoriteView()
{
    if(m_pDatabase) {
        delete m_pDatabase;
        m_pDatabase = nullptr;
    }
}

void CFavoriteView::setupUI()
{
    bool check = false;
    QVBoxLayout *pMainLayout = new QVBoxLayout(this);
    if(!pMainLayout) return;

    // 工具栏
    setupToolBar();
    if(m_pToolBar)
        pMainLayout->addWidget(m_pToolBar);

    setupTreeView();
    if(m_pTreeView)
        pMainLayout->addWidget(m_pTreeView);
}

void CFavoriteView::setupToolBar()
{
    if(m_pToolBar) return;
    bool check = false;
    m_pToolBar = new QToolBar(this);

    m_pStartAction = m_pToolBar->addAction(QIcon::fromTheme("media-playback-start"), tr("Start"));
    if(m_pStartAction) {
        m_pStartAction->setToolTip(m_pStartAction->text());
        m_pStartAction->setStatusTip(m_pStartAction->text());
        check = connect(m_pStartAction, &QAction::triggered, this, &CFavoriteView::slotStart);
        Q_ASSERT(check);
    }

    m_pEidtStartAction = m_pToolBar->addAction(QIcon::fromTheme("system-settings"), tr("Open settings and Start"));
    if(m_pEidtStartAction) {
        m_pEidtStartAction->setToolTip(m_pEidtStartAction->text());
        m_pEidtStartAction->setStatusTip(m_pEidtStartAction->text());
        check = connect(m_pEidtStartAction, &QAction::triggered, this, &CFavoriteView::slotOpenStart);
        Q_ASSERT(check);
    }

    m_pToolBar->addSeparator();

    m_pAddFolderAction = m_pToolBar->addAction(QIcon::fromTheme("folder-new"), tr("New group"));
    if(m_pAddFolderAction) {
        m_pAddFolderAction->setToolTip(m_pAddFolderAction->text());
        m_pAddFolderAction->setStatusTip(m_pAddFolderAction->text());
        check = connect(m_pAddFolderAction, &QAction::triggered, this, &CFavoriteView::slotNewGroup);
        Q_ASSERT(check);
    }

    m_pEditAction = m_pToolBar->addAction(QIcon::fromTheme("edit"), tr("Edit"));
    if(m_pEditAction) {
        m_pEditAction->setToolTip(m_pEditAction->text());
        m_pEditAction->setStatusTip(m_pEditAction->text());
        check = connect(m_pEditAction, &QAction::triggered, this, &CFavoriteView::slotEdit);
        Q_ASSERT(check);
    }

    m_pDeleteAction = m_pToolBar->addAction(QIcon::fromTheme("edit-delete"), tr("Delete"));
    if(m_pDeleteAction) {
        m_pDeleteAction->setToolTip(m_pDeleteAction->text());
        m_pDeleteAction->setStatusTip(m_pDeleteAction->text());
        check = connect(m_pDeleteAction, &QAction::triggered, this, &CFavoriteView::slotDelete);
        Q_ASSERT(check);
    }

    m_pToolBar->addSeparator();

    m_pImportAction = m_pToolBar->addAction(QIcon::fromTheme("import"), tr("Import"));
    if(m_pImportAction) {
        m_pImportAction->setToolTip(m_pImportAction->text());
        m_pImportAction->setStatusTip(m_pImportAction->text());
        check = connect(m_pImportAction, &QAction::triggered, this, &CFavoriteView::slotImport);
        Q_ASSERT(check);
    }

    m_pExportAction = m_pToolBar->addAction(QIcon::fromTheme("export"), tr("Export"));
    if(m_pExportAction) {
        m_pExportAction->setToolTip(m_pExportAction->text());
        m_pExportAction->setStatusTip(m_pExportAction->text());
        check = connect(m_pExportAction, &QAction::triggered, this, &CFavoriteView::slotExport);
        Q_ASSERT(check);
    }

    EnableAction();
}

void CFavoriteView::EnableAction(const QModelIndex &index)
{
    bool enable = false;
    bool favEnable = false;
    if(index.isValid()) {

        CFavoriteDatabase::Item item = m_pModel->data(index, CFavoriteModel::RoleItem).value<CFavoriteDatabase::Item>();
        if(item.isFavorite()) {
            m_pStartAction->setEnabled(true);
            m_pEidtStartAction->setEnabled(true);
        } else {
            m_pStartAction->setEnabled(false);
            m_pEidtStartAction->setEnabled(false);
        }
        m_pEditAction->setEnabled(true);
        m_pDeleteAction->setEnabled(true);
        m_pImportAction->setEnabled(true);
        m_pExportAction->setEnabled(true);
        return;
    }
    m_pStartAction->setEnabled(false);
    m_pEidtStartAction->setEnabled(false);
    m_pEditAction->setEnabled(false);
    m_pDeleteAction->setEnabled(false);
    m_pImportAction->setEnabled(false);
    m_pExportAction->setEnabled(false);
}

void CFavoriteView::setupTreeView()
{
    if(m_pTreeView) return;
    m_pTreeView = new QTreeView(this);
    if(!m_pTreeView)
        return;

    m_pTreeView->setAcceptDrops(true);
    m_pTreeView->setUniformRowHeights(true);
    m_pTreeView->setHeaderHidden(true); // 如果没有表头
    m_pModel = new CFavoriteModel(m_pDatabase, this);
    m_pTreeView->setModel(m_pModel);
    // 设置展开行为
    m_pTreeView->setExpandsOnDoubleClick(true);

    // 如果需要自动展开第一层
    //m_pTreeView->expandAll(); // 或者根据需要展开特定节点
    //m_pTreeView->expandToDepth(1);

    m_pTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    bool check = connect(m_pTreeView,
                         SIGNAL(customContextMenuRequested(const QPoint &)),
                         this, SLOT(slotCustomContextMenu(const QPoint &)));
    Q_ASSERT(check);
    check = connect(m_pTreeView, SIGNAL(clicked(const QModelIndex&)),
                    this, SLOT(slotFavrtieClicked(const QModelIndex&)));
    Q_ASSERT(check);
    check = connect(m_pTreeView, SIGNAL(doubleClicked(const QModelIndex&)),
                    this, SLOT(slotFavortiedoubleClicked(const QModelIndex&)));
    Q_ASSERT(check);

    //m_pTreeView->installEventFilter(this);
}

void CFavoriteView::slotAddToFavorite(const QString &szName,
                                      const QString &szDescription,
                                      const QIcon &icon,
                                      const QString &szFile)
{
    if(!m_pModel || !m_pTreeView) return;
    QStandardItem* pItem = nullptr;
    int parentId = 0;
    QString szGroup;
    auto indexes = m_pTreeView->selectionModel()->selectedIndexes();
    if(!indexes.isEmpty())
    {
        QModelIndex idx = indexes.at(0);
        while (idx.isValid()) {
            CFavoriteDatabase::Item item =
                m_pModel->data(idx, CFavoriteModel::RoleItem).value<CFavoriteDatabase::Item>();
            if(item.isGroup()) {
                parentId = item.id;
                break;
            }
            idx = idx.parent();
        }
    }
    bool ok = m_pModel->AddFavorite(icon, szName, szFile, szDescription, parentId);
    if(ok)
        m_pTreeView->viewport()->update();
    return;
}

void CFavoriteView::slotFavrtieClicked(const QModelIndex &index)
{
    EnableAction(index);
}

void CFavoriteView::slotFavortiedoubleClicked(const QModelIndex &index)
{
    if(!index.isValid()) return;
    if(m_pTreeView->editTriggers() != QTreeView::NoEditTriggers)
        return;
    QString szFile = m_pModel->data(index, CFavoriteModel::RoleFile).toString();
    if(!szFile.isEmpty())
        emit sigStart(szFile, false);
}

void CFavoriteView::slotDoubleEditNode(bool bEdit)
{
    if(bEdit)
        m_pTreeView->setEditTriggers(QTreeView::DoubleClicked);
    else
        m_pTreeView->setEditTriggers(QTreeView::NoEditTriggers);
}

void CFavoriteView::slotMenu()
{
    if(!m_pTreeView)
        return;

    m_pMenu->clear();
    m_pMenu->addAction(m_pStartAction);
    m_pMenu->addAction(m_pEidtStartAction);

    m_pMenu->addSeparator();
    m_pMenu->addAction(m_pAddFolderAction);
    m_pMenu->addAction(m_pEditAction);
    m_pMenu->addAction(m_pDeleteAction);

    // m_pMenu->addSeparator();
    // m_pMenu->addAction(tr("Add current operate to favorite"), this, SIGNAL(sigFavorite()));
}

void CFavoriteView::slotImport()
{

}

void CFavoriteView::slotExport()
{

}

void CFavoriteView::slotCustomContextMenu(const QPoint &pos)
{
    slotMenu();
    m_pMenu->exec(m_pTreeView->mapToGlobal(pos));
}

void CFavoriteView::slotStart()
{
    auto lstIndex = m_pTreeView->selectionModel()->selectedIndexes();
    foreach(auto index, lstIndex)
    {
        QString szFile = m_pModel->data(index, CFavoriteModel::RoleFile).toString();
        if(!szFile.isEmpty())
            emit sigStart(szFile, false);
    }
}

void CFavoriteView::slotOpenStart()
{
    auto lstIndex = m_pTreeView->selectionModel()->selectedIndexes();
    foreach(auto index, lstIndex)
    {
        QString szFile = m_pModel->data(index, CFavoriteModel::RoleFile).toString();
        if(!szFile.isEmpty())
            emit sigStart(szFile, true);
    }
}

void CFavoriteView::slotEdit()
{}

void CFavoriteView::slotDelete()
{
    auto lstIndex = m_pTreeView->selectionModel()->selectedIndexes();
    foreach(auto index, lstIndex)
        m_pModel->removeRow(index.row(), index.parent());
}

void CFavoriteView::slotNewGroup()
{
    if(!m_pModel) return;
    QString szGroup = QInputDialog::getText(this, tr("New group"), tr("Input group name"));
    if(szGroup.isEmpty()) return;

    int parentId = 0;
    auto lstIndex = m_pTreeView->selectionModel()->selectedIndexes();
    if(!lstIndex.isEmpty())
    {
        CFavoriteDatabase::Item item =
            m_pModel->data(lstIndex.at(0), CFavoriteModel::RoleItem).value<CFavoriteDatabase::Item>();
        if(0 < item.id && item.isFavorite())
            parentId = item.id;
    }

    QStandardItem* pGroup = new QStandardItem(szGroup);
    pGroup->setIcon(QIcon::fromTheme("network-workgroup"));
    m_pModel->AddNode(szGroup, parentId);
}

void CFavoriteView::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug(log) << "dragEnterEvent";
    const CFavoriteMimeData* pData = qobject_cast<const CFavoriteMimeData*>(event->mimeData());
    if (pData)
    {
        qDebug(log) << "dragEnterEvent acceptProposedAction";
        event->acceptProposedAction();
    }
}

void CFavoriteView::dragMoveEvent(QDragMoveEvent *event)
{
}

void CFavoriteView::dropEvent(QDropEvent *event)
{
    qDebug(log) << "dropEvent";
    const CFavoriteMimeData *pData = qobject_cast<const CFavoriteMimeData*>(event->mimeData());
    if(!pData) return;
    /*
    QStandardItemModel* pModel = dynamic_cast<QStandardItemModel*>(model());
    if(!pModel) return;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    auto index = indexAt(event->position().toPoint());
#else
    auto index = indexAt(event->pos());
#endif
    if(index.isValid())
    {
        auto item = pModel->itemFromIndex(index);
        if(item->data().isNull())
        {
            foreach(auto i, pData->m_Items)
            {
                qDebug(log) << "dropEvent:" << item->text();
                
                auto newItem = NewItem(i);
                item->appendRow(newItem);
                if(event->dropAction() == Qt::MoveAction)
                    pModel->removeRow(i.row(), i.parent());
            } 
        } else
            qWarning(log) << "Don't group node. the data:" << item->data();
    }else{
        foreach(auto i, pData->m_Items)
        {
            pModel->appendRow(NewItem(i));
            if(event->dropAction() == Qt::MoveAction)
                pModel->removeRow(i.row(), i.parent());
        }
    }
//*/
    event->accept();
}
/*
QStandardItem* CFavoriteView::NewItem(const QModelIndex &index)
{
    QStandardItemModel* pModel = dynamic_cast<QStandardItemModel*>(model());
    if(!pModel) return nullptr;
    auto item = pModel->itemFromIndex(index);
    if(!item) return nullptr;
    auto ri = new QStandardItem(item->text());
    ri->setData(item->data());
    return ri;
}

void CFavoriteView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        m_DragStartPosition = event->pos();
    QTreeView::mousePressEvent(event);
}

void CFavoriteView::mouseMoveEvent(QMouseEvent *event)
{
    qDebug(log) << "mouseMoveEvent";
    do{
        if (!(event->buttons() & Qt::LeftButton))
            break;
        if ((event->pos() - m_DragStartPosition).manhattanLength()
                < QApplication::startDragDistance())
            break;
        qDebug(log) << "mouseMoveEvent drag";
        QDrag *drag = new QDrag(this);
        CFavoriteMimeData *pData = new CFavoriteMimeData();
        pData->m_Items = this->selectionModel()->selectedIndexes();
        drag->setMimeData(pData);
        
        Qt::DropAction dropAction = Qt::MoveAction;
        if(event->modifiers() & Qt::ControlModifier)
            dropAction = Qt::CopyAction;
        drag->exec(dropAction);
    } while (false);
    
    QTreeView::mouseMoveEvent(event);
}
//*/
bool CFavoriteView::eventFilter(QObject *watched, QEvent *event)
{

    return QWidget::eventFilter(watched, event);
}
