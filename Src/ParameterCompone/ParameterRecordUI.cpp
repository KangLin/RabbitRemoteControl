// Author: Kang Lin <kl222@126.com>
#include <QDir>
#include <QDateTime>
#include <QFileDialog>
#include <QMediaRecorder>
#include <QLoggingCategory>
#if HAVE_QT6_MULTIMEDIA
    #include <QMediaFormat>
#endif
#include "ParameterRecordUI.h"
#include "ui_ParameterRecordUI.h"

static Q_LOGGING_CATEGORY(log, "Parameter.Record.UI")
CParameterRecordUI::CParameterRecordUI(QWidget *parent)
    : CParameterUI(parent)
    , ui(new Ui::CParameterRecordUI)
    , m_pParameters(nullptr)
{
    ui->setupUi(this);
    setWindowTitle(tr("Record"));
    setWindowIcon(QIcon::fromTheme("media-record"));
#if HAVE_QT6_MULTIMEDIA
    QMediaRecorder recorder;
    auto formats = recorder.mediaFormat().supportedFileFormats(QMediaFormat::Encode);
    foreach(auto f, formats) {
        qDebug(log) << "File format name:" << f;
        ui->cmbFileFormat->addItem(QMediaFormat::fileFormatName(f), f);
    }
    auto video = recorder.mediaFormat().supportedVideoCodecs(QMediaFormat::Encode);
    foreach (auto v, video) {
        qDebug(log) << "Video codec:" << v;
        ui->cmbVideoEncode->addItem(QMediaFormat::videoCodecName(v), (int)v);
    }
    ui->dsbFrameRate->setToolTip(tr("A value of 0 indicates the recorder should make an optimal choice based on what is available from the video source and the limitations of the codec.") + "\n" +
                                 tr("If record remote desktop, recommended options: 24, 50, 60"));
    auto audio = recorder.mediaFormat().supportedAudioCodecs(QMediaFormat::Encode);
    foreach(auto a, audio) {
        qDebug(log) << "Audio codec:" << a;
        ui->cmbAudioEncode->addItem(QMediaFormat::audioCodecName(a), (int)a);
    }
    ui->sbSampleRate->setToolTip(tr("A value of -1 indicates the recorder should make an optimal choice based on what is available from the audio source, and the limitations of the codec. options: 8kHz, 11.025kHz, 22.05kHz, 16kHz, 37.8kHz, 44.1kHz, 48kHz, 96kHz, 192kHz etc"));

    ui->cmbQuality->addItem("Very height", QMediaRecorder::Quality::VeryHighQuality);
    ui->cmbQuality->addItem("Height", QMediaRecorder::Quality::HighQuality);
    ui->cmbQuality->addItem("Normal", QMediaRecorder::Quality::NormalQuality);
    ui->cmbQuality->addItem("Low", QMediaRecorder::Quality::LowQuality);
    ui->cmbQuality->addItem("Very low", QMediaRecorder::Quality::VeryLowQuality);

    ui->cmbEncodingMode->addItem("Constant quality", QMediaRecorder::EncodingMode::ConstantQualityEncoding);
    ui->cmbEncodingMode->setItemData(
        QMediaRecorder::EncodingMode::ConstantQualityEncoding,
        tr("Encoding will aim to have a constant quality, adjusting bitrate to fit."),
        Qt::ToolTipRole);
    ui->cmbEncodingMode->addItem("Constant bit rate", QMediaRecorder::EncodingMode::ConstantBitRateEncoding);
    ui->cmbEncodingMode->setItemData(
        QMediaRecorder::EncodingMode::ConstantBitRateEncoding,
        tr("Encoding will use a constant bit rate, adjust quality to fit."),
        Qt::ToolTipRole);
    ui->cmbEncodingMode->addItem("Average bit rate", QMediaRecorder::EncodingMode::AverageBitRateEncoding);
    ui->cmbEncodingMode->setItemData(
        QMediaRecorder::EncodingMode::AverageBitRateEncoding,
        tr("Encoding will try to keep an average bitrate setting, but will use more or less as needed."),
        Qt::ToolTipRole);
    ui->cmbEncodingMode->addItem("Two pass", QMediaRecorder::EncodingMode::TwoPassEncoding);
    ui->cmbEncodingMode->setItemData(
        QMediaRecorder::EncodingMode::TwoPassEncoding,
        tr("The media will first be processed to determine the characteristics, and then processed a second time allocating more bits to the areas that need it."),
        Qt::ToolTipRole);
    on_cmbEncodingMode_currentIndexChanged(ui->cmbEncodingMode->currentIndex());
#endif
}

CParameterRecordUI::~CParameterRecordUI()
{
    delete ui;
}

