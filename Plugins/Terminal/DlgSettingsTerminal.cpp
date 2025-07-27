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

CDlgSettingsTerminal::CDlgSettingsTerminal(CParameterTerminalBase *pPara, QWidget *parent) :
    QDialog(parent)
    , ui(new Ui::CDlgSettingsTerminal)
    , m_pPara(pPara)
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
    AddShell("C:\\Windows\\System32\\cmd.exe", "msys64 bash shell");
    AddShell("C:\\Windows\\System32\\cmd.exe", "cygwin64 bash shell");
    AddShell("C:\\Windows\\System32\\cmd.exe", tr("VS 2022 Professional x86"));
    AddShell("C:\\Windows\\System32\\cmd.exe", tr("VS 2022 Professional x64"));
    AddShell("C:\\Windows\\System32\\cmd.exe", tr("VS 2019 Professional x86"));
    AddShell("C:\\Windows\\System32\\cmd.exe", tr("VS 2019 Professional x64"));
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
    szHelp += tr("    - VS 2022 Professional x64 default path:") + " \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\",\n";
    szHelp += tr("      So that cmd shell with parameters:") + "/k \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x64\n";
    szHelp += tr("    - VS 2022 Professional x86 default path:") + " \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\",\n";
    szHelp += tr("      So that cmd shell with parameters:") + "/k \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x86\n";
    szHelp += tr("    - VS 2019 Professional x64 default path:") + " \"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\",\n";
    szHelp += tr("      So that cmd shell with parameters:") + "/k \"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x64\n";
    szHelp += tr("    - VS 2019 Professional x86 default path:") + " \"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\",\n";
    szHelp += tr("      So that cmd shell with parameters:") + "/k \"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x86\n";
    QString szHtml;

    szHtml = RabbitCommon::CTools::MarkDownToHtml(szHelp);

    ui->teHelp->setText(szHtml);
    ui->teHelp->show();
#endif

    QString szShell(m_pPara->GetShell());
    AddShell(szShell);
    if(!m_pPara->GetShellName().isEmpty())
        ui->cbShell->setCurrentText(m_pPara->GetShellName());
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
    if("msys64 bash shell" == szName && ui->leParameters->text().isEmpty())
        ui->leParameters->setText("/k c:\\msys64\\usr\\bin\\bash.exe -l");
    if("cygwin64 bash shell" == szName && ui->leParameters->text().isEmpty())
        ui->leParameters->setText("/k c:\\cygwin64\\bin\\bash.exe -l");
    if(tr("VS 2022 Professional x64") == szName && ui->leParameters->text().isEmpty())
        ui->leParameters->setText("/k \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x64");
    if(tr("VS 2022 Professional x86") == szName && ui->leParameters->text().isEmpty())
        ui->leParameters->setText("/k \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x86");
    if(tr("VS 2019 Professional x64") == szName && ui->leParameters->text().isEmpty())
        ui->leParameters->setText("/k \"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x64");
    if(tr("VS 2019 Professional x86") == szName && ui->leParameters->text().isEmpty())
        ui->leParameters->setText("/k \"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Professional\\VC\\Auxiliary\\Build\\vcvarsall.bat\" x86");
}

void CDlgSettingsTerminal::on_pbAddCommand_clicked()
{
    QString szCmd = ui->leCommand->text();
    if(szCmd.isEmpty())
        return;
    if(ui->lvCommands->findItems(szCmd, Qt::MatchCaseSensitive).isEmpty())
        ui->lvCommands->addItem(szCmd);
}
