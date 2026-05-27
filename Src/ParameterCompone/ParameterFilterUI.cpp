#include <QInputDialog>
#include <QItemSelectionModel>
#include <QMenu>
#include "ParameterFilterUI.h"
#include "ui_ParameterFilterUI.h"

CParameterFilterUI::CParameterFilterUI(QWidget *parent)
    : CParameterUI(parent)
    , ui(new Ui::CParameterFilterUI)
    , m_pPara(nullptr)
{
    ui->setupUi(this);
    ui->lsFilter->setModel(&m_Model);

    m_szFilteListPrompt = tr("The IP address and the netmask must be separated by a slash (/).") + "\n\n"
                          + tr("ag:") + "\n"
                          + "- 123.123.123.123/n " + tr("where n is any value between 0 and 32") + "\n"
                          + "- 123.123.123.123/255.255.255.255" + "\n"
                          + "- <ipv6-address>/n " + tr("where n is any value between 0 and 128") + "\n\n"
                          + tr("For IP version 4, accepts as well missing trailing components") + "\n"
                          + tr("(i.e., less than 4 octets, like \"192.168.1\"), followed or not by a dot. ") + "\n"
                          + tr("If the netmask is also missing in that case,") + "\n"
                          + tr("it is set to the number of octets actually passed") + "\n"
                          + tr("(in the example above, it would be 24, for 3 octets).") + "\n\n"
                          + tr("Add IP address and the netmask:");

    bool check = connect(ui->lsFilter, SIGNAL(customContextMenuRequested(const QPoint&)),
                    this, SLOT(slotContextMenuRequested(const QPoint&)));
    Q_ASSERT(check);
}

CParameterFilterUI::~CParameterFilterUI()
{
    delete ui;
}

int CParameterFilterUI::SetParameter(CParameter *pParameter)
{
    int nRet = 0;
    m_pPara = qobject_cast<CParameterFilter*>(pParameter);
    if(!m_pPara) return -1;
    m_pPara->OnProcess([&](const QString & key)->int {
        m_Model.appendRow(new QStandardItem(key));
        return 0;
    });
    return nRet;
}

int CParameterFilterUI::Accept()
{
    int nRet = 0;
    if(!m_pPara) return -1;

    m_pPara->Clear();
    QStringList lstBlack;
    for (int i = 0; i < m_Model.rowCount(); ++i) {
        m_pPara->AddKey(m_Model.item(i)->text());
    }

    return nRet;
}

void CParameterFilterUI::slotContextMenuRequested(const QPoint &pos)
{
    QMenu m;
    QItemSelectionModel* pSelect = ui->lsFilter->selectionModel();
    QModelIndexList lstIndex = pSelect->selectedRows();
    m.addAction(tr("Add"), this, SLOT(on_pbAdd_clicked()));
    if(!lstIndex.isEmpty()) {
        m.addAction(tr("Delete"), this, SLOT(on_pbDelete_clicked()));
    }

    QPoint p = ui->lsFilter->mapToGlobal(pos);
    m.exec(p);
}

void CParameterFilterUI::on_pbAdd_clicked()
{
    QString szIp = QInputDialog::getText(this, tr("Add"), m_szFilteListPrompt);
    QStandardItem* item = new QStandardItem(szIp);
    m_Model.appendRow(item);
}

void CParameterFilterUI::on_pbDelete_clicked()
{
    QItemSelectionModel* pSelect = ui->lsFilter->selectionModel();
    if(!pSelect) return;
    QModelIndexList lstIndex = pSelect->selectedRows();
    foreach(auto idx, lstIndex) {
        m_Model.removeRow(idx.row());
    }
}
