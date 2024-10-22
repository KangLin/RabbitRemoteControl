#include <QWindowCapture>
#include <QFileDialog>
#include <QLoggingCategory>

#include "DlgCapture.h"
#include "ui_DlgCapture.h"

static Q_LOGGING_CATEGORY(log, "Screen.Capture.Dialog")
CDlgCapture::CDlgCapture(CParameterScreenCapture *pPara, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgCapture)
    , m_pParameters(pPara)
{
    ui->setupUi(this);

    auto screens = QGuiApplication::screens();
    ui->sbScreen->setMinimum(0);
    ui->sbScreen->setMaximum(screens.size() - 1);
    ui->sbScreen->setValue(m_pParameters->GetScreen());

    m_Windows = QWindowCapture::capturableWindows();
    foreach(auto w, m_Windows)
        ui->cmbWindows->addItem(w.description());

    //TODO: Complete below
    ui->rbCustomize->setVisible(false);
    ui->rbCustomRectangle->setVisible(false);

    switch(m_pParameters->GetTarget())
    {
    case CParameterScreenCapture::TARGET::Screen:
        ui->rbScreen->setChecked(true);
        break;
    case CParameterScreenCapture::TARGET::Window:
        ui->rbWindow->setChecked(true);
        break;
    case CParameterScreenCapture::TARGET::CustomRectangle:
        ui->rbCustomRectangle->setChecked(true);
        break;
    case CParameterScreenCapture::TARGET::Curstomize:
        ui->rbCustomize->setChecked(true);
        break;
    }

    switch(m_pParameters->GetOperate())
    {
    case CParameterScreenCapture::OPERATE::Shot:
        ui->rbShot->setChecked(true);
        break;
    case CParameterScreenCapture::OPERATE::Record:
        ui->rbRecord->setChecked(true);
        break;
    }

    auto &record = m_pParameters->m_Record;
    ui->cbRecordEnable->setChecked(record.GetEnable());
    ui->lePath->setText(record.GetPath());
}

CDlgCapture::~CDlgCapture()
{
    delete ui;
}

void CDlgCapture::accept()
{
    if(ui->rbScreen->isChecked())
        m_pParameters->SetTarget(CParameterScreenCapture::TARGET::Screen);
    else if(ui->rbWindow->isChecked())
        m_pParameters->SetTarget(CParameterScreenCapture::TARGET::Window);
    else if(ui->rbCustomRectangle->isChecked())
        m_pParameters->SetTarget(CParameterScreenCapture::TARGET::CustomRectangle);
    else if(ui->rbCustomize->isChecked())
        m_pParameters->SetTarget(CParameterScreenCapture::TARGET::Curstomize);

    if(ui->rbShot->isChecked())
        m_pParameters->SetOperate(CParameterScreenCapture::OPERATE::Shot);
    else if(ui->rbRecord->isChecked())
        m_pParameters->SetOperate(CParameterScreenCapture::OPERATE::Record);

    m_pParameters->SetScreen(ui->sbScreen->value());
    int nIndex = ui->cmbWindows->currentIndex();
    if(-1 < nIndex && nIndex < m_Windows.size())
        m_pParameters->SetWindow(m_Windows.at(nIndex));

    auto &record = m_pParameters->m_Record;
    record.SetEnable(ui->cbRecordEnable->isChecked());
    record.SetPath(ui->lePath->text());
    QString szFile = record.GetPath()
                     + QDir::separator()
                     + QDateTime::currentDateTime().toLocalTime()
                           .toString("yyyy_MM_dd_hh_mm_ss_zzz");
    if(ui->rbShot->isChecked())
        szFile += ".png";
    else if(ui->rbRecord->isChecked())
        szFile += ".mp4";
    record.SetUrl(szFile);

    QDialog::accept();
}

void CDlgCapture::on_pbPathBrowe_clicked()
{
    QString szPath;
    szPath = QFileDialog::getExistingDirectory(
        this, tr("Open path"),
        m_pParameters->m_Record.GetPath());
    if(szPath.isEmpty())
        return;
    ui->lePath->setText(szPath);
}
