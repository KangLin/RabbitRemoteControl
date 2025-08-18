// Author: Kang Lin <kl222@126.com>

#include <QMessageBox>
#include <QLoggingCategory>
#include <qtermwidget.h>
#include <QFileInfo>
#include <QFileDialog>
#include <QUrl>

#include "RabbitCommonTools.h"
#include "DlgSettingsTerminal.h"
#include "ui_DlgSettingsTerminal.h"

static Q_LOGGING_CATEGORY(log, "Plugin.Terminal.DlgSettings")

CDlgSettingsTerminal::CDlgSettingsTerminal(CTerminalParameter *pPara, QWidget *parent) :
    QDialog(parent)
    , ui(new Ui::CDlgSettingsTerminal)
    , m_pPara(pPara)
{
    ui->setupUi(this);
    ui->teHelp->hide();
    ui->pbHelp->hide();

#if defined(Q_OS_UNIX)    
    AddShell("/bin/sh");
    AddShell("/bin/bash");
    AddShell("/bin/dash");
    AddShell(qgetenv("SHELL"));
#elif defined(Q_OS_WIN)
    AddShell("C:\\Windows\\System32\\cmd.exe");
    AddShell("C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe");
    QFileInfo fi;
    if(fi.exists("c:\\msys64\\usr\\bin\\bash.exe"))
        AddShell("C:\\Windows\\System32\\cmd.exe", "msys64 bash shell");
    if(fi.exists("c:\\cygwin64\\bin\\bash.exe"))
        AddShell("C:\\Windows\\System32\\cmd.exe", "cygwin64 bash shell");
    if(fi.exists("C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat")) {
        AddShell("C:\\Windows\\System32\\cmd.exe", tr("VS 2022 Community x86"));
        AddShell("C:\\Windows\\System32\\cmd.exe", tr("VS 2022 Community x64"));
    }
    if(fi.exists("C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat")) {
        AddShell("C:\\Windows\\System32\\cmd.exe", tr("VS 2022 Professional x86"));
        AddShell("C:\\Windows\\System32\\cmd.exe", tr("VS 2022 Professional x64"));
    }
    if(fi.exists("C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat")) {
        AddShell("C:\\Windows\\System32\\cmd.exe", tr("VS 2019 Professional x86"));
        AddShell("C:\\Windows\\System32\\cmd.exe", tr("VS 2019 Professional x64"));
    }
    AddShell(qgetenv("ComSpec"));
    QString szHelp;
    szHelp = tr("Help:") + "\n";
    szHelp += tr("- Set bash shell:") + "\n";
    szHelp += tr("  cmd shell with parameters:") + " \"/k %BASH_PATH% -l\"\n";
    szHelp += tr("  eg:") + "\n";
    szHelp += tr("    - msys64 bash shell default path:") + " \"c:\\msys64\\usr\\bin\\bash.exe\",\n";
    szHelp += tr("      So that cmd shell with parameters:") + " \"/k \"c:\\msys64\\usr\\bin\\bash.exe\" -l\"\n";
    szHelp += tr("    - cygwin64 bash shell default path:") + " \"c:\\cygwin64\\bin\\bash.exe\",\n";
    szHelp += tr("      So that cmd shell with parameters:") + " \"/k \"c:\\cygwin64\\bin\\bash.exe\" -l\"\n";
    szHelp += tr("- Set sh shell:") + "\n";
    szHelp += tr("  cmd shell with parameters:") + " \"/k %SH_PATH% -l\"\n";
    szHelp += tr("  eg:") + "\n";
    szHelp += tr("    - msys64 sh shell default path:") + " \"c:\\msys64\\usr\\bin\\sh.exe\",\n";
    szHelp += tr("      So that cmd shell with parameters:") + " \"/k \"c:\\msys64\\usr\\bin\\sh.exe\" -l\"\n";
    szHelp += tr("    - cygwin64 sh shell default path:") + " \"c:\\cygwin64\\bin\\sh.exe\",\n";
    szHelp += tr("      So that cmd shell with parameters:") + " \"/k \"c:\\cygwin64\\bin\\sh.exe\" -l\"\n";
    szHelp += tr("- Set dash shell:") + "\n";
    szHelp += tr("  cmd shell with parameters:") + " \"/k %DASH_PATH% -l\"\n";
    szHelp += tr("  eg:") + "\n";
    szHelp += tr("    - msys64 dash shell default path:") + " \"c:\\msys64\\usr\\bin\\dash.exe\",\n";
    szHelp += tr("      So that cmd shell with parameters:") + " \"/k \"c:\\msys64\\usr\\bin\\dash.exe\" -l\"\n";
    szHelp += tr("    - cygwin64 dash shell default path:") + " \"c:\\cygwin64\\bin\\dash.exe\",\n";
    szHelp += tr("      So that cmd shell with parameters:") + " \"/k \"c:\\cygwin64\\bin\\dash.exe\" -l\"\n";
    szHelp += tr("- Set VS sdk shell:") + "\n";
    szHelp += tr("  cmd shell with parameters:") + " \"/k %SDK_PATH%\"\n";
    szHelp += tr("  eg:") + "\n";
    szHelp += tr("    - VS 2022 Community x64 default path:") + " \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat\",\n";
    szHelp += tr("      So that cmd shell with parameters:") + "/k \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x64\n";
    szHelp += tr("    - VS 2022 Community x86 default path:") + " \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat\",\n";
    szHelp += tr("      So that cmd shell with parameters:") + "/k \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x86\n";
    szHelp += tr("    - VS 2022 Professional x64 default path:") + " \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\",\n";
    szHelp += tr("      So that cmd shell with parameters:") + "/k \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x64\n";
    szHelp += tr("    - VS 2022 Professional x86 default path:") + " \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\",\n";
    szHelp += tr("      So that cmd shell with parameters:") + "/k \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x86\n";
    szHelp += tr("    - VS 2019 Professional x64 default path:") + " \"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\",\n";
    szHelp += tr("      So that cmd shell with parameters:") + "/k \"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x64\n";
    szHelp += tr("    - VS 2019 Professional x86 default path:") + " \"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\",\n";
    szHelp += tr("      So that cmd shell with parameters:") + "/k \"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x86\n";
    ui->teHelp->setMarkdown(szHelp);
    ui->pbHelp->show();
#endif

    ui->leName->setText(m_pPara->GetName());

    QString szShell(m_pPara->GetShell());
    AddShell(szShell);

    if(!m_pPara->GetShellName().isEmpty()) {
        ui->cbShellName->setCurrentText(m_pPara->GetShellName());
        int nIndex = ui->cbShellName->findText(m_pPara->GetShellName(), Qt::MatchFixedString);
        if(-1 != nIndex)
            ui->cbShellName->setCurrentIndex(nIndex);
    }
    ui->leShellPath->setText(m_pPara->GetShell());
    ui->leParameters->setText(m_pPara->GetShellParameters());

    ui->cbRestoreDirctory->setChecked(m_pPara->GetRestoreDirectory());
    m_pFrmParaAppearnce =
            new CParameterTerminalUI(this);
    if(m_pFrmParaAppearnce) {
        m_pFrmParaAppearnce->SetParameter(&m_pPara->m_Terminal);
        ui->tabWidget->addTab(m_pFrmParaAppearnce,
                          m_pFrmParaAppearnce->windowTitle());
    }

    ui->cbTitleChanged->setChecked(m_pPara->GetEnableTitleChanged());

    foreach(auto c, m_pPara->GetCommands()) {
        ui->lvCommands->addItem(c);
    }
}

