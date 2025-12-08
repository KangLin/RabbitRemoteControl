// Author: Kang Lin <kl222@126.com>

#pragma once

#if HAVE_QT6_MULTIMEDIA
    #include <QMediaFormat>
#endif
#include <QMediaRecorder>

#include "Parameter.h"

class PLUGIN_EXPORT CParameterRecord : public CParameter
{
    Q_OBJECT

public:
    explicit CParameterRecord(QObject *parent = nullptr,
                              const QString& szPrefix = QString());

    const QString GetVideoPath() const;
    int SetVideoPath(const QString& szPath);

    /*! \param bAuto:
     *         - true: Automatically generated
     *         - false: use the value of SetFile()
     *  \param ext: filename extension
     */
    const QString GetVideoFile(bool bAuto = false);
    int SetVideoFile(const QString& szFile);

    QString GetImageFile(bool bAuto = false);
    void SetImageFile(const QString &newImageFile);

    QString GetImagePath() const;
    void SetImagePath(const QString &newImagePath);

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
    
    QSize GetVideoResolution();
    void SetVideoResolution(QSize size);
    int GetVideoBitRate() const;
    void SetVideoBitRate(int bitRate);
    qreal GetVideoFrameRate() const;
    void SetVideoFrameRate(qreal newVideoFrameRate);
    int GetAudioBitRate();
    void SetAudioBitRate(int bitRate);
    int GetAudioSampleRate() const;
    void SetAudioSampleRate(int newAudioSampleRate);
    int GetAudioChannelCount();
    void SetAudioChannelCount(int count);

    virtual CParameterRecord& operator=(const CParameterRecord &in);

    bool GetEnableVideo() const;
    void SetEnableVideo(bool newEnableVideo);
    bool GetEnableAudio() const;
    void SetEnableAudio(bool newEnableAudio);

    enum ENDACTION {
        No,
        OpenFile,
        OpenFolder
    };
    Q_ENUM(ENDACTION)

    CParameterRecord::ENDACTION GetEndAction() const;
    void SetEndAction(ENDACTION newEndAction);

Q_SIGNALS:
    void sigQualityChanged();
    void sigEncodingModeChanged();
    void sigImageFileChanged();
    void sigEnableVideoChanged();
    void sigEnableAudioChanged();
    void sigEndActionChanged();

    void ImagePathChanged();

private:
    bool m_bEnableVideo;
    bool m_bEnableAudio;
    QString m_szVideoFile;
    QString m_szVideoPath;
    QString m_szImageFile;
    QString m_szImagePath;

#if HAVE_QT6_MULTIMEDIA
    QMediaFormat::FileFormat m_FileFormat;
    QMediaFormat::VideoCodec m_VideoCodec;
    QMediaFormat::AudioCodec m_AudioCodec;

    QMediaRecorder::Quality m_Quality;
    QMediaRecorder::EncodingMode m_EncodingMode;
#endif

    QSize m_VideoResolution;
    int m_VideoBitRate;
    qreal m_VideoFrameRate;
    int m_AudioBitRate;
    int m_AudioSampleRate;
    int m_AudioChannelCount;

    ENDACTION m_EndAction;

protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};

PLUGIN_EXPORT CParameterRecord& operator << (CParameterRecord& para, QMediaRecorder& recorder);
PLUGIN_EXPORT CParameterRecord& operator << (CParameterRecord& para, QMediaRecorder& recorder);
PLUGIN_EXPORT CParameterRecord& operator >> (CParameterRecord& para, QMediaRecorder& recorder);
