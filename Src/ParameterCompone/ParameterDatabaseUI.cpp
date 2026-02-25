// Author: Kang Lin <kl222@126.com>

#include <QSqlDatabase>
#include <QLoggingCategory>
#include <QSet>
#include <QFileDialog>
#include <QWhatsThisClickedEvent>
#include <QDesktopServices>

#include "RabbitCommonDir.h"
#include "ParameterNet.h"
#include "ParameterUser.h"
#include "ParameterDatabaseUI.h"
#include "ui_ParameterDatabaseUI.h"

static Q_LOGGING_CATEGORY(log, "Para.Database.UI")
CParameterDatabaseUI::CParameterDatabaseUI(QWidget *parent)
    : CParameterUI(parent)
    , ui(new Ui::CParameterDatabaseUI)
    , m_pPara(nullptr)
{
    bool check = false;
    ui->setupUi(this);

    setWindowTitle(tr("Database"));

    ui->leDatabaseName->installEventFilter(this);
    ui->pbBrowser->setVisible(false);

    QStringList drivers = QSqlDatabase::drivers();
    qDebug(log) << "Database:" << drivers;
    QSet<QString> supportDrivers = CParameterDatabase::GetSupportDatabase();
    QSet<QString> drv(drivers.begin(), drivers.end());
    QSet<QString> interDb = drv.intersect(supportDrivers);
    QStringList lstDb(interDb.begin(), interDb.end());
    ui->cbType->addItems(lstDb);
    slotTypeCurrentTextChanged(ui->cbType->currentText());
    check = connect(ui->cbType, &QComboBox::currentTextChanged,
            this, &CParameterDatabaseUI::slotTypeCurrentTextChanged);
    Q_ASSERT(check);
}

CParameterDatabaseUI::~CParameterDatabaseUI()
{
    delete ui;
}

int CParameterDatabaseUI::SetParameter(CParameter *pParameter)
{
    m_pPara = qobject_cast<CParameterDatabase*>(pParameter);
    if(!m_pPara) return -1;

    ui->wNet->SetParameter(&m_pPara->m_Net);

    ui->cbType->setCurrentText(m_pPara->GetType());
    ui->leDatabaseName->setText(m_pPara->GetDatabaseName());
    ui->leOptions->setText(m_pPara->GetOptions());

    return 0;
}

int CParameterDatabaseUI::Accept()
{
    ui->wNet->Accept();
    m_pPara->SetType(ui->cbType->currentText());

    m_pPara->SetDatabaseName(ui->leDatabaseName->text());
    m_pPara->SetOptions(ui->leOptions->text());

    return 0;
}

void CParameterDatabaseUI::slotTypeCurrentTextChanged(const QString &text)
{
    QString szMsg;
    QString szHelp;
    szMsg = tr("Set the database name");

    bool bBrowser = false;
    bool bNet = true;
    if("QSQLITE" == text) {
        szMsg = tr("Set the sqlite database file. If is empty, then use default database file.");
        bBrowser = true;
        bNet = false;
    } else if("QODBC" == text) {
        szMsg = tr("The name can either be a DSN, a DSN filename (in which case the file must have a .dsn extension), or a connection string.");
        szHelp = szMsg + " <a href=https://github.com/KangLin/RabbitRemoteControl/wiki/UserManual_zh_CN#%E5%9C%A8-windows-%E4%B8%8B%E4%BD%BF%E7%94%A8-odbc-%E8%AE%BF%E9%97%AE-mysql-%E6%95%B0%E6%8D%AE%E5%BA%93>" + tr("Configure database") + "</a>";
        bNet = false;
    } else if("QMYSQL" == text) {
        bNet = true;
    }

    if(szHelp.isEmpty())
        szHelp = szMsg + " <a href=https://github.com/KangLin/RabbitRemoteControl/wiki/UserManual_zh_CN#%E9%85%8D%E7%BD%AE%E6%95%B0%E6%8D%AE%E5%BA%93>" + tr("Configure database") + "</a>";
    ui->leDatabaseName->setWhatsThis(szHelp);
    ui->leDatabaseName->setPlaceholderText(szMsg);
    ui->leDatabaseName->setToolTip(szMsg);
    ui->pbBrowser->setVisible(bBrowser);
    ui->wNet->setVisible(bNet);
}

void CParameterDatabaseUI::on_pbBrowser_clicked()
{
    QString szTitle;
    QString szFilter;
    szTitle = tr("Database file");
    szFilter = tr("Database file (*.db);; All files (*.*)");
    QString szFile = QFileDialog::getSaveFileName(
        nullptr, szTitle,
        RabbitCommon::CDir::Instance()->GetDirUserDatabase(),
        szFilter);
    if(szFile.isEmpty()) return;
    ui->leDatabaseName->setText(szFile);
}

bool CParameterDatabaseUI::eventFilter(QObject *watched, QEvent *event)
{
    if(ui->leDatabaseName == watched) {
        if(event->type() == QEvent::WhatsThisClicked) {
            QWhatsThisClickedEvent* e = (QWhatsThisClickedEvent*)event;
            if(e) {
                qDebug(log) << e->href();
                return QDesktopServices::openUrl(e->href());
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}