CDlgSettingsTerminal::~CDlgSettingsTerminal()
{
    qDebug(log) << "CDlgSettingsTerminal::~CDlgSettingsTerminal()";
    delete ui;
}

void CDlgSettingsTerminal::on_pbOk_clicked()
{
    if(ui->leShellPath->text().isEmpty())
    {
        ui->tabWidget->setCurrentIndex(0);
        ui->leShellPath->setFocus();
        QMessageBox::critical(this, tr("Error"), tr("The shell is empty"));
        return;
    }
    m_pPara->SetName(ui->leName->text());
    m_pPara->SetShellName(ui->cbShellName->currentText());
    m_pPara->SetShell(ui->leShellPath->text());
    m_pPara->SetShellParameters(ui->leParameters->text());
    m_pPara->SetRestoreDirectory(ui->cbRestoreDirctory->isChecked());
    if(m_pFrmParaAppearnce)
        m_pFrmParaAppearnce->Accept();

    m_pPara->SetEnableTitleChanged(ui->cbTitleChanged->isChecked());
    QStringList cmds;
    for(int i = 0; i < ui->lvCommands->count(); i++) {
        auto c = ui->lvCommands->item(i)->data(Qt::DisplayRole).toString();
        cmds << c;
    }
    if(!cmds.isEmpty())
        m_pPara->SetCommands(cmds);

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

    nIndex = ui->cbShellName->findText(szName, Qt::MatchFixedString);
    if(-1 == nIndex) {
        ui->cbShellName->addItem(szName, szShell);
        nIndex = ui->cbShellName->findText(szName, Qt::MatchFixedString);
    }

    ui->cbShellName->setItemData(nIndex, szShell, Qt::ToolTipRole);
    ui->cbShellName->setItemData(nIndex, szShell, Qt::StatusTipRole);
    ui->cbShellName->setCurrentIndex(nIndex);

    return 0;
}

