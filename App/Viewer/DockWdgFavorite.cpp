#include "DockWdgFavorite.h"
#include "ui_DockWdgFavorite.h"

#include "RabbitCommonDir.h"
#include <QStandardItem>
#include <QSettings>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QMessageBox>

CDockWdgFavorite::CDockWdgFavorite(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::CDockWdgFavorite)
{
    ui->setupUi(this);
    
    m_szSaveFile = RabbitCommon::CDir::Instance()->GetDirUserData()
            + QDir::separator() + "Favorite.ini";
    
    m_pView = new CFavoriteView(this);
    this->setWidget(m_pView);
    m_pModel = new QStandardItemModel(m_pView);
    m_pView->setModel(m_pModel);
    
    m_pView->setContextMenuPolicy(Qt::CustomContextMenu);
    bool check = connect(m_pView,
                         SIGNAL(customContextMenuRequested(const QPoint &)),
                         this, SLOT(slotCustomContextMenu(const QPoint &)));
    Q_ASSERT(check);
    check = connect(m_pView, SIGNAL(clicked(const QModelIndex&)),
                    this, SLOT(slotFavrtieClicked(const QModelIndex&)));
    Q_ASSERT(check);
    check = connect(m_pView, SIGNAL(doubleClicked(const QModelIndex&)),
                    this, SLOT(slotFavortiedoubleClicked(const QModelIndex&)));
    Q_ASSERT(check);
    
    QSettings set(m_szSaveFile, QSettings::IniFormat);
    int nRootCount = 0;
    nRootCount = set.value("RootCount").toInt();
    for(int i = 0; i < nRootCount; i++)
    {
        QString name = set.value("Name_" + QString::number(i)).toString();
        QString file = set.value("File_" + QString::number(i)).toString();
        QStandardItem* item = new QStandardItem(name);
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
        m_pModel->appendRow(pGroup);
        int n = set.value(szGroup + "/" + "count").toInt();
        for(int i = 0; i < n; i++)
        {
            QString name = set.value(szGroup + "/Name_" + QString::number(i)).toString();
            QString file = set.value(szGroup + "/File_" + QString::number(i)).toString();
            QStandardItem* item = new QStandardItem(name);
            if(item)
            {
                item->setData(file);
                pGroup->appendRow(item);
            }
        }
    }
}

CDockWdgFavorite::~CDockWdgFavorite()
{
    Save();
    delete ui;
}

int CDockWdgFavorite::Save()
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
        if(rootItem->hasChildren())
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

int CDockWdgFavorite::AddFavorite(const QString& szName, const QString &szFile, QString szGroup)
{
    if(!m_pModel) return -1;
    QStandardItem* pItem = nullptr;
    if(szGroup.isEmpty())
    {
        auto it = m_pModel->findItems(szName, Qt::MatchFixedString);
        if(it.isEmpty())
        {
            pItem = new QStandardItem(szName);
            m_pModel->appendRow(pItem);
        } else {
            QList<QStandardItem*>::iterator i;
            for(i = it.begin(); i != it.end(); i++)
            {
                if((*i)->data() == szFile)
                    break;
            }
            if(it.end() != i)
                return 0;
            pItem = new QStandardItem(szName);
            m_pModel->appendRow(pItem);
        }
    } else {
        auto lstGroup = m_pModel->findItems(szGroup, Qt::MatchFixedString);
        if(lstGroup.isEmpty())
        {
            QStandardItem* pGroup = new QStandardItem(szGroup);
            m_pModel->appendRow(pGroup);
            pItem = new QStandardItem(szName);
            pGroup->appendRow(pItem);
        } else {
            for(int i = 0; i < lstGroup[0]->rowCount(); i++)
            {
                auto item = lstGroup[0]->child(i);
                if(item->text() == szName && item->data() == szFile)
                    return 0;
            }
            pItem = new QStandardItem(szName);
            lstGroup[0]->appendRow(pItem);
        }
    }
    if(pItem)
        pItem->setData(szFile);
    return 0;
}

void CDockWdgFavorite::slotFavrtieClicked(const QModelIndex &index)
{
}

void CDockWdgFavorite::slotFavortiedoubleClicked(const QModelIndex &index)
{
    auto item = m_pModel->itemFromIndex(index);
    if(!item) return;
    QString szFile = item->data().toString();
    if(!szFile.isEmpty())
        emit sigConnect(szFile, false);
}

void CDockWdgFavorite::slotCustomContextMenu(const QPoint &pos)
{
    if(!m_pView) return;
    QMenu menu(m_pView);
    menu.addAction(tr("Connect"), this, SLOT(slotConnect()));
    menu.addAction(tr("Open settings and connect"), this, SLOT(slotOpenConnect()));
    menu.addSeparator();
    menu.addAction(tr("New group"), this, SLOT(slotNewGroup()));
    menu.addAction(tr("Delete"), this, SLOT(slotDelete()));
    menu.exec(m_pView->mapToGlobal(pos));
}

void CDockWdgFavorite::slotConnect()
{
    auto lstIndex = m_pView->selectionModel()->selectedIndexes();
    foreach(auto index, lstIndex)
    {
        slotFavortiedoubleClicked(index);
    }
}

void CDockWdgFavorite::slotOpenConnect()
{
    auto lstIndex = m_pView->selectionModel()->selectedIndexes();
    foreach(auto index, lstIndex)
    {
        auto item = m_pModel->itemFromIndex(index);
        QString szFile = item->data().toString();
        if(!szFile.isEmpty())
            emit sigConnect(szFile, true);
    }
}

void CDockWdgFavorite::slotDelete()
{
    auto lstIndex = m_pView->selectionModel()->selectedIndexes();
    foreach(auto index, lstIndex)
        m_pModel->removeRow(index.row());
}

void CDockWdgFavorite::slotNewGroup()
{
    QString szGroup = QInputDialog::getText(this, tr("Input"), tr("Input group name"));    
    if(szGroup.isEmpty()) return;
    auto lstItem = m_pModel->findItems(szGroup);
    if(!lstItem.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("The group [%1] is existed").arg(szGroup));
        return;
    }
    
    QStandardItem* pItem = new QStandardItem(szGroup);
    m_pModel->appendRow(pItem);
}
