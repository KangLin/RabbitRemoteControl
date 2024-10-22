// Author: Kang Lin <kl222@126.com>

#include <QCamera>
#include <QCameraDevice>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMediaRecorder>
#include <QMediaFormat>
#include <QLoggingCategory>
#include <QScreen>
#include <QGuiApplication>
#include <QMessageBox>

#include "DlgPlayer.h"
#include "ui_DlgPlayer.h"

static Q_LOGGING_CATEGORY(log, "Player.Connect")
CDlgPlayer::CDlgPlayer(CParameterPlayer *pPara, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgPlayer)
    , m_pParameters(pPara)
{
    ui->setupUi(this);

    foreach(auto ai, QMediaDevices::audioInputs())
    {
        ui->cmbAudioInput->addItem(ai.description());
    }
    ui->gbAudioInput->setCheckable(true);
    if(QMediaDevices::audioInputs().size() > 0)
        ui->gbAudioInput->setChecked(m_pParameters->GetEnableAudioInput());
    else {
        ui->gbAudioInput->setEnabled(false);
        ui->gbAudioInput->setChecked(false);
    }
    if(m_pParameters->GetAudioInput() != -1)
        ui->cmbAudioInput->setCurrentIndex(m_pParameters->GetAudioInput());
    ui->cbAudioInputMuted->setChecked(m_pParameters->GetAudioInputMuted());
    ui->dsAudioInputVolume->setValue(m_pParameters->GetAudioInputVolume());

    foreach(auto ao, QMediaDevices::audioOutputs())
        ui->cmbAudioOutput->addItem(ao.description());
    ui->gbAudioOutput->setCheckable(true);
    if(QMediaDevices::audioOutputs().size() > 0)
        ui->gbAudioOutput->setChecked(m_pParameters->GetEnableAudioOutput());
    else {
        ui->gbAudioOutput->setEnabled(false);
        ui->gbAudioOutput->setChecked(false);
    }
    if(m_pParameters->GetAudioOutput() != -1)
        ui->cmbAudioOutput->setCurrentIndex(m_pParameters->GetAudioOutput());
    ui->cbAudioOutputMuted->setCheckable(true);
    ui->cbAudioOutputMuted->setChecked(m_pParameters->GetAudioOutputMuted());
    ui->dsAudioOutputVolume->setValue(m_pParameters->GetAudioOutputVolume());

    if(QMediaDevices::videoInputs().size() > 0)
    {
        ui->cmbType->addItem(tr("Camera"), (int)CParameterPlayer::TYPE::Camera);
        foreach(const QCameraDevice &cameraDevice, QMediaDevices::videoInputs()) {
            ui->cmbCamera->addItem(cameraDevice.description());
        }
        if(m_pParameters->GetCamera() > -1)
            ui->cmbCamera->setCurrentIndex(m_pParameters->GetCamera());
    }

    ui->cmbType->addItem(tr("Url"), (int)CParameterPlayer::TYPE::Url);
    ui->leUrl->setText(m_pParameters->GetUrl());

    int nIndex = ui->cmbType->findData((int)m_pParameters->GetType());
    if(-1 < nIndex) {
        ui->cmbType->setCurrentIndex(nIndex);
        on_cmbType_currentIndexChanged(nIndex);
    }

    m_pRecordUI = new CParameterRecordUI(ui->tabWidget);
    m_pRecordUI->SetParameter(&m_pParameters->m_Record);
    ui->tabWidget->addTab(m_pRecordUI, m_pRecordUI->windowIcon(), m_pRecordUI->windowTitle());
}

CDlgPlayer::~CDlgPlayer()
{
    delete ui;
}

void CDlgPlayer::accept()
{
    int nRet = 0;

    if((ui->cmbType->currentData().value<CParameterPlayer::TYPE>()
         == CParameterPlayer::TYPE::Url) && ui->leUrl->text().isEmpty()) {
        ui->leUrl->setFocus();
        ui->tabWidget->setCurrentIndex(0);
        QString szErr = tr("The url is empty. please set!");
        QMessageBox::critical(this, tr("Error"), szErr);
        qCritical(log) << szErr;
        return;
    }

    if(!m_pRecordUI->CheckValidity(true)) {
        ui->tabWidget->setCurrentWidget(m_pRecordUI);
        return;
    }

    m_pParameters->SetType(
        ui->cmbType->currentData().value<CParameterPlayer::TYPE>());
    m_pParameters->SetUrl(ui->leUrl->text());
    m_pParameters->SetCamera(ui->cmbCamera->currentIndex());
    switch(m_pParameters->GetType()) {
    case CParameterPlayer::TYPE::Camera: {
        int nIndex = m_pParameters->GetCamera();
        if(-1 < nIndex && nIndex < QMediaDevices::videoInputs().size())
            m_pParameters->SetName(
                tr("Camera: ")
                + QMediaDevices::videoInputs().at(nIndex).description());
        break;
    }
    case CParameterPlayer::TYPE::Url:
        QFileInfo fi(m_pParameters->GetUrl());
        m_pParameters->SetName(tr("Url: ") + fi.fileName());
        break;
    }

    m_pParameters->SetEnableAudioInput(ui->gbAudioInput->isChecked());
    m_pParameters->SetAudioInput(ui->cmbAudioInput->currentIndex());
    m_pParameters->SetAudioInputMuted(ui->cbAudioInputMuted->isChecked());
    m_pParameters->SetAudioInputVolume(ui->dsAudioInputVolume->value());
    m_pParameters->SetEnableAudioOutput(ui->gbAudioOutput->isChecked());
    m_pParameters->SetAudioOutput(ui->cmbAudioOutput->currentIndex());
    m_pParameters->SetAudioOutputMuted(ui->cbAudioOutputMuted->isChecked());
    m_pParameters->SetAudioOutputVolume(ui->dsAudioOutputVolume->value());

    nRet = m_pRecordUI->Accept();
    if(nRet)
        return;

    QDialog::accept();
}

void CDlgPlayer::on_cmbType_currentIndexChanged(int index)
{
    ui->cmbCamera->setVisible(false);
    ui->leUrl->setVisible(false);
    ui->pbUrlBrowse->setVisible(false);

    CParameterPlayer::TYPE type
        = (CParameterPlayer::TYPE)ui->cmbType->itemData(index).toInt();
    switch(type) {
    case CParameterPlayer::TYPE::Camera:
        ui->cmbCamera->setVisible(true);
        ui->gbAudioInput->setChecked(true);
        break;
    case CParameterPlayer::TYPE::Url:
        ui->leUrl->setVisible(true);
        ui->pbUrlBrowse->setVisible(true);
        ui->gbAudioInput->setChecked(false);
        break;
    default:
        break;
    }
}

void CDlgPlayer::on_pbUrlBrowse_clicked()
{
    /*
    QMediaRecorder recorder;
    auto formats = recorder.mediaFormat().supportedFileFormats(QMediaFormat::Decode);
    qDebug(log) << "Support file formats:" << formats;
    foreach(auto f, formats) {
        qDebug(log) << "File format name:" << QMediaFormat::fileFormatName(f);
    } //*/
    QString szFilter(tr("Media files(* wmv *.aac *.WebM *.mtk *.ogg *.mp4 *.mov *.avi);;All files(*.*)"));
    QString szFile = QFileDialog::getOpenFileName(
        this, tr("Open file"),
        QStandardPaths::writableLocation(QStandardPaths::MoviesLocation),
        szFilter);
    if(!szFile.isEmpty())
        ui->leUrl->setText(szFile);
}
