#include "FavoriteView.h"

#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QDebug>
#include <QApplication>
#include <QHeaderView>
#include <QStandardItemModel>
#include "FavoriteMimeData.h"

CFavoriteView::CFavoriteView(QWidget *parent) : QTreeView()
{
    setFocusPolicy(Qt::NoFocus);
    header()->hide();
    setEditTriggers(QTreeView::NoEditTriggers);
    setAcceptDrops(true);
}

void CFavoriteView::dragEnterEvent(QDragEnterEvent *event)
{
    const CFavoriteMimeData* pData = qobject_cast<const CFavoriteMimeData*>(event->mimeData());
    if (pData)
        event->acceptProposedAction();
}

void CFavoriteView::dropEvent(QDropEvent *event)
{
    const CFavoriteMimeData *pData = qobject_cast<const CFavoriteMimeData*>(event->mimeData());
    if(!pData) return;
    QStandardItemModel* pModel = dynamic_cast<QStandardItemModel*>(model());
    
    auto index = indexAt(event->pos());
    if(index.isValid())
    {
        auto item = pModel->itemFromIndex(index);
        foreach(auto index, pData->m_Items)
            item->appendRow(pModel->itemFromIndex(index));
    }else{
        foreach(auto index, pData->m_Items)
            pModel->appendRow(pModel->itemFromIndex(index));
    }
    
    qDebug() << "dropEvent" << event;
    event->accept();
}

//void CFavoriteView::mousePressEvent(QMouseEvent *event)
//{
//    if (event->button() == Qt::LeftButton)
//        m_DragStartPosition = event->pos();
//}

void CFavoriteView::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;
//    if ((event->pos() - m_DragStartPosition).manhattanLength()
//            < QApplication::startDragDistance())
//        return;
    
    QDrag *drag = new QDrag(this);
    CFavoriteMimeData *pData = new CFavoriteMimeData();
    pData->m_Items = this->selectionModel()->selectedIndexes();
    drag->setMimeData(pData);
    
    Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction);
}
