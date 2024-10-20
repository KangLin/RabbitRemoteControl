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

    for (const QCameraDevice &cameraDevice : QMediaDevices::videoInputs()) {
        ui->cmbCamera->addItem(cameraDevice.description());
    }
    ui->cmbType->addItem(tr("Camera"), (int)CParameterPlayer::TYPE::Camera);
    ui->cmbType->addItem(tr("Url"), (int)CParameterPlayer::TYPE::Url);
    int nIndex = ui->cmbType->findData((int)m_pParameters->GetType());
    ui->cmbType->setCurrentIndex(nIndex);

    ui->leUrl->setText(m_pParameters->GetUrl());
    ui->cmbCamera->setCurrentIndex(m_pParameters->GetCamera());

    for(auto ai : QMediaDevices::audioInputs())
    {
        ui->cmbAudioInput->addItem(ai.description());
    }
    ui->gbAudioInput->setCheckable(true);
    ui->gbAudioInput->setChecked(m_pParameters->GetEnableAudioInput());
    ui->cmbAudioInput->setCurrentIndex(m_pParameters->GetAudioInput());

    for(auto ao : QMediaDevices::audioOutputs())
        ui->cmbAudioOutput->addItem(ao.description());
    ui->gbAudioOutput->setCheckable(true);
    ui->gbAudioOutput->setChecked(m_pParameters->GetEnableAudioOutput());
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
    case CParameterPlayer::TYPE::Camera:
        m_pParameters->SetName(tr("Camera: ") + QMediaDevices::videoInputs().at(m_pParameters->GetCamera()).description());
        break;
    case CParameterPlayer::TYPE::Url:
        m_pParameters->SetName(tr("Url: ") + m_pParameters->GetUrl());
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

