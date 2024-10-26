// Author: Kang Lin <kl222@126.com>

#ifndef PARAMETERRECORD_H
#define PARAMETERRECORD_H

#pragma once

#if HAVE_QT6_MULTIMEDIA
    #include <QMediaFormat>
#endif
#include <QMediaRecorder>

#include "Parameter.h"

class CLIENT_EXPORT CParameterRecord : public CParameter
{
    Q_OBJECT

public:
    explicit CParameterRecord(QObject *parent = nullptr,
                              const QString& szPrefix = QString());

    const bool GetEnable() const;
    int SetEnable(bool bEnable);
    /*! \param bAuto:
     *         - true: Automatically generated
     *         - false: use the value of SetFile()
     *  \param ext: filename extension
     */
    const QString GetFile(bool bAuto = false);
    int SetFile(const QString& szFile);

    QString GetImageFile(bool bAuto = false);
    void SetImageFile(const QString &newImageFile);

    const QString GetPath() const;
    int SetPath(const QString& szPath);

#if HAVE_QT6_MULTIMEDIA
    const QMediaFormat::FileFormat GetFileFormat() const;
    int SetFileFormat(const QMediaFormat::FileFormat &f);

    const QMediaFormat::VideoCodec GetVideoCodec() const;
    int SetVideoCodec(QMediaFormat::VideoCodec);

    const QMediaFormat::AudioCodec GetAudioCodec() const;
    int SetAudioCodec(QMediaFormat::AudioCodec);

    QMediaRecorder::Quality GetQuality() const;
    void SetQuality(const QMediaRecorder::Quality &newQuality);
    QMediaRecorder::EncodingMode GetEncodingMode() const;
    void SetEncodingMode(const QMediaRecorder::EncodingMode &newEncodingMode);
#endif

    qreal GetVideoFrameRate() const;
    void SetVideoFrameRate(qreal newVideoFrameRate);
    int GetAudioSampleRate() const;
    void SetAudioSampleRate(int newAudioSampleRate);

    virtual CParameterRecord& operator=(const CParameterRecord &in);

    bool GetEnableVideo() const;
    void SetEnableVideo(bool newEnableVideo);
    bool GetEnableAudio() const;
    void SetEnableAudio(bool newEnableAudio);

Q_SIGNALS:
    void sigQualityChanged();
    void sigEncodingModeChanged();
    void sigImageFileChanged();
    void sigEnableVideoChanged();
    void sigEnableAudioChanged();

private:
    bool m_bEnable;
    bool m_bEnableVideo;
    bool m_bEnableAudio;
    QString m_szFile;
    QString m_szImageFile;
    QString m_szPath;

#if HAVE_QT6_MULTIMEDIA
    QMediaFormat::FileFormat m_FileFormat;
    QMediaFormat::VideoCodec m_VideoCodec;
    QMediaFormat::AudioCodec m_AudioCodec;

    QMediaRecorder::Quality m_Quality;
    QMediaRecorder::EncodingMode m_EncodingMode;
#endif

    qreal m_VideoFrameRate;
    int m_AudioSampleRate;

protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};

CLIENT_EXPORT CParameterRecord& operator << (CParameterRecord& para, QMediaRecorder& recorder);
CLIENT_EXPORT CParameterRecord& operator << (CParameterRecord& para, QMediaRecorder& recorder);
CLIENT_EXPORT CParameterRecord& operator >> (CParameterRecord& para, QMediaRecorder& recorder);

#endif // PARAMETERRECORD_H
