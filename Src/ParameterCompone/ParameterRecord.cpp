// Author: Kang Lin <kl222@126.com>

#include <QDir>
#include <QDateTime>
#include <QStandardPaths>

#include "ParameterRecord.h"
#include "RabbitCommonDir.h"

CParameterRecord::CParameterRecord(QObject *parent, const QString &szPrefix)
    : CParameter(parent, szPrefix)
    , m_bEnableVideo(true)
    , m_bEnableAudio(true)
#if HAVE_QT6_MULTIMEDIA
    , m_FileFormat(QMediaFormat::FileFormat::MPEG4)
    , m_VideoCodec(QMediaFormat::VideoCodec::H264)
    , m_AudioCodec(QMediaFormat::AudioCodec::AAC)
    , m_Quality(QMediaRecorder::NormalQuality)
    , m_EncodingMode(QMediaRecorder::ConstantQualityEncoding)
#endif
    , m_VideoFrameRate(0)
    , m_AudioSampleRate(-1)
    , m_EndAction(ENDACTION::No)
{
    m_szVideoPath = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    QDir d;
    if(m_szVideoPath.isEmpty() || !d.exists(m_szVideoPath))
        m_szVideoPath = RabbitCommon::CDir::Instance()->GetDirUserImage();
    m_szVideoPath += QDir::separator() + QString("RabbitRemoteControl")
                + QDir::separator() + QString("Record");

    if(!d.exists(m_szVideoPath))
        d.mkpath(m_szVideoPath);

    m_szImagePath = QStandardPaths::writableLocation(
        QStandardPaths::PicturesLocation);
    if(m_szImagePath.isEmpty() || !d.exists(m_szImagePath))
        m_szImagePath = RabbitCommon::CDir::Instance()->GetDirUserImage();
    m_szImagePath += QDir::separator() + QString("RabbitRemoteControl")
                     + QDir::separator() + QString("ScreenShot");
    if(!d.exists(m_szImagePath))
        d.mkpath(m_szImagePath);
}

CParameterRecord& CParameterRecord::operator =(const CParameterRecord& in)
{
    if(this == &in)
        return *this;
    m_bEnableVideo = in.m_bEnableVideo;
    m_bEnableAudio = in.m_bEnableAudio;
    m_szVideoFile = in.m_szVideoFile;
    m_szImageFile = in.m_szImageFile;
    m_szVideoPath = in.m_szVideoPath;
    m_szImagePath = in.m_szImagePath;

#if HAVE_QT6_MULTIMEDIA
    m_FileFormat = in.m_FileFormat;
    m_VideoCodec = in.m_VideoCodec;
    m_AudioCodec = in.m_AudioCodec;
    m_Quality = in.m_Quality;
    m_EncodingMode = in.m_EncodingMode;
#endif

    m_VideoFrameRate = in.m_VideoFrameRate;
    m_AudioSampleRate = in.m_AudioSampleRate;

    m_EndAction = in.m_EndAction;

    return *this;
}

bool CParameterRecord::GetEnableVideo() const
{
    return m_bEnableVideo;
}

void CParameterRecord::SetEnableVideo(bool newEnableVideo)
{
    if (m_bEnableVideo == newEnableVideo)
        return;
    m_bEnableVideo = newEnableVideo;
    SetModified(true);
    emit sigEnableVideoChanged();
}

bool CParameterRecord::GetEnableAudio() const
{
    return m_bEnableAudio;
}

void CParameterRecord::SetEnableAudio(bool newEnableAudio)
{
    if (m_bEnableAudio == newEnableAudio)
        return;
    m_bEnableAudio = newEnableAudio;
    SetModified(true);
    emit sigEnableAudioChanged();
}

CParameterRecord::ENDACTION CParameterRecord::GetEndAction() const
{
    return m_EndAction;
}

void CParameterRecord::SetEndAction(ENDACTION newEndAction)
{
    if (m_EndAction == newEndAction)
        return;
    m_EndAction = newEndAction;
    SetModified(true);
    emit sigEndActionChanged();
}

QString CParameterRecord::GetImagePath() const
{
    return m_szImagePath;
}

