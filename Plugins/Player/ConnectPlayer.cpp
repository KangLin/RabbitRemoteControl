// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QMediaDevices>
#include <QPainter>
#include <QAudioDevice>

#include "ConnectPlayer.h"

static Q_LOGGING_CATEGORY(log, "Player.Connect")

CConnectPlayer::CConnectPlayer(CConneterPlayer* pConnecter)
    : CConnectDesktop(pConnecter)
    , m_pCamera(nullptr)
{
    m_pParameters = qobject_cast<CParameterPlayer*>(pConnecter->GetParameter());
    bool check = false;
    check = connect(&m_VideoSink, &QVideoSink::videoFrameChanged,
                    this, [&](const QVideoFrame &frame){
                        //qDebug(log) << "QVideoSink::videoFrameChanged" << frame;
                        QRect rect(0, 0, frame.width(), frame.height());
                        QImage img(frame.width(), frame.height(), QImage::Format_ARGB32);
                        QPainter painter(&img);
                        const QVideoFrame::PaintOptions option;
                        QVideoFrame f = frame;
                        f.paint(&painter, rect, option);
                        emit this->sigUpdateRect(img);
                    });
    Q_ASSERT(check);
    check = connect(pConnecter, SIGNAL(sigStart()), this, SLOT(slotStart()));
    Q_ASSERT(check);
    check = connect(pConnecter, SIGNAL(sigStop()), this , SLOT(slotStop()));
    Q_ASSERT(check);
}

CConnectPlayer::~CConnectPlayer()
{
}

CConnect::OnInitReturnValue CConnectPlayer::OnInit()
{
    slotStart();
    emit sigConnected();
    return OnInitReturnValue::NotUseOnProcess;
};

void CConnectPlayer::slotStart()
{
    if(m_pParameters->GetEnableAudioInput()) {
        m_AudioInput.setDevice(QMediaDevices::audioInputs().at(m_pParameters->GetAudioInput()));
        m_CaptureSessioin.setAudioInput(&m_AudioInput);
    }

    bool check = false;
    check = connect(m_pParameters, &CParameterPlayer::sigEnableAudioOutput,
                    this, &CConnectPlayer::slotEnableAudioOutput);
    Q_ASSERT(check);
    slotEnableAudioOutput(m_pParameters->GetEnableAudioOutput());

    switch (m_pParameters->GetType()) {
    case CParameterPlayer::TYPE::Camera: {
        if(!m_pCamera) {
            const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
            if(cameras.isEmpty())
                break;
            m_pCamera = new QCamera(cameras.at(m_pParameters->GetCamera()));
        }
        if(m_pCamera) {
            m_CaptureSessioin.setVideoSink(&m_VideoSink);
            m_CaptureSessioin.setCamera(m_pCamera);
            m_pCamera->start();
        }
        break;
    }
    case CParameterPlayer::TYPE::Url: {
        m_Player.setSource(QUrl::fromLocalFile(m_pParameters->GetUrl()));
        m_Player.setVideoSink(&m_VideoSink);
        bool check = connect(&m_Player, &QMediaPlayer::positionChanged,
                             this, [&](qint64 position) {
            emit sigInformation(tr("Postion: ") + QString::number(position));
        });
        Q_ASSERT(check);
        m_Player.play();
        break;
    }
    default:
        break;
    }
}

int CConnectPlayer::OnClean()
{
    slotStop();
    emit sigDisconnected();
    return 0;
}

void CConnectPlayer::slotStop()
{
    switch (m_pParameters->GetType()) {
    case CParameterPlayer::TYPE::Camera:
        if(m_pCamera)
            m_pCamera->stop();
        break;
    case CParameterPlayer::TYPE::Url:
        m_Player.stop();
        break;
    }

    if(m_pCamera) {
        delete m_pCamera;
        m_pCamera = nullptr;
    }
}

void CConnectPlayer::slotClipBoardChanged()
{
}

void CConnectPlayer::slotEnableAudioOutput(bool bEnable)
{
    if(bEnable) {
        m_AudioOutput.setDevice(
            QMediaDevices::audioOutputs().at(m_pParameters->GetAudioOutput()));
        bool check = connect(m_pParameters,
                             &CParameterPlayer::sigAudioOutputMuted,
                             &m_AudioOutput, &QAudioOutput::setMuted);
        Q_ASSERT(check);
        check = connect(m_pParameters, &CParameterPlayer::sigAudioOutputVolume,
                        &m_AudioOutput, &QAudioOutput::setVolume);
        Q_ASSERT(check);
        check = connect(m_pParameters, &CParameterPlayer::sigAudioOutput,
                        this, [&](int nIndex) {
                            m_AudioOutput.setDevice(
                                QMediaDevices::audioOutputs()
                                    .at(m_pParameters->GetAudioOutput()));
                        });
        switch (m_pParameters->GetType()) {
        case CParameterPlayer::TYPE::Camera:
            m_CaptureSessioin.setAudioOutput(&m_AudioOutput);
            break;
        case CParameterPlayer::TYPE::Url:
            m_Player.setAudioOutput(&m_AudioOutput);
            break;
        default:
            break;
        }
    } else {
        m_Player.setAudioOutput(nullptr);
        m_CaptureSessioin.setAudioOutput(nullptr);
        m_AudioOutput.disconnect();
    }
}
