// Author: Kang Lin <kl222@126.com>

#include "DlgSettingsTerminal.h"
#include "ui_DlgSettingsTerminal.h"
#include <QMessageBox>
#include <QLoggingCategory>
#include <qtermwidget.h>
#include <QFileInfo>
#include <QFileDialog>
#include <QUrl>

static Q_LOGGING_CATEGORY(log, "Plugin.Terminal.DlgSettings")

CDlgSettingsTerminal::CDlgSettingsTerminal(CParameterTerminalBase *pPara, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSettingsTerminal),
    m_pPara(pPara)
{
    ui->setupUi(this);
    ui->teHelp->hide();
    
#if defined(Q_OS_UNIX)    
    AddShell("/bin/sh");
    AddShell("/bin/bash");
    AddShell("/bin/dash");
    AddShell(qgetenv("SHELL"));
#elif defined(Q_OS_WIN)
    AddShell("C:\\Windows\\System32\\cmd.exe");
    AddShell("C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe");
    AddShell("C:\\Windows\\System32\\cmd.exe", "msys64");
    AddShell("C:\\Windows\\System32\\cmd.exe", "cygwin64");
    AddShell(qgetenv("ComSpec"));
    QString szHelp;
    szHelp = tr("Help:") + "\n";
    szHelp += tr("- Set bash shell:") + "\n";
    szHelp += tr("  cmd shell with parameters:") + " '/k %BASH_PATH% -l'\n";
    szHelp += tr("  eg:") + "\n";
    szHelp += tr("    msys64 default bash path:") + " 'c:\\msys64\\usr\\bin\\bash.exe'\n";
    szHelp += tr("    So that cmd shell with parameters:") + " '/k c:\\msys64\\usr\\bin\\bash.exe -l'\n";
    szHelp += tr("    cygwin64 default bash path:") + " 'c:\\cygwin64\\bin\\bash.exe'\n";
    szHelp += tr("    So that cmd shell with parameters:") + " '/k c:\\cygwin64\\bin\\bash.exe -l'";
    ui->teHelp->setText(szHelp);
    ui->teHelp->show();
#endif

    QString szShell(m_pPara->GetShell());
    AddShell(szShell);
    if(!m_pPara->GetShellName().isEmpty())
        ui->cbShell->setCurrentText(m_pPara->GetShellName());
    ui->leParameters->setText(m_pPara->GetShellParameters());

    m_pFrmParaAppearnce =
            new CParameterTerminalUI(this);
    if(m_pFrmParaAppearnce) {
        m_pFrmParaAppearnce->SetParameter(&m_pPara->m_Terminal);
        ui->tabWidget->addTab(m_pFrmParaAppearnce,
                          m_pFrmParaAppearnce->windowTitle());
    }

    ui->cbTitleChanged->setChecked(m_pPara->GetEnableTitleChanged());
}

CDlgSettingsTerminal::~CDlgSettingsTerminal()
{
    qDebug(log) << "CDlgSettingsTerminal::~CDlgSettingsTerminal()";
    delete ui;
}

void CDlgSettingsTerminal::on_pbOk_clicked()
{
    if(!ui->cbShell->currentData().isValid())
    {
        ui->tabWidget->setCurrentIndex(0);
        ui->cbShell->setFocus();
        QMessageBox::critical(this, tr("Error"), tr("The shell is empty"));
        return;
    }
    m_pPara->SetShell(ui->cbShell->currentData().toString());
    m_pPara->SetShellName(ui->cbShell->currentText());
    m_pPara->SetShellParameters(ui->leParameters->text());
    if(m_pFrmParaAppearnce)
        m_pFrmParaAppearnce->Accept();

    m_pPara->SetEnableTitleChanged(ui->cbTitleChanged->isChecked());
    accept();
}

void CDlgSettingsTerminal::on_pbCancel_clicked()
{
    this->reject();
}

int CDlgSettingsTerminal::AddShell(QString szShell, const QString &name)
{
    int nIndex = 0;
    QString szName = name;
    QFileInfo fi(szShell);
    if(!fi.exists())
    {
        qCritical(log) << "The shell is not exist:" << szShell;
        return -1;
    }
    if(szName.isEmpty())
        szName = fi.baseName();

    nIndex = ui->cbShell->findText(szName, Qt::MatchFixedString);
    if(-1 == nIndex) {
        ui->cbShell->addItem(szName, szShell);
        nIndex = ui->cbShell->findText(szName, Qt::MatchFixedString);
    }

    ui->cbShell->setItemData(nIndex, szShell, Qt::ToolTipRole);
    ui->cbShell->setItemData(nIndex, szShell, Qt::StatusTipRole);
    ui->cbShell->setCurrentIndex(nIndex);

    return 0;
}

void CDlgSettingsTerminal::on_pbShellBrowse_clicked()
{
    QUrl url = QFileDialog::getOpenFileUrl(
        this, tr("Select shell"),
        QUrl::fromLocalFile(ui->cbShell->currentData().toString()));
    if(url.isEmpty())
        return;
    AddShell(url.toLocalFile());
}

void CDlgSettingsTerminal::on_cbShell_currentIndexChanged(int index)
{
    QString szData, szName;
    szData = ui->cbShell->itemData(index).toString();
    szName = ui->cbShell->itemText(index);
    ui->cbShell->setToolTip(szData);
    ui->cbShell->setStatusTip(szData);
    if("msys64" == szName && ui->leParameters->text().isEmpty()) {
        ui->leParameters->setText("/k c:\\msys64\\usr\\bin\\bash.exe -l");
    }
    if("cygwin64" == szName && ui->leParameters->text().isEmpty()) {
        ui->leParameters->setText("/k c:\\cygwin64\\bin\\bash.exe -l");
    }
}

