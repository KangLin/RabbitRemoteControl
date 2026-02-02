// Author: Kang Lin <kl222@126.com>

#include <QFileDialog>
#include <QToolBar>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include <QHeaderView>
#include <QSettings>
#include <QSettings>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QMessageBox>
#include <QDir>
#include <QLoggingCategory>
#include <QVBoxLayout>

#include "RabbitCommonDir.h"
#include "FavoriteMimeData.h"
#include "TitleBar.h"
#include "FavoriteView.h"

static Q_LOGGING_CATEGORY(log, "Favorite")

CFavoriteView::CFavoriteView(QWidget *parent) : QWidget(parent)
    , m_pDockTitleBar(nullptr)
    , m_pTreeView(nullptr)
    , m_pModel(nullptr)
    , m_pDatabase(nullptr)
    , m_pStartAction(nullptr)
    , m_pEidtStartAction(nullptr)
    , m_pAddFolderAction(nullptr)
    , m_pEditAction(nullptr)
    , m_pDeleteAction(nullptr)
    , m_pImportAction(nullptr)
    , m_pExportAction(nullptr)
    , m_pRefresh(nullptr)
    , m_pMenu(nullptr)
{
    bool check = false;

    setWindowTitle(tr("Favorite"));

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

    setupUI();
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
    QVBoxLayout *pMainLayout = new QVBoxLayout(this);
    if(!pMainLayout) return;

    setupToolBar(pMainLayout);

    setupTreeView(pMainLayout);
}

void CFavoriteView::setupToolBar(QLayout *layout)
{
    if(!layout) return;
    bool check = false;
    QToolBar* pToolBar = new QToolBar(this);
    if(!pToolBar) return;
    layout->addWidget(pToolBar);

    m_pStartAction = pToolBar->addAction(
        QIcon::fromTheme("media-playback-start"), tr("Start"));
    if(m_pStartAction) {
        m_pStartAction->setToolTip(m_pStartAction->text());
        m_pStartAction->setStatusTip(m_pStartAction->text());
        check = connect(m_pStartAction, &QAction::triggered,
                        this, &CFavoriteView::slotStart);
        Q_ASSERT(check);
    }

    m_pEidtStartAction = pToolBar->addAction(
        QIcon::fromTheme("system-settings"), tr("Open settings and Start"));
    if(m_pEidtStartAction) {
        m_pEidtStartAction->setToolTip(m_pEidtStartAction->text());
        m_pEidtStartAction->setStatusTip(m_pEidtStartAction->text());
        check = connect(m_pEidtStartAction, &QAction::triggered,
                        this, &CFavoriteView::slotOpenStart);
        Q_ASSERT(check);
    }

    pToolBar->addSeparator();

    m_pAddFolderAction = pToolBar->addAction(
        QIcon::fromTheme("folder-new"), tr("New group"));
    if(m_pAddFolderAction) {
        m_pAddFolderAction->setToolTip(m_pAddFolderAction->text());
        m_pAddFolderAction->setStatusTip(m_pAddFolderAction->text());
        check = connect(m_pAddFolderAction, &QAction::triggered,
                        this, &CFavoriteView::slotNewGroup);
        Q_ASSERT(check);
    }

    m_pEditAction = pToolBar->addAction(QIcon::fromTheme("edit"), tr("Edit"));
    if(m_pEditAction) {
        m_pEditAction->setToolTip(m_pEditAction->text());
        m_pEditAction->setStatusTip(m_pEditAction->text());
        check = connect(m_pEditAction, &QAction::triggered,
                        this, &CFavoriteView::slotEdit);
        Q_ASSERT(check);
    }

    m_pDeleteAction = pToolBar->addAction(QIcon::fromTheme("edit-delete"), tr("Delete"));
    if(m_pDeleteAction) {
        m_pDeleteAction->setToolTip(m_pDeleteAction->text());
        m_pDeleteAction->setStatusTip(m_pDeleteAction->text());
        check = connect(m_pDeleteAction, &QAction::triggered, this, &CFavoriteView::slotDelete);
        Q_ASSERT(check);
    }

    pToolBar->addSeparator();

    m_pImportAction = pToolBar->addAction(QIcon::fromTheme("import"), tr("Import"));
    if(m_pImportAction) {
        m_pImportAction->setToolTip(m_pImportAction->text());
        m_pImportAction->setStatusTip(m_pImportAction->text());
        check = connect(m_pImportAction, &QAction::triggered, this, &CFavoriteView::slotImport);
        Q_ASSERT(check);
    }

    m_pExportAction = pToolBar->addAction(QIcon::fromTheme("export"), tr("Export"));
    if(m_pExportAction) {
        m_pExportAction->setToolTip(m_pExportAction->text());
        m_pExportAction->setStatusTip(m_pExportAction->text());
        check = connect(m_pExportAction, &QAction::triggered, this, &CFavoriteView::slotExport);
        Q_ASSERT(check);
    }

    pToolBar->addSeparator();
    m_pRefresh = pToolBar->addAction(QIcon::fromTheme("refresh"), tr("Refresh"), this, [&](){
        if(m_pModel)
            m_pModel->Refresh();
    });
}

