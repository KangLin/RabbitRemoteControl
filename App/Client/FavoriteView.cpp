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

#include "FavoriteMimeData.h"
#include "RabbitCommonDir.h"

Q_LOGGING_CATEGORY(FavoriteLogger, "App.Favorite")

Q_DECLARE_LOGGING_CATEGORY(FavoriteLogger)

CFavoriteView::CFavoriteView(QWidget *parent) : QTreeView(),
    m_pModel(nullptr)
{
    setWindowTitle(tr("Favorite"));
    setFocusPolicy(Qt::NoFocus);
    header()->hide();
    
    setAcceptDrops(true);
    
    m_pModel = new QStandardItemModel(this);
    setModel(m_pModel);

    setContextMenuPolicy(Qt::CustomContextMenu);
    bool check = connect(this,
                         SIGNAL(customContextMenuRequested(const QPoint &)),
                         this, SLOT(slotCustomContextMenu(const QPoint &)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(clicked(const QModelIndex&)),
                    this, SLOT(slotFavrtieClicked(const QModelIndex&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(doubleClicked(const QModelIndex&)),
                    this, SLOT(slotFavortiedoubleClicked(const QModelIndex&)));
    Q_ASSERT(check);
    
    m_szSaveFile = RabbitCommon::CDir::Instance()->GetDirUserConfig()
            + QDir::separator() + "Favorite.ini";
    QSettings set(m_szSaveFile, QSettings::IniFormat);
    int nRootCount = 0;
    nRootCount = set.value("RootCount").toInt();
    for(int i = 0; i < nRootCount; i++)
    {
        QString name = set.value("Name_" + QString::number(i)).toString();
        QString file = set.value("File_" + QString::number(i)).toString();
        if(name.isEmpty()) {
            qCritical(FavoriteLogger) << "Current node is empty!";
            continue;
        }
        QStandardItem* item = new QStandardItem(name);
        item->setIcon(QIcon::fromTheme("network-wired"));
        if(item)
        {
            item->setData(file);
            m_pModel->appendRow(item);
        }
    }
    int nGroupCount = set.value("GroupCount").toInt();
    for(int g = 0; g < nGroupCount; g++)
    {
        QString szGroup = set.value("Group_" + QString::number(g)).toString();
        QStandardItem* pGroup = new QStandardItem(szGroup);
        pGroup->setIcon(QIcon::fromTheme("network-workgroup"));
        m_pModel->appendRow(pGroup);
        int n = set.value(szGroup + "/" + "count").toInt();
        for(int i = 0; i < n; i++)
        {
            QString name = set.value(szGroup + "/Name_" + QString::number(i)).toString();
            QString file = set.value(szGroup + "/File_" + QString::number(i)).toString();
            if(name.isEmpty())
            {
                qCritical(FavoriteLogger) << "Current node is empty!";
                continue;
            }
            QStandardItem* item = new QStandardItem(name);
            item->setIcon(QIcon::fromTheme("network-wired"));
            if(item)
            {
                item->setData(file);
                pGroup->appendRow(item);
            }
        }
    }
}

CFavoriteView::~CFavoriteView()
{
    Save();
}

int CFavoriteView::Save()
{
    int nRet = 0;
    if(!m_pModel)
        return -1;
    QSettings set(m_szSaveFile, QSettings::IniFormat);
    int nRootCount = 0;
    int nGroup = 0;
    for(int rootIndex = 0; rootIndex < m_pModel->rowCount(); rootIndex++)
    {
        auto rootItem = m_pModel->item(rootIndex);
        if(!rootItem) continue;
        QString text = rootItem->text();
        QVariant data = rootItem->data();
        if(rootItem->hasChildren() || rootItem->data().isNull())
        {
            int nCount = 0;
            QString szGroup = rootItem->text();
            set.setValue("Group_" + QString::number(nGroup++), szGroup);
            for(int childIndex = 0; childIndex < rootItem->rowCount(); childIndex++)
            {
                auto childItem = rootItem->child(childIndex);
                if(!childItem) continue;
                set.setValue(szGroup + "/Name_" + QString::number(nCount), childItem->text());
                set.setValue(szGroup + "/File_" + QString::number(nCount), childItem->data());
                nCount++;
            }
            set.setValue(szGroup + "/" + "count", nCount);
        } else {
            set.setValue("Name_" + QString::number(nRootCount), rootItem->text());
            set.setValue("File_" + QString::number(nRootCount), rootItem->data());
            nRootCount++;
        }
    }
    set.setValue("RootCount", nRootCount);
    set.setValue("GroupCount", nGroup);
    return nRet;
}

int CFavoriteView::AddFavorite(const QString& szName, const QString &szFile)
{
    if(!m_pModel) return -1;
    QStandardItem* pItem = nullptr;
    QString szGroup;
    auto indexes = selectionModel()->selectedIndexes();
    if(indexes.isEmpty())
    {
        auto it = m_pModel->findItems(szName, Qt::MatchFixedString);
        if(it.isEmpty())
        {
            pItem = new QStandardItem(szName);
            pItem->setIcon(QIcon::fromTheme("network-wired"));
            m_pModel->appendRow(pItem);
        } else {
            QList<QStandardItem*>::iterator i;
            for(i = it.begin(); i != it.end(); i++)
            {
                if((*i)->data() == szFile)
                    break;
            }
            if(it.end() != i)
                return 1;
            pItem = new QStandardItem(szName);
            pItem->setIcon(QIcon::fromTheme("network-wired"));
            m_pModel->appendRow(pItem);
        }
    } else {
        auto itemGroup = m_pModel->itemFromIndex(indexes[0]);
        if(itemGroup->data().isValid())
        {
            auto it = m_pModel->findItems(szName, Qt::MatchFixedString);
            if(it.isEmpty())
            {
                pItem = new QStandardItem(szName);
                pItem->setIcon(QIcon::fromTheme("network-wired"));
                m_pModel->appendRow(pItem);
            }
        } else {
            szGroup = itemGroup->text();
            auto lstGroup = m_pModel->findItems(szGroup, Qt::MatchFixedString);
            if(lstGroup.isEmpty())
            {
                QStandardItem* pGroup = new QStandardItem(szGroup);
                m_pModel->appendRow(pGroup);
                pItem = new QStandardItem(szName);
                pItem->setIcon(QIcon::fromTheme("network-wired"));
                pGroup->appendRow(pItem);
            } else {
                if(lstGroup[0]->data().isValid()) return 2;
                for(int i = 0; i < lstGroup[0]->rowCount(); i++)
                {
                    auto item = lstGroup[0]->child(i);
                    if(item->text() == szName)
                        return -3;
                }
                pItem = new QStandardItem(szName);
                pItem->setIcon(QIcon::fromTheme("network-wired"));
                lstGroup[0]->appendRow(pItem);
            }
        }
    }
    if(pItem)
        pItem->setData(szFile);
    return 0;
}

void CFavoriteView::slotFavrtieClicked(const QModelIndex &index)
{
}

void CFavoriteView::slotFavortiedoubleClicked(const QModelIndex &index)
{
    auto item = m_pModel->itemFromIndex(index);
    if(!item) return;
    if(editTriggers() != QTreeView::NoEditTriggers)
        return;
    QString szFile = item->data().toString();
    if(!szFile.isEmpty())
        emit sigConnect(szFile, false);
}

void CFavoriteView::slotDoubleEditNode(bool bEdit)
{
    if(bEdit)
        setEditTriggers(QTreeView::DoubleClicked);
    else
        setEditTriggers(QTreeView::NoEditTriggers);
}

void CFavoriteView::slotCustomContextMenu(const QPoint &pos)
{
    auto index = indexAt(pos);
    QMenu menu(this);
    auto item = m_pModel->itemFromIndex(index);
    if(item)
    {
        if(item->data().isValid()) {
            menu.addAction(tr("Connect"), this, SLOT(slotConnect()));
            menu.addAction(tr("Open settings and connect"), this, SLOT(slotOpenConnect()));
            menu.addSeparator();
            menu.addAction(tr("Delete"), this, SLOT(slotDelete()));
        } else {
            menu.addAction(tr("New group"), this, SLOT(slotNewGroup()));
            menu.addAction(tr("Delete"), this, SLOT(slotDelete()));
        }
    } else
        menu.addAction(tr("New group"), this, SLOT(slotNewGroup()));
        
    menu.exec(mapToGlobal(pos));
}

void CFavoriteView::slotConnect()
{
    auto lstIndex = selectionModel()->selectedIndexes();
    foreach(auto index, lstIndex)
    {
        slotFavortiedoubleClicked(index);
    }
}

void CFavoriteView::slotOpenConnect()
{
    auto lstIndex = selectionModel()->selectedIndexes();
    foreach(auto index, lstIndex)
    {
        auto item = m_pModel->itemFromIndex(index);
        QString szFile = item->data().toString();
        if(!szFile.isEmpty())
            emit sigConnect(szFile, true);
    }
}

void CFavoriteView::slotDelete()
{
    auto lstIndex = selectionModel()->selectedIndexes();
    foreach(auto index, lstIndex)
        m_pModel->removeRow(index.row(), index.parent());
}

void CFavoriteView::slotNewGroup()
{
    QString szGroup = QInputDialog::getText(this, tr("Input"), tr("Input group name"));
    if(szGroup.isEmpty()) return;
    auto lstItem = m_pModel->findItems(szGroup);
    if(!lstItem.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("The group [%1] is existed").arg(szGroup));
        return;
    }

    QStandardItem* pGroup = new QStandardItem(szGroup);
    pGroup->setIcon(QIcon::fromTheme("network-workgroup"));
    m_pModel->appendRow(pGroup);
}

void CFavoriteView::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug(FavoriteLogger) << "dragEnterEvent";
    const CFavoriteMimeData* pData = qobject_cast<const CFavoriteMimeData*>(event->mimeData());
    if (pData)
    {
        qDebug(FavoriteLogger) << "dragEnterEvent acceptProposedAction";
        event->acceptProposedAction();
    }
}

void CFavoriteView::dragMoveEvent(QDragMoveEvent *event)
{
}

void CFavoriteView::dropEvent(QDropEvent *event)
{
    qDebug(FavoriteLogger) << "dropEvent";
    const CFavoriteMimeData *pData = qobject_cast<const CFavoriteMimeData*>(event->mimeData());
    if(!pData) return;
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
                qDebug(FavoriteLogger) << "dropEvent:" << item->text();
                
                auto newItem = NewItem(i);
                item->appendRow(newItem);
                if(event->dropAction() == Qt::MoveAction)
                    pModel->removeRow(i.row(), i.parent());
            } 
        } else
            qWarning(FavoriteLogger) << "Don't group node. the data:" << item->data();
    }else{
        foreach(auto i, pData->m_Items)
        {
            pModel->appendRow(NewItem(i));
            if(event->dropAction() == Qt::MoveAction)
                pModel->removeRow(i.row(), i.parent());
        }
    }

    event->accept();
}

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
    qDebug(FavoriteLogger) << "mouseMoveEvent";
    do{
        if (!(event->buttons() & Qt::LeftButton))
            break;
        if ((event->pos() - m_DragStartPosition).manhattanLength()
                < QApplication::startDragDistance())
            break;
        qDebug(FavoriteLogger) << "mouseMoveEvent drag";
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
