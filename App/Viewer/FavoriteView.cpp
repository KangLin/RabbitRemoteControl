#include "FavoriteView.h"

#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QDebug>
#include <QApplication>
#include <QHeaderView>
#include <QStandardItemModel>
#include "FavoriteMimeData.h"
#include "RabbitCommonLog.h"

CFavoriteView::CFavoriteView(QWidget *parent) : QTreeView()
{
    setFocusPolicy(Qt::NoFocus);
    header()->hide();
    setEditTriggers(QTreeView::NoEditTriggers);
    setAcceptDrops(true);
}

void CFavoriteView::dragEnterEvent(QDragEnterEvent *event)
{
    LOG_MODEL_DEBUG("CFavoriteView", "dragEnterEvent");
    const CFavoriteMimeData* pData = qobject_cast<const CFavoriteMimeData*>(event->mimeData());
    if (pData)
    {
        LOG_MODEL_DEBUG("CFavoriteView", "dragEnterEvent acceptProposedAction");
        event->acceptProposedAction();
    }
}

void CFavoriteView::dragMoveEvent(QDragMoveEvent *event)
{
}

void CFavoriteView::dropEvent(QDropEvent *event)
{
    LOG_MODEL_DEBUG("CFavoriteView", "dropEvent");
    const CFavoriteMimeData *pData = qobject_cast<const CFavoriteMimeData*>(event->mimeData());
    if(!pData) return;
    QStandardItemModel* pModel = dynamic_cast<QStandardItemModel*>(model());
    if(!pModel) return;
    auto index = indexAt(event->pos());
    if(index.isValid())
    {
        auto item = pModel->itemFromIndex(index);
        foreach(auto index, pData->m_Items)
        {
            LOG_MODEL_DEBUG("CFavoriteView", "dropEvent: %s",
                            item->text().toStdString().c_str());
            if(item->data().isNull())
            {
                auto newItem = NewItem(index);
                item->appendRow(newItem);
                if(event->dropAction() == Qt::MoveAction)
                    pModel->removeRow(index.row(), index.parent());
            } else
                LOG_MODEL_WARNING("CFavoriteView", "Don't group node. the data:%s",
                                  item->data().toString().toStdString().c_str());
        }
    }else{
        foreach(auto index, pData->m_Items)
        {
            pModel->appendRow(NewItem(index));
            if(event->dropAction() == Qt::MoveAction)
                pModel->removeRow(index.row(), index.parent());
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
    LOG_MODEL_DEBUG("CFavoriteView", "mouseMoveEvent");
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - m_DragStartPosition).manhattanLength()
            < QApplication::startDragDistance())
        return;
    LOG_MODEL_DEBUG("CFavoriteView", "mouseMoveEvent drag");
    QDrag *drag = new QDrag(this);
    CFavoriteMimeData *pData = new CFavoriteMimeData();
    pData->m_Items = this->selectionModel()->selectedIndexes();
    drag->setMimeData(pData);

    Qt::DropAction dropAction = Qt::MoveAction;
    if(event->modifiers() & Qt::ControlModifier)
        dropAction = Qt::CopyAction;
    drag->exec(dropAction);
}
