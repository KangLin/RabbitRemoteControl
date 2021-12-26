#include "DockWdgFavorite.h"
#include "ui_DockWdgFavorite.h"

#include "RabbitCommonDir.h"
#include <QStandardItem>
#include <QSettings>

CDockWdgFavorite::CDockWdgFavorite(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::CDockWdgFavorite)
{
    ui->setupUi(this);

    m_szSaveFile = RabbitCommon::CDir::Instance()->GetDirUserData()
            + QDir::separator() + "Favorite.ini";
    ui->tvFavorite->header()->hide();
    ui->tvFavorite->setEditTriggers(QTreeView::NoEditTriggers);
    
    m_pModel = new QStandardItemModel(ui->tvFavorite);
    ui->tvFavorite->setModel(m_pModel);
    
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
        if(rootItem->hasChildren())
        {
            int nCount = 0;
            QString szGroup = rootItem->text();
            set.setValue("Group_" + QString::number(nGroup++), szGroup);
            for(int childIndex = 0; childIndex < rootItem->rowCount(); childIndex++)
            {
                auto childItem = rootItem->child(childIndex);
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

void CDockWdgFavorite::on_tvFavorite_clicked(const QModelIndex &index)
{
    auto item = m_pModel->itemFromIndex(index);
    if(!item) return;
    QString szFile = item->data().toString();
    if(!szFile.isEmpty())
        emit sigConnect(szFile);
}

void CDockWdgFavorite::on_tvFavorite_doubleClicked(const QModelIndex &index)
{
}