void CParameterRecord::SetImagePath(const QString &newImagePath)
{
    if (m_szImagePath == newImagePath)
        return;
    m_szImagePath = newImagePath;
    SetModified(true);
    QDir d(m_szImagePath);
    if(!d.exists())
        d.mkpath(m_szImagePath);
    emit ImagePathChanged();
}

int CParameterRecord::OnLoad(QSettings &set)
{
    set.beginGroup("Record");

    SetEnableVideo(set.value("EnableVideo", GetEnableVideo()).toBool());
    SetEnableAudio(set.value("EnableAudio", GetEnableAudio()).toBool());
    SetVideoPath(set.value("Path/Video", GetVideoPath()).toString());
    SetImagePath(set.value("Path/Image", GetImagePath()).toString());
#if HAVE_QT6_MULTIMEDIA
    SetFileFormat((QMediaFormat::FileFormat)
                  set.value("FileFormat", (int)GetFileFormat()).toInt());
    SetVideoCodec((QMediaFormat::VideoCodec)
                  set.value("Video/Codec", (int)GetVideoCodec()).toInt());
    SetAudioCodec((QMediaFormat::AudioCodec)
                  set.value("Audio/Codec", (int)GetAudioCodec()).toInt());
    SetQuality((QMediaRecorder::Quality)
               set.value("Quality", GetQuality()).toInt());
    SetEncodingMode((QMediaRecorder::EncodingMode)
                    set.value("EncodingMode", GetEncodingMode()).toInt());
#endif
    SetAudioSampleRate(set.value("Audio/SampleRate", GetAudioSampleRate()).toInt());
    SetVideoFrameRate(set.value("Video/FrameRate", GetVideoFrameRate()).toInt());
    SetEndAction((ENDACTION)set.value("EndAction", GetEndAction()).toInt());
    set.endGroup();
    return 0;
}

int CParameterRecord::OnSave(QSettings &set)
{
    set.beginGroup("Record");

    set.setValue("EnableVideo", GetEnableVideo());
    set.setValue("EnableAudio", GetEnableAudio());
    set.setValue("Path/Video", GetVideoPath());
    set.setValue("Path/Image", GetImagePath());
#if HAVE_QT6_MULTIMEDIA
    set.setValue("FileFormat", (int)GetFileFormat());
    set.setValue("Video/Codec", (int)GetVideoCodec());
    set.setValue("Audio/Codec", (int)GetAudioCodec());
    set.setValue("Quality", (int)GetQuality());
    set.setValue("EncodingMode", GetEncodingMode());
#endif
    set.setValue("Audio/SampleRate", GetAudioSampleRate());
    set.setValue("Video/FrameRate", GetVideoFrameRate());
    set.setValue("EndAction", (int)GetEndAction());
    set.endGroup();
    return 0;
}

const QString CParameterRecord::GetVideoFile(bool bAuto)
{
    if(bAuto)
        m_szVideoFile = GetVideoPath() + QDir::separator()
                   + QDateTime::currentDateTime().toLocalTime()
                         .toString("yyyy_MM_dd_hh_mm_ss_zzz");

    return m_szVideoFile;
}

int CParameterRecord::SetVideoFile(const QString &szFile)
{
    if(m_szVideoFile == szFile)
        return 0;
    SetModified(true);
    m_szVideoFile = szFile;
    return 0;
}

QString CParameterRecord::GetImageFile(bool bAuto)
{
    QString szPath = GetImagePath();
    if(bAuto) {
        m_szImageFile = szPath
                        + QDir::separator()
                        + QDateTime::currentDateTime().toLocalTime()
                              .toString("yyyy_MM_dd_hh_mm_ss_zzz") + ".png";
    }
    return m_szImageFile;
}

void CParameterRecord::SetImageFile(const QString &newImageFile)
{
    if (m_szImageFile == newImageFile)
        return;
    m_szImageFile = newImageFile;
    SetModified(true);
    emit sigImageFileChanged();
}

const QString CParameterRecord::GetVideoPath() const
{
    return m_szVideoPath;
}

int CParameterRecord::SetVideoPath(const QString &szPath)
{
    if(m_szVideoPath == szPath)
        return 0;
    m_szVideoPath = szPath;
    SetModified(true);
    QDir d(m_szVideoPath);
    if(!d.exists())
        d.mkpath(m_szVideoPath);
    return 0;
}

