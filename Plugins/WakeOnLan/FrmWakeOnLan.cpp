#include <QLoggingCategory>
#include <QMessageBox>

#include "FrmWakeOnLan.h"
#include "ui_FrmWakeOnLan.h"

static Q_LOGGING_CATEGORY(log, "WakeOnLan.CFrmWakeOnLan")
CFrmWakeOnLan::CFrmWakeOnLan(CWakeOnLanModel *pModel, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CFrmWakeOnLan)
{
    bool check = false;
    ui->setupUi(this);
    ui->tableView->setModel(pModel);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableView->installEventFilter(this);
    check = connect(ui->tableView,
                    SIGNAL(customContextMenuRequested(const QPoint&)),
                    this, SIGNAL(customContextMenuRequested(const QPoint&)));
    Q_ASSERT(check);
    check = connect(pModel, SIGNAL(modelReset()),
                    ui->tableView, SLOT(resizeColumnsToContents()));
    Q_ASSERT(check);
    setWindowTitle(tr("Wake on lan"));
    setWindowIcon(QIcon::fromTheme("lan"));

    // Sort
    check = connect(
        ui->tableView->horizontalHeader(),
        &QHeaderView::sectionClicked,
        this, [&](int c){
            if(0 == c || 1 == c) {
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
                        if(0 == logicalIndex || 1 == logicalIndex)
                            ui->tableView->model()->sort(logicalIndex, order);
                    });
    Q_ASSERT(check);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    //必须在 setModel 后,才能应用
    /*第二个参数可以为：
    QHeaderView::Interactive     ：0 用户可设置，也可被程序设置成默认大小
    QHeaderView::Fixed           ：2 用户不可更改列宽
    QHeaderView::Stretch         ：1 根据空间，自动改变列宽，用户与程序不能改变列宽
    QHeaderView::ResizeToContents：3 根据内容改变列宽，用户与程序不能改变列宽
    */
    ui->tableView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Interactive);
    //以下设置列宽函数必须要数据加载完成后使用,才能应用
    //See: https://blog.csdn.net/qq_40450386/article/details/86083759
    //ui->tableView->resizeColumnsToContents(); //设置所有列宽度自适应内容
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
    {
        QMessageBox::information(
            nullptr,
            tr("Information"),
            tr("Please select a item"));
        return;
    }

    ui->tableView->model()->removeRow(index.row());
}

QModelIndex CFrmWakeOnLan::GetCurrentIndex()
{
    return ui->tableView->currentIndex();
}

QModelIndexList CFrmWakeOnLan::GetSelect()
{
    return ui->tableView->selectionModel()->selectedRows();
}

bool CFrmWakeOnLan::eventFilter(QObject *watched, QEvent *event)
{
    if(ui->tableView == watched)
    {
        switch(event->type()){
        case QEvent::FocusIn:
        {
            //qDebug(log) << Q_FUNC_INFO << event;
            emit sigViewerFocusIn(this);
            return false;
        }
        default:
            return false;
        }
    }
    return false;
}