void CDlgSettingsTerminal::on_pbShellBrowse_clicked()
{
    QUrl url = QFileDialog::getOpenFileUrl(
        this, tr("Select shell"),
        QUrl::fromLocalFile(ui->leShellPath->text()));
    if(url.isEmpty())
        return;
    AddShell(url.toLocalFile());
}

void CDlgSettingsTerminal::on_cbShellName_currentIndexChanged(int index)
{
    QString szData, szName;
    szData = ui->cbShellName->itemData(index).toString();
    szName = ui->cbShellName->itemText(index);
    ui->cbShellName->setToolTip(szData);
    ui->cbShellName->setStatusTip(szData);
    ui->leShellPath->setText(szData);
    ui->leParameters->setText("");

    if("msys64 bash shell" == szName)
        ui->leParameters->setText("/k c:\\msys64\\usr\\bin\\bash.exe -l");
    if("cygwin64 bash shell" == szName)
        ui->leParameters->setText("/k c:\\cygwin64\\bin\\bash.exe -l");
    if(tr("VS 2022 Community x64") == szName)
        ui->leParameters->setText("/k \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x64");
    if(tr("VS 2022 Community x86") == szName)
        ui->leParameters->setText("/k \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x86");
    if(tr("VS 2022 Professional x64") == szName)
        ui->leParameters->setText("/k \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x64");
    if(tr("VS 2022 Professional x86") == szName)
        ui->leParameters->setText("/k \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x86");
    if(tr("VS 2019 Professional x64") == szName)
        ui->leParameters->setText("/k \"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x64");
    if(tr("VS 2019 Professional x86") == szName)
        ui->leParameters->setText("/k \"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x86");

    if(m_pPara->GetShellName() == szName)
        ui->leParameters->setText(m_pPara->GetShellParameters());
}

void CDlgSettingsTerminal::on_pbAddCommand_clicked()
{
    QString szCmd = ui->leCommand->text();
    if(szCmd.isEmpty())
        return;
    if(ui->lvCommands->findItems(szCmd, Qt::MatchCaseSensitive).isEmpty())
        ui->lvCommands->addItem(szCmd);
    ui->leCommand->setText(QString());
}

void CDlgSettingsTerminal::on_leCommand_editingFinished()
{
    on_pbAddCommand_clicked();
}

void CDlgSettingsTerminal::on_pbDeleteCommands_clicked()
{
    auto item = ui->lvCommands->currentItem();
    delete item;
}

void CDlgSettingsTerminal::on_pbHelp_clicked()
{
    if(ui->teHelp->isVisible())
    {
        ui->teHelp->hide();
        ui->pbHelp->setText(tr("Help(&H)"));
    } else {
        ui->teHelp->show();
        ui->pbHelp->setText(tr("Hide help(&H)"));
    }
}
