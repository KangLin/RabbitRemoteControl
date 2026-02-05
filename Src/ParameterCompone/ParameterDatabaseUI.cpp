// Author: Kang Lin <kl222@126.com>

#include <QSqlDatabase>
#include <QLoggingCategory>
#include <QSet>
#include <QFileDialog>

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

    ui->pbBrowser->setVisible(false);

    QStringList drivers = QSqlDatabase::drivers();
    qDebug(log) << "Database:" << drivers;
    QSet<QString> supportDrivers;
    supportDrivers << "QSQLITE" << "QMYSQL" << "QODBC";
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
    szMsg = tr("Set the database name");
    ui->wNet->setVisible(true);

    bool bBrowser = false;
    if("QSQLITE" == text) {
        szMsg = tr("Set the sqlite database file");
        bBrowser = true;
        ui->wNet->setVisible(false);
    } else if("QODBC" == text) {
        szMsg = tr("The name can either be a DSN, a DSN filename (in which case the file must have a .dsn extension), or a connection string.");
    } else if("QMYSQL" == text) {
        ;
    }

    ui->leDatabaseName->setPlaceholderText(szMsg);
    ui->leDatabaseName->setToolTip(szMsg);
    ui->pbBrowser->setVisible(bBrowser);
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