#if HAVE_QT6_MULTIMEDIA
const QMediaFormat::FileFormat CParameterRecord::GetFileFormat() const
{
    return m_FileFormat;
}

int CParameterRecord::SetFileFormat(const QMediaFormat::FileFormat &f)
{
    if(m_FileFormat == f)
        return 0;
    m_FileFormat = f;
    SetModified(true);
    return 0;
}

const QMediaFormat::VideoCodec CParameterRecord::GetVideoCodec() const
{
    return m_VideoCodec;
}

int CParameterRecord::SetVideoCodec(QMediaFormat::VideoCodec v)
{
    if(m_VideoCodec == v)
        return 0;
    m_VideoCodec = v;
    SetModified(true);
    return 0;
}

const QMediaFormat::AudioCodec CParameterRecord::GetAudioCodec() const
{
    return m_AudioCodec;
}

int CParameterRecord::SetAudioCodec(QMediaFormat::AudioCodec a)
{
    if(m_AudioCodec == a)
        return 0;
    m_AudioCodec = a;
    SetModified(true);
    return 0;
}

QMediaRecorder::Quality CParameterRecord::GetQuality() const
{
    return m_Quality;
}

void CParameterRecord::SetQuality(const QMediaRecorder::Quality &newQuality)
{
    if (m_Quality == newQuality)
        return;
    m_Quality = newQuality;
    SetModified(true);
    emit sigQualityChanged();
}

QMediaRecorder::EncodingMode CParameterRecord::GetEncodingMode() const
{
    return m_EncodingMode;
}

void CParameterRecord::SetEncodingMode(const QMediaRecorder::EncodingMode &newEncodingMode)
{
    if (m_EncodingMode == newEncodingMode)
        return;
    m_EncodingMode = newEncodingMode;
    SetModified(true);
    emit sigEncodingModeChanged();
}
#endif

qreal CParameterRecord::GetVideoFrameRate() const
{
    return m_VideoFrameRate;
}

void CParameterRecord::SetVideoFrameRate(qreal newVideoFrameRate)
{
    if(m_VideoFrameRate == newVideoFrameRate)
        return;
    m_VideoFrameRate = newVideoFrameRate;
    SetModified(true);
    return;
}

int CParameterRecord::GetAudioSampleRate() const
{
    return m_AudioSampleRate;
}

void CParameterRecord::SetAudioSampleRate(int newAudioSampleRate)
{
    if(m_AudioSampleRate == newAudioSampleRate)
        return;
    m_AudioSampleRate = newAudioSampleRate;
    SetModified(true);
    return;
}

CParameterRecord& operator << (CParameterRecord& para, QMediaRecorder& recorder)
{
#if HAVE_QT6_MULTIMEDIA
    para.SetVideoFrameRate(recorder.videoFrameRate());
    para.SetAudioSampleRate(recorder.audioSampleRate());
    QMediaFormat format;
    format = recorder.mediaFormat();
    para.SetAudioCodec(format.audioCodec());
    para.SetVideoCodec(format.videoCodec());
    para.SetFileFormat(format.fileFormat());
    para.SetQuality(recorder.quality());
    para.SetEncodingMode(recorder.encodingMode());
#endif
    para.SetVideoFile(recorder.outputLocation().toString());
    return para;
}

CParameterRecord& operator >> (CParameterRecord& para, QMediaRecorder& recorder)
{
#if HAVE_QT6_MULTIMEDIA
    recorder.setVideoFrameRate(para.GetVideoFrameRate());
    recorder.setAudioSampleRate(para.GetAudioSampleRate());
    QMediaFormat format;
    format.setAudioCodec(para.GetAudioCodec());
    format.setVideoCodec(para.GetVideoCodec());
    format.setFileFormat(para.GetFileFormat());
    recorder.setMediaFormat(format);
    recorder.setQuality(para.GetQuality());
    recorder.setEncodingMode(para.GetEncodingMode());
#endif

    if(!para.GetVideoFile(true).isEmpty())
        recorder.setOutputLocation(QUrl::fromLocalFile(para.GetVideoFile(true)));
    return para;
}
