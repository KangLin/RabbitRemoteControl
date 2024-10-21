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
#include <QWindowCapture>
#include <QCapturableWindow>
#include "DlgPlayer.h"
#include "ui_DlgPlayer.h"

static Q_LOGGING_CATEGORY(log, "Player.Connect")
CDlgPlayer::CDlgPlayer(CParameterPlayer *pPara, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgPlayer)
    , m_pParameters(pPara)
{
    ui->setupUi(this);

    foreach(const QCameraDevice &cameraDevice, QMediaDevices::videoInputs()) {
        ui->cmbCamera->addItem(cameraDevice.description());
    }
    ui->cmbType->addItem(tr("Camera"), (int)CParameterPlayer::TYPE::Camera);
    ui->cmbType->addItem(tr("Url"), (int)CParameterPlayer::TYPE::Url);
    ui->cmbType->addItem(tr("Capture screen"), (int)CParameterPlayer::TYPE::CaptureScreen);
    ui->cmbType->addItem(tr("Capture window"), (int)CParameterPlayer::TYPE::CaptureWindow);
    int nIndex = ui->cmbType->findData((int)m_pParameters->GetType());
    ui->cmbType->setCurrentIndex(nIndex);

    ui->leUrl->setText(m_pParameters->GetUrl());
    if(m_pParameters->GetCamera() != -1)
        ui->cmbCamera->setCurrentIndex(m_pParameters->GetCamera());

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

    auto lstScreens = QGuiApplication::screens();
    for(int i = 0; i < lstScreens.size(); i++)
        ui->cmbScreens->addItem(tr("Screen:") + QString::number(i), i);
    nIndex = m_pParameters->GetScreen();
    if(-1 < nIndex && nIndex < lstScreens.size()) {
        ui->cmbScreens->setCurrentIndex(nIndex);
    }
}

CDlgPlayer::~CDlgPlayer()
{
    delete ui;
}

void CDlgPlayer::accept()
{
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
    m_pParameters->SetScreen(ui->cmbScreens->currentIndex());

    QDialog::accept();
}

void CDlgPlayer::on_cmbType_currentIndexChanged(int index)
{
    ui->cmbCamera->setVisible(false);
    ui->leUrl->setVisible(false);
    ui->pbUrlBrowse->setVisible(false);
    ui->cmbScreens->setVisible(false);
    ui->cmbWindows->setVisible(false);

    switch((CParameterPlayer::TYPE) index) {
    case CParameterPlayer::TYPE::Camera:
        ui->cmbCamera->setVisible(true);
        break;
    case CParameterPlayer::TYPE::Url:
        ui->leUrl->setVisible(true);
        ui->pbUrlBrowse->setVisible(true);
        break;
    case CParameterPlayer::TYPE::CaptureScreen:
        ui->cmbScreens->setVisible(true);
        break;
    case CParameterPlayer::TYPE::CaptureWindow:
    {
        ui->cmbWindows->clear();
        auto lstWin = QWindowCapture::capturableWindows();
        foreach(auto w, lstWin) {
            qDebug(log) << w.description();
            ui->cmbWindows->addItem(w.description());
        }
        ui->cmbWindows->setVisible(true);
        break;
    }
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