int CParameterRecordUI::SetParameter(CParameter *pParameter)
{
    m_pParameters = qobject_cast<CParameterRecord*>(pParameter);
    if(!m_pParameters)
        return 1;
    ui->gbVideo->setChecked(m_pParameters->GetEnableVideo());
    ui->gbAudio->setChecked(m_pParameters->GetEnableAudio());
    ui->leFolder->setText(m_pParameters->GetVideoPath());
#if HAVE_QT6_MULTIMEDIA
    int nIndex = -1;
    nIndex = ui->cmbFileFormat->findData((int)m_pParameters->GetFileFormat());
    if(-1 < nIndex)
        ui->cmbFileFormat->setCurrentIndex(nIndex);

    nIndex = ui->cmbVideoEncode->findData((int)m_pParameters->GetVideoCodec());
    if(-1 < nIndex)
        ui->cmbVideoEncode->setCurrentIndex(nIndex);

    nIndex = ui->cmbAudioEncode->findData((int)m_pParameters->GetAudioCodec());
    if(-1 < nIndex)
        ui->cmbAudioEncode->setCurrentIndex(nIndex);

    nIndex = ui->cmbQuality->findData((int)m_pParameters->GetQuality());
    if(-1 < nIndex)
        ui->cmbQuality->setCurrentIndex(nIndex);

    nIndex = ui->cmbEncodingMode->findData((int)m_pParameters->GetEncodingMode());
    if(-1 < nIndex)
        ui->cmbEncodingMode->setCurrentIndex(nIndex);
#endif
    ui->dsbFrameRate->setValue(m_pParameters->GetVideoFrameRate());
    ui->sbSampleRate->setValue(m_pParameters->GetAudioSampleRate());

    switch(m_pParameters->GetEndAction()) {
    case CParameterRecord::ENDACTION::No:
        ui->rbNoAction->setChecked(true);
        break;
    case CParameterRecord::ENDACTION::OpenFile:
        ui->rbOpenFile->setChecked(true);
        break;
    case CParameterRecord::ENDACTION::OpenFolder:
        ui->rbOpenFolder->setChecked(true);
        break;
    }

    return 0;
}

int CParameterRecordUI::Accept()
{
    m_pParameters->SetEnableVideo(ui->gbVideo->isChecked());
    m_pParameters->SetEnableAudio(ui->gbAudio->isChecked());
    m_pParameters->SetVideoPath(ui->leFolder->text());
    QString szFile = m_pParameters->GetVideoPath()
                     + QDir::separator()
                     + QDateTime::currentDateTime().toLocalTime()
                           .toString("yyyy_MM_dd_hh_mm_ss_zzz");
    m_pParameters->SetImageFile(szFile + ".png");
    m_pParameters->SetVideoFile(szFile);
#if HAVE_QT6_MULTIMEDIA
    int nData = ui->cmbFileFormat->currentData().toInt();
    m_pParameters->SetFileFormat((QMediaFormat::FileFormat)nData);

    nData = ui->cmbVideoEncode->currentData().toInt();
    m_pParameters->SetVideoCodec((QMediaFormat::VideoCodec)nData);

    nData = ui->cmbAudioEncode->currentData().toInt();
    m_pParameters->SetAudioCodec((QMediaFormat::AudioCodec)nData);

    nData = ui->cmbQuality->currentData().toInt();
    m_pParameters->SetQuality((QMediaRecorder::Quality)nData);

    nData = ui->cmbEncodingMode->currentData().toInt();
    m_pParameters->SetEncodingMode((QMediaRecorder::EncodingMode)nData);
#endif
    m_pParameters->SetVideoFrameRate(ui->dsbFrameRate->value());
    m_pParameters->SetAudioSampleRate(ui->sbSampleRate->value());

    if(ui->rbNoAction->isChecked())
        m_pParameters->SetEndAction(CParameterRecord::ENDACTION::No);
    else if(ui->rbOpenFile->isChecked())
        m_pParameters->SetEndAction(CParameterRecord::ENDACTION::OpenFile);
    else if(ui->rbOpenFolder->isChecked())
        m_pParameters->SetEndAction(CParameterRecord::ENDACTION::OpenFolder);
    return 0;
}

void CParameterRecordUI::on_pbFolder_clicked()
{
    QString szPath;
    szPath = QFileDialog::getExistingDirectory(
        this, tr("Open path"),
        m_pParameters->GetVideoPath());
    if(szPath.isEmpty())
        return;
    ui->leFolder->setText(szPath);
}

void CParameterRecordUI::on_cmbEncodingMode_currentIndexChanged(int index)
{
#if HAVE_QT6_MULTIMEDIA
    if(ui->cmbEncodingMode->itemData(index) == QMediaRecorder::ConstantQualityEncoding) {
        ui->cmbQuality->setEnabled(true);
        ui->dsbFrameRate->setEnabled(false);
        ui->dsbFrameRate->setToolTip(tr("'Encoding mode' is 'constant quality'"));
        ui->sbSampleRate->setEnabled(false);
        ui->sbSampleRate->setToolTip(tr("'Encoding mode' is 'constant quality'"));
        if(ui->dsbFrameRate->value() != 0)
            ui->dsbFrameRate->setValue(0);
        if(ui->sbSampleRate->value() != -1)
            ui->sbSampleRate->setValue(-1);
    } else {
        ui->cmbQuality->setEnabled(false);
        ui->dsbFrameRate->setEnabled(true);
        ui->dsbFrameRate->setToolTip(tr("A value of 0 indicates the recorder should make an optimal choice based on what is available from the video source and the limitations of the codec.") + "\n" +
                                     tr("If record remote desktop, recommended options: 24, 50, 60"));
        ui->sbSampleRate->setEnabled(true);
        ui->sbSampleRate->setToolTip(tr("A value of -1 indicates the recorder should make an optimal choice based on what is available from the audio source, and the limitations of the codec. options: 8kHz, 11.025kHz, 22.05kHz, 16kHz, 37.8kHz, 44.1kHz, 48kHz, 96kHz, 192kHz etc"));
        if(ui->dsbFrameRate->value() == 0)
            ui->dsbFrameRate->setValue(24);
        if(ui->sbSampleRate->value() == -1)
            ui->sbSampleRate->setValue(8000);
    }
#endif
}

