#include "FrmWakeOnLan.h"
#include "ui_FrmWakeOnLan.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "WakeOnLan.CFrmWakeOnLan")
CFrmWakeOnLan::CFrmWakeOnLan(CWakeOnLanModel *pModel, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CFrmWakeOnLan)
{
    bool check = false;
    ui->setupUi(this);
    ui->tableView->setModel(pModel);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    check = connect(ui->tableView,
                    SIGNAL(customContextMenuRequested(const QPoint&)),
                    this, SIGNAL(customContextMenuRequested(const QPoint&)));
    Q_ASSERT(check);
    setWindowTitle(tr("Wake on lan"));
    setWindowIcon(QIcon::fromTheme("lan"));

    check = connect(
        ui->tableView->horizontalHeader(),
        &QHeaderView::sectionClicked,
        this, [&](int c){
            if(1 == c) {
                ui->tableView->horizontalHeader()->setSortIndicatorShown(true);
            }
            else {
                ui->tableView->horizontalHeader()->setSortIndicatorShown(false);
            }
        });
    Q_ASSERT(check);
    check = connect(ui->tableView->horizontalHeader(),
                    &QHeaderView::sortIndicatorChanged,
                    this, [&](int logicalIndex, Qt::SortOrder order){
                        if(1 == logicalIndex)
                            ui->tableView->model()->sort(1, order);
                    });
    Q_ASSERT(check);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
}

CFrmWakeOnLan::~CFrmWakeOnLan()
{
    qDebug(log) << "CFrmWakeOnLan::~CFrmWakeOnLan()";
    delete ui;
}

void CFrmWakeOnLan::slotRemoveRow()
{
    QModelIndex index = ui->tableView->currentIndex();
    if(!index.isValid())
        return;
    ui->tableView->model()->removeRow(index.row());
}

QModelIndex CFrmWakeOnLan::GetCurrentIndex()
{
    return ui->tableView->currentIndex();
}
