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
    auto audio = recorder.mediaFormat().supportedAudioCodecs(QMediaFormat::Encode);
    foreach(auto a, audio) {
        qDebug(log) << "Audio codec:" << a;
        ui->cmbAudioEncode->addItem(QMediaFormat::audioCodecName(a), (int)a);
    }

    ui->cmbQuality->addItem("Very height", QMediaRecorder::Quality::VeryHighQuality);
    ui->cmbQuality->addItem("Height", QMediaRecorder::Quality::HighQuality);
    ui->cmbQuality->addItem("Normal", QMediaRecorder::Quality::NormalQuality);
    ui->cmbQuality->addItem("Low", QMediaRecorder::Quality::LowQuality);
    ui->cmbQuality->addItem("Very low", QMediaRecorder::Quality::VeryLowQuality);

    ui->cmbEncodingMode->addItem("Constant quality", QMediaRecorder::EncodingMode::ConstantQualityEncoding);
    ui->cmbEncodingMode->addItem("Constant bit rate", QMediaRecorder::EncodingMode::ConstantBitRateEncoding);
    ui->cmbEncodingMode->addItem("Average bit rate", QMediaRecorder::EncodingMode::AverageBitRateEncoding);
    ui->cmbEncodingMode->addItem("Two pass", QMediaRecorder::EncodingMode::TwoPassEncoding);
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
    ui->gbRecord->setChecked(m_pParameters->GetEnable());
    ui->gbVideo->setChecked(m_pParameters->GetEnableVideo());
    ui->gbAudio->setChecked(m_pParameters->GetEnableAudio());
    ui->leFolder->setText(m_pParameters->GetPath());
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
    return 0;
}

int CParameterRecordUI::Accept()
{
    m_pParameters->SetEnable(ui->gbRecord->isChecked());
    m_pParameters->SetEnableVideo(ui->gbVideo->isChecked());
    m_pParameters->SetEnableAudio(ui->gbAudio->isChecked());
    m_pParameters->SetPath(ui->leFolder->text());
    QString szFile = m_pParameters->GetPath()
                     + QDir::separator()
                     + QDateTime::currentDateTime().toLocalTime()
                           .toString("yyyy_MM_dd_hh_mm_ss_zzz");
    m_pParameters->SetImageFile(szFile + ".png");
    m_pParameters->SetFile(szFile);
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

    return 0;
}

void CParameterRecordUI::on_pbFolder_clicked()
{
    QString szPath;
    szPath = QFileDialog::getExistingDirectory(
        this, tr("Open path"),
        m_pParameters->GetPath());
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
        ui->sbSampleRate->setEnabled(false);
    } else {
        ui->cmbQuality->setEnabled(false);
        ui->dsbFrameRate->setEnabled(true);
        ui->sbSampleRate->setEnabled(true);
    }
#endif
}
