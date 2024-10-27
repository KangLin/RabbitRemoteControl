// Author: Kang Lin <kl222@126.com>

#include <QDir>
#include <QDateTime>

#include "ParameterRecord.h"
#include "RabbitCommonDir.h"

CParameterRecord::CParameterRecord(QObject *parent, const QString &szPrefix)
    : CParameter(parent, szPrefix)
    , m_bEnable(false)
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
{
    m_szPath = RabbitCommon::CDir::Instance()->GetDirUserImage()
        + QDir::separator()
        + "ScreenShot";
    QDir d(m_szPath);
    if(!d.exists())
        d.mkpath(m_szPath);
}

CParameterRecord& CParameterRecord::operator =(const CParameterRecord& in)
{
    if(this == &in)
        return *this;
    m_bEnable = in.m_bEnable;
    m_szFile = in.m_szFile;
    m_szImageFile = in.m_szImageFile;
    m_szPath = in.m_szPath;
#if HAVE_QT6_MULTIMEDIA
    m_FileFormat = in.m_FileFormat;
    m_VideoCodec = in.m_VideoCodec;
    m_AudioCodec = in.m_AudioCodec;
    m_Quality = in.m_Quality;
    m_EncodingMode = in.m_EncodingMode;
#endif
    m_VideoFrameRate = in.m_VideoFrameRate;
    m_AudioSampleRate = in.m_AudioSampleRate;
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

int CParameterRecord::OnLoad(QSettings &set)
{
    set.beginGroup("Record");

    SetEnable(set.value("Enable", GetEnable()).toBool());
    SetEnableVideo(set.value("EnableVideo", GetEnableVideo()).toBool());
    SetEnableAudio(set.value("EnableAudio", GetEnableAudio()).toBool());
    SetPath(set.value("Path", GetPath()).toString());
#if HAVE_QT6_MULTIMEDIA
    SetFileFormat((QMediaFormat::FileFormat)
                  set.value("FileFormat", (int)GetFileFormat()).toInt());
    SetVideoCodec((QMediaFormat::VideoCodec)
                  set.value("VideoCodec", (int)GetVideoCodec()).toInt());
    SetAudioCodec((QMediaFormat::AudioCodec)
                  set.value("AudioCodec", (int)GetAudioCodec()).toInt());
    SetQuality((QMediaRecorder::Quality)
               set.value("Quality", GetQuality()).toInt());
    SetEncodingMode((QMediaRecorder::EncodingMode)
                    set.value("EncodingMode", GetEncodingMode()).toInt());
#endif

    set.endGroup();
    return 0;
}

int CParameterRecord::OnSave(QSettings &set)
{
    set.beginGroup("Record");

    set.setValue("Enable", GetEnable());
    set.setValue("EnableVideo", GetEnableVideo());
    set.setValue("EnableAudio", GetEnableAudio());
    set.setValue("Path", GetPath());
#if HAVE_QT6_MULTIMEDIA
    set.setValue("FileFormat", (int)GetFileFormat());
    set.setValue("VideoCodec", (int)GetVideoCodec());
    set.setValue("AudioCodec", (int)GetAudioCodec());
    set.setValue("Quality", (int)GetQuality());
    set.setValue("EncodingMode", GetEncodingMode());
#endif

    set.endGroup();
    return 0;
}

const bool CParameterRecord::GetEnable() const
{
    return m_bEnable;
}

int CParameterRecord::SetEnable(bool bEnable)
{
    if(m_bEnable == bEnable)
        return 0;
    m_bEnable = bEnable;
    SetModified(true);
    return 0;
}

const QString CParameterRecord::GetFile(bool bAuto)
{
    if(bAuto) {
        m_szFile = GetPath()
            + QDir::separator()
            + QDateTime::currentDateTime().toLocalTime()
                  .toString("yyyy_MM_dd_hh_mm_ss_zzz");
    }
    return m_szFile;
}

int CParameterRecord::SetFile(const QString &szFile)
{
    if(m_szFile == szFile)
        return 0;
    SetModified(true);
    m_szFile = szFile;
    return 0;
}

QString CParameterRecord::GetImageFile(bool bAuto)
{
    if(bAuto) {
        m_szImageFile = GetPath()
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

const QString CParameterRecord::GetPath() const
{
    return m_szPath;
}

int CParameterRecord::SetPath(const QString &szPath)
{
    if(m_szPath == szPath)
        return 0;
    m_szPath = szPath;
    SetModified(true);
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
    para.SetFile(recorder.outputLocation().toString());
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
    recorder.setEncodingMode(para.GetEncodingMode());
    recorder.setQuality(para.GetQuality());
#endif

    if(!para.GetFile(true).isEmpty())
        recorder.setOutputLocation(QUrl::fromLocalFile(para.GetFile(true)));
    return para;
}
