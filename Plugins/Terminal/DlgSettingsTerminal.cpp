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
    
#if defined(Q_OS_UNIX)    
    AddShell("/bin/sh");
    AddShell("/bin/bash");
    AddShell("/bin/dash");
    AddShell(qgetenv("SHELL"));
#elif defined(Q_OS_WIN)
    AddShell("C:\\Windows\\System32\\cmd.exe");
    AddShell("C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe");
    AddShell(qgetenv("ComSpec"));
#endif

    QString szShell(m_pPara->GetShell());
    AddShell(szShell);

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
    if(m_pFrmParaAppearnce)
        m_pFrmParaAppearnce->Accept();

    m_pPara->SetEnableTitleChanged(ui->cbTitleChanged->isChecked());
    accept();
}

void CDlgSettingsTerminal::on_pbCancel_clicked()
{
    this->reject();
}

int CDlgSettingsTerminal::AddShell(QString szShell)
{
    int nIndex = 0;
    nIndex = ui->cbShell->findData(szShell
#if defined(Q_OS_WIN)
                                   , Qt::UserRole, Qt::MatchFixedString
#endif
                                   );
    if(-1 == nIndex) {
        QFileInfo fi(szShell);
        if(!fi.exists())
        {
            qCritical(log) << "The shell is not exist:" << szShell;
            return -1;
        }
        ui->cbShell->addItem(fi.baseName(), szShell);
        nIndex =  ui->cbShell->findData(szShell
#if defined(Q_OS_WIN)
                                       , Qt::UserRole, Qt::MatchFixedString
#endif
                                       );
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
    QString szText;
    szText = ui->cbShell->itemData(index).toString();
    ui->cbShell->setToolTip(szText);
    ui->cbShell->setStatusTip(szText);
}

