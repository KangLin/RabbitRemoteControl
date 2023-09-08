#include "DlgDesktopSize.h"
#include "ui_DlgDesktopSize.h"
#include <QMessageBox>

CDlgDesktopSize::CDlgDesktopSize(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgDesktopSize)
{
    ui->setupUi(this);
    ui->lbError->hide();
    m_pModel = new QStandardItemModel(this);
    if(m_pModel)
    {
        ui->lstSizes->setModel(m_pModel);
        bool check = connect(m_pModel, SIGNAL(itemChanged(QStandardItem*)),
                             this, SLOT(slotItemChanged(QStandardItem*)));
        Q_ASSERT(check);
    }
}

CDlgDesktopSize::~CDlgDesktopSize()
{
    delete ui;
}

void CDlgDesktopSize::SetDesktopSizes(const QStringList& lstSize)
{
    m_pModel->clear();
    foreach (auto s, lstSize) {
        QStandardItem *item = new QStandardItem(s);
        m_pModel->appendRow(item);
    }
    m_lstDesktopSize = lstSize;
}

const QStringList CDlgDesktopSize::GetDesktopSize()
{
    m_lstDesktopSize.clear();
    for(int i = 0; i < m_pModel->rowCount(); i++)
    {
        QModelIndex index = m_pModel->index(i, 0);
        QString s = m_pModel->data(index).toString();
        if(checkFormat(s))
            m_lstDesktopSize << s;
    }
    return m_lstDesktopSize;
}

void CDlgDesktopSize::on_pbAdd_clicked()
{
    QStandardItem *item = new QStandardItem("width×height");
    m_pModel->appendRow(item);
    QModelIndex index = m_pModel->index(m_pModel->rowCount() - 1, 0);
    ui->lstSizes->setCurrentIndex(index);
    ui->lstSizes->edit(index);
}

void CDlgDesktopSize::on_pbRemove_clicked()
{
    QModelIndex index = ui->lstSizes->currentIndex();
    m_pModel->removeRow(index.row());
}

void CDlgDesktopSize::slotItemChanged(QStandardItem *item)
{
    bool ok = false;
    QString s = item->data(Qt::DisplayRole).toString();
    if(checkFormat(s))
        ui->lbError->hide();
    else
        ui->lbError->show();
}

bool CDlgDesktopSize::checkFormat(QString size)
{
    int index = size.indexOf("×");
    if(-1 == index)
    {
        return false;
    }
    bool ok = false;
    int w = size.left(index).toInt(&ok);
    Q_UNUSED(w);
    if(ok)
    {
        int h = size.right(size.length() - index - 1).toInt(&ok);
        Q_UNUSED(h);
    }
    return ok;
}