void CFavoriteView::setupTreeView(QLayout *layout)
{
    if(!layout) return;
    m_pTreeView = new QTreeView(this);
    if(!m_pTreeView)
        return;
    layout->addWidget(m_pTreeView);

    m_pTreeView->setAcceptDrops(true);
    m_pTreeView->setUniformRowHeights(true);
    m_pTreeView->setHeaderHidden(true);

    //m_pTreeView->setExpandsOnDoubleClick(true);
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

void CFavoriteView::Initial()
{
    m_pDatabase = new CFavoriteDatabase(this);
    if(m_pDatabase)
        m_pDatabase->OpenDatabase("favorite_connect");
    if(m_pDatabase) {
        m_pModel = new CFavoriteModel(m_pDatabase, this);
        m_pTreeView->setModel(m_pModel);
    }

    EnableAction();
}

void CFavoriteView::EnableAction(const QModelIndex &index)
{
    if(!m_pModel) return;
    if(index.isValid()) {
        CFavoriteDatabase::Item item =
            m_pModel->data(index, CFavoriteModel::RoleItem)
                                           .value<CFavoriteDatabase::Item>();
        if(item.isFavorite()) {
            m_pStartAction->setEnabled(true);
            m_pEidtStartAction->setEnabled(true);
        } else {
            m_pStartAction->setEnabled(false);
            m_pEidtStartAction->setEnabled(false);
        }
        m_pEditAction->setEnabled(true);
        m_pDeleteAction->setEnabled(true);
        m_pExportAction->setEnabled(true);
        return;
    }

    if(m_pStartAction)
        m_pStartAction->setEnabled(false);
    if(m_pEidtStartAction)
        m_pEidtStartAction->setEnabled(false);
    if(m_pEditAction)
        m_pEditAction->setEnabled(false);
    if(m_pDeleteAction)
        m_pDeleteAction->setEnabled(false);
    if(m_pExportAction)
        m_pExportAction->setEnabled(false);
}

void CFavoriteView::slotAddToFavorite(const QString &szFile,
                                      const QString &szName,
                                      const QString &szDescription,
                                      const QIcon &icon
                                      )
{
    if(!m_pModel || !m_pTreeView) return;

    int parentId = 0;
    QString szGroup = tr("Root");
    auto indexes = m_pTreeView->selectionModel()->selectedIndexes();
    if(!indexes.isEmpty())
    {
        QModelIndex idx = indexes.at(0);
        while (idx.isValid()) {
            CFavoriteDatabase::Item item =
                m_pModel->data(idx, CFavoriteModel::RoleItem).value<CFavoriteDatabase::Item>();
            if(item.isFolder() && item.id > 0) {
                parentId = item.id;
                szGroup = item.szName;
                break;
            }
            idx = idx.parent();
        }
    }
    // Check if it already exists
    auto item = m_pModel->GetFavorite(szFile);
    if(item.id > 0) {
        if(item.parentId == parentId) {
            QMessageBox::information(
                nullptr, tr("Add favorite"),
                tr("The operation already exists in \"%1\"").arg(szGroup));
            return;
        }
        int ret = QMessageBox::warning(
            nullptr, tr("Add favorite"),
            tr("The operation already exists, do you want to move it to \"%1\"?").arg(szGroup),
            QMessageBox::Ok | QMessageBox::No);
        if(QMessageBox::Ok != ret)
            return;
    }

    m_pModel->AddFavorite(szFile, szName, icon, szDescription, parentId);

    return;
}

void CFavoriteView::slotUpdateFavorite(
    const QString &szFile, const QString &szName,
    const QString &szDescription, const QIcon &icon)
{
    if(!m_pModel || !m_pTreeView || szFile.isEmpty()) return;
    m_pModel->UpdateFavorite(szFile, szName, szDescription, icon);
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

    m_pMenu->addSeparator();
    m_pMenu->addAction(m_pImportAction);
    m_pMenu->addAction(m_pExportAction);

    m_pMenu->addSeparator();
    m_pMenu->addAction(m_pRefresh);
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
{
    auto lstIndex = m_pTreeView->selectionModel()->selectedIndexes();
    foreach(auto index, lstIndex)
    {
        QString szName = m_pModel->data(index).toString();
        szName = QInputDialog::getText(
            this, tr("Edit"), tr("Enter the modified name"), QLineEdit::Normal, szName);
        if(szName.isEmpty())
            return;
        m_pModel->setData(index, szName);
    }
}

void CFavoriteView::slotDelete()
{
    auto lstIndex = m_pTreeView->selectionModel()->selectedIndexes();
    if(1 == lstIndex.size()) {
        CFavoriteDatabase::Item item =
            m_pModel->data(lstIndex.at(0), CFavoriteModel::RoleItem)
                                           .value<CFavoriteDatabase::Item>();
        if(0 < item.id) {
            int ret = QMessageBox::warning(
                nullptr, tr("Delete"), tr("Will be delete \"%1\"").arg(item.szName),
                QMessageBox::Ok|QMessageBox::No);
            if(QMessageBox::Ok != ret)
                return;
        }
    }
    foreach(auto index, lstIndex)
        m_pModel->removeRow(index.row(), index.parent());
}

void CFavoriteView::slotNewGroup()
{
    if(!m_pModel) return;
    QString szGroup = QInputDialog::getText(
        this, tr("New group"), tr("Input group name"));
    if(szGroup.isEmpty()) return;

    int parentId = 0;
    auto lstIndex = m_pTreeView->selectionModel()->selectedIndexes();
    if(!lstIndex.isEmpty())
    {
        CFavoriteDatabase::Item item =
            m_pModel->data(lstIndex.at(0), CFavoriteModel::RoleItem)
                                           .value<CFavoriteDatabase::Item>();
        if(0 < item.id && item.isFolder())
            parentId = item.id;
    }

    m_pModel->AddNode(szGroup, parentId);
}

void CFavoriteView::slotRefresh()
{
    if(m_pModel)
        m_pModel->Refresh();
}

void CFavoriteView::slotImport()
{
    QString filename = QFileDialog::getOpenFileName(
        this, tr("Import favorite"),
        RabbitCommon::CDir::Instance()->GetDirUserDocument(),
        tr("JSON (*.json)"));

    if (!filename.isEmpty()) {
        QFileInfo fi(filename);
        if(0 == fi.suffix().compare("json", Qt::CaseInsensitive)) {
            if (m_pDatabase->Import(filename)) {
                slotRefresh();
                QMessageBox::information(
                    this, tr("Success"),
                    tr("Favorite import from json file successfully"));
            } else {
                QMessageBox::warning(
                    this, tr("Failure"),
                    tr("Failed to import favorite from json file"));
            }
            return;
        }
    }
}

void CFavoriteView::slotExport()
{
    QString filename = QFileDialog::getSaveFileName(
        this, tr("Export favorite"),
        RabbitCommon::CDir::Instance()->GetDirUserDocument(),
        tr("JSON (*.json)"));

    if (!filename.isEmpty()) {
        QFileInfo fi(filename);
        if(0 == fi.suffix().compare("json", Qt::CaseInsensitive)) {
            if (m_pDatabase->Export(filename)) {
                QMessageBox::information(
                    this, tr("Success"),
                    tr("Favorite exported to json file successfully"));
            } else {
                QMessageBox::warning(
                    this, tr("Failure"),
                    tr("Failed to export favorite to json file"));
            }
            return;
        }
    }
}

void CFavoriteView::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug(log) << "dragEnterEvent";
    const CFavoriteMimeData* pData =
        qobject_cast<const CFavoriteMimeData*>(event->mimeData());
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
