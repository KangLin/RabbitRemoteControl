// Author: Kang Lin <kl222@126.com>

#include <QMessageBox>
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
#include "ParameterGlobal.h"
#include "Database.h"

static Q_LOGGING_CATEGORY(log, "Para.Database.UI")
CParameterDatabaseUI::CParameterDatabaseUI(QWidget *parent)
    : CParameterUI(parent)
    , ui(new Ui::CParameterDatabaseUI)
    , m_pParaDB(nullptr)
    , m_pParaGlobal(nullptr)
    , m_Net(nullptr)
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
    m_pParaGlobal = qobject_cast<CParameterGlobal*>(pParameter);
    if(!m_pParaGlobal) return -1;

    m_pParaDB = &m_pParaGlobal->m_Database;
    if(!m_pParaDB) return -1;
    
    m_Net = m_pParaDB->m_Net;
    ui->wNet->SetParameter(&m_Net);

    ui->cbType->setCurrentText(m_pParaDB->GetType());
    ui->leDatabaseName->setText(m_pParaDB->GetDatabaseName());
    ui->leOptions->setText(m_pParaDB->GetOptions());
    
    switch(m_pParaGlobal->GetSaveSettingsType()) {
    case CParameterGlobal::SaveSettingsType::Local:
        ui->rbSaveSettingsToLocal->setChecked(true);
        break;
    case CParameterGlobal::SaveSettingsType::Database:
        ui->rbSaveSettingsToDatabase->setChecked(true);
        break;
    }
    ui->lbDatabaseWarn->setVisible(
        ui->rbSaveSettingsToDatabase->isChecked()
        && ui->cbType->currentText() != "QSQLITE");

    return 0;
}

bool CParameterDatabaseUI::CheckValidity(bool validity)
{
    if(!validity) return true;

    if(ui->cbType->currentText() == "QMYSQL") {
        if(!ui->wNet->CheckValidity(validity)) {
            return false;
        }
    }

    if(ui->rbSaveSettingsToLocal->isChecked()) {
        if(ui->cbType->currentText() != "QSQLITE") {
            QString szErr = tr("Save operate settings to:") + " \"" + tr("Local") + "\". " + tr("but the database is not set local database \"QSQLITE\".");
            szErr += "\n\n";
            szErr += tr("Please modify database \"Type\" to \"QSQLITE\"") + " ";
            szErr += tr("or modify ") + tr("Save operate settings to:") + " " + tr("Database");
            QMessageBox::critical(this, tr("Error"), szErr);
            qCritical(log) << szErr;
            ui->rbSaveSettingsToDatabase->setFocus();
            return false;
        }
    }

    return true;
}

int CParameterDatabaseUI::Accept()
{
    ui->wNet->Accept();
    m_pParaDB->m_Net = m_Net;
    m_pParaDB->SetType(ui->cbType->currentText());

    m_pParaDB->SetDatabaseName(ui->leDatabaseName->text());
    m_pParaDB->SetOptions(ui->leOptions->text());
    
    if(ui->rbSaveSettingsToLocal->isChecked()) {
        m_pParaGlobal->SetSaveSettingsType(CParameterGlobal::SaveSettingsType::Local);
    } else if(ui->rbSaveSettingsToDatabase->isChecked()) {
        m_pParaGlobal->SetSaveSettingsType(CParameterGlobal::SaveSettingsType::Database);
    }
    
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
        if(m_pParaDB) {
            auto &net = m_pParaDB->m_Net;
            if(0 == net.GetPort()) {
                net.SetPort(3306);
                ui->wNet->SetParameter(&net);
            }
        }
    }

    if(szHelp.isEmpty())
        szHelp = szMsg + " <a href=https://github.com/KangLin/RabbitRemoteControl/wiki/UserManual_zh_CN#%E9%85%8D%E7%BD%AE%E6%95%B0%E6%8D%AE%E5%BA%93>" + tr("Configure database") + "</a>";
    ui->leDatabaseName->setWhatsThis(szHelp);
    ui->leDatabaseName->setPlaceholderText(szMsg);
    ui->leDatabaseName->setToolTip(szMsg);
    ui->pbBrowser->setVisible(bBrowser);
    ui->wNet->setVisible(bNet);

    ui->lbDatabaseWarn->setVisible(
        ui->rbSaveSettingsToDatabase->isChecked()
        && ui->cbType->currentText() != "QSQLITE");
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

void CParameterDatabaseUI::on_rbSaveSettingsToDatabase_toggled(bool checked)
{
    ui->lbDatabaseWarn->setVisible(checked && ui->cbType->currentText() != "QSQLITE");
}

void CParameterDatabaseUI::on_pbTest_clicked()
{
    CParameterDatabase dbPara;
    dbPara.SetType(ui->cbType->currentText());
    dbPara.SetDatabaseName(ui->leDatabaseName->text());
    dbPara.SetOptions(ui->leOptions->text());
    if(!CheckValidity(true))
        return;
    ui->wNet->Accept();
    dbPara.m_Net = m_Net;
    CDatabase db;
    bool bRet = db.OpenDatabase(&dbPara);
    if(bRet) {
        bRet = db.IsOpen();
    }
    if(bRet)
        QMessageBox::information(this, tr("Test"), tr("Test is successfully!"));
    else
        QMessageBox::critical(this, tr("Test"),
                              tr("Test is failed!") + "\n\n"
                                  + db.GetError());
    db.CloseDatabase();
}

