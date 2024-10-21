#include <QCamera>
#include <QCameraDevice>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QFileDialog>
#include <QStandardPaths>

#include "DlgPlayer.h"
#include "ui_DlgPlayer.h"

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
    else
        ui->gbAudioInput->setChecked(false);
    if(m_pParameters->GetAudioInput() != -1)
        ui->cmbAudioInput->setCurrentIndex(m_pParameters->GetAudioInput());

    foreach(auto ao, QMediaDevices::audioOutputs())
        ui->cmbAudioOutput->addItem(ao.description());
    ui->gbAudioOutput->setCheckable(true);
    if(QMediaDevices::audioOutputs().size() > 0)
        ui->gbAudioOutput->setChecked(m_pParameters->GetEnableAudioOutput());
    else
        ui->gbAudioOutput->setChecked(false);
    if(m_pParameters->GetAudioOutput() != -1)
        ui->cmbAudioOutput->setCurrentIndex(m_pParameters->GetAudioOutput());
    ui->cbAudioOutputMuted->setCheckable(true);
    ui->cbAudioOutputMuted->setChecked(m_pParameters->GetAudioOutputMuted());
    ui->dsAudioOutputVolume->setValue(m_pParameters->GetAudioOutputVolume());
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
    m_pParameters->SetEnableAudioOutput(ui->gbAudioOutput->isChecked());
    m_pParameters->SetAudioOutput(ui->cmbAudioOutput->currentIndex());
    m_pParameters->SetAudioOutputMuted(ui->cbAudioOutputMuted->isChecked());
    m_pParameters->SetAudioOutputVolume(ui->dsAudioOutputVolume->value());

    QDialog::accept();
}

void CDlgPlayer::on_cmbType_currentIndexChanged(int index)
{
    if(index == (int)CParameterPlayer::TYPE::Camera) {
        ui->cmbCamera->setVisible(true);
        ui->leUrl->setVisible(false);
        ui->pbUrlBrowse->setVisible(false);
    } else {
        ui->cmbCamera->setVisible(false);
        ui->leUrl->setVisible(true);
        ui->pbUrlBrowse->setVisible(true);
    }
}

void CDlgPlayer::on_pbUrlBrowse_clicked()
{
    QString szFilter(tr("Video files(*.mp4 *.avi);;Audio files(*.mp3);;All files(*.*)"));
    QString szFile = QFileDialog::getOpenFileName(
        this, tr("Open file"),
        QStandardPaths::writableLocation(QStandardPaths::MoviesLocation),
        szFilter);
    if(!szFile.isEmpty())
        ui->leUrl->setText(szFile);
}
