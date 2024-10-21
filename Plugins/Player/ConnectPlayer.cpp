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
    check = connect(
        &m_VideoSink, &QVideoSink::videoFrameChanged,
        this, [&](const QVideoFrame &frame){
            if(m_Video.width() != frame.width()
                || m_Video.height() != frame.height())
            {
                m_Video = QRect(0, 0, frame.width(), frame.height());
                emit sigSetDesktopSize(m_Video.width(), m_Video.height());
            }
            QImage img(frame.width(), frame.height(), QImage::Format_ARGB32);
            QPainter painter(&img);
            const QVideoFrame::PaintOptions option;
            QVideoFrame f = frame;
            f.paint(&painter, m_Video, option);
            qDebug(log) << "QVideoSink::videoFrameChanged" << frame << img;
            emit this->sigUpdateRect(img);
        });
    Q_ASSERT(check);
    check = connect(pConnecter, SIGNAL(sigStart()), this, SLOT(slotStart()));
    Q_ASSERT(check);
    check = connect(pConnecter, SIGNAL(sigStop()), this , SLOT(slotStop()));
    Q_ASSERT(check);

    check = connect(m_pParameters, &CParameterPlayer::sigEnableAudioInput,
                    this, &CConnectPlayer::slotEnableAudioInput);
    Q_ASSERT(check);
    check = connect(m_pParameters, &CParameterPlayer::sigEnableAudioOutput,
                    this, &CConnectPlayer::slotEnableAudioOutput);
    Q_ASSERT(check);
}

CConnectPlayer::~CConnectPlayer()
{
    qDebug(log) << "CConnectPlayer::~CConnectPlayer()";
}

CConnect::OnInitReturnValue CConnectPlayer::OnInit()
{
    slotStart();
    emit sigConnected();
    return OnInitReturnValue::NotUseOnProcess;
}

void CConnectPlayer::slotStart()
{
    slotEnableAudioInput(m_pParameters->GetEnableAudioInput());
    slotEnableAudioOutput(m_pParameters->GetEnableAudioOutput());

    switch (m_pParameters->GetType()) {
    case CParameterPlayer::TYPE::Camera: {
        if(!m_pCamera) {
            const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
            if(cameras.isEmpty()
                || -1 > m_pParameters->GetCamera()
                || m_pParameters->GetCamera() > QMediaDevices::videoInputs().size())
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
        bool check = connect(
            &m_Player, &QMediaPlayer::positionChanged,
            this, [&](qint64 position) {
                emit sigInformation(tr("Position: ")
                                    + QString::number(position));
            });
        Q_ASSERT(check);
        m_Player.play();
        break;
    }
    case CParameterPlayer::TYPE::CaptureScreen: {
        int nIndex = m_pParameters->GetScreen();
        if(nIndex < 0 || nIndex > QGuiApplication::screens().size()) {
            qCritical(log) << "The screen out of range. nIndex:" << nIndex
                           << "Screens:" << QGuiApplication::screens().size();
            break;
        }
        QScreen* pScreen = QGuiApplication::screens().at(nIndex);
        m_ScreenCapture.setScreen(pScreen);
        m_CaptureSessioin.setScreenCapture(&m_ScreenCapture);
        m_CaptureSessioin.setVideoSink(&m_VideoSink);
        m_ScreenCapture.start();
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
    case CParameterPlayer::TYPE::CaptureScreen:
        m_ScreenCapture.stop();
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

void CConnectPlayer::slotEnableAudioInput(bool bEnable)
{
    if(bEnable && -1 < m_pParameters->GetAudioInput()
        && m_pParameters->GetAudioInput() < QMediaDevices::audioInputs().size()) {
        m_AudioInput.setDevice(QMediaDevices::audioInputs()
                                   .at(m_pParameters->GetAudioInput()));
        m_CaptureSessioin.setAudioInput(&m_AudioInput);

        bool check = connect(m_pParameters,
                             &CParameterPlayer::sigAudioInputMuted,
                             &m_AudioInput, &QAudioInput::setMuted);
        Q_ASSERT(check);
        check = connect(m_pParameters, &CParameterPlayer::sigAudioInputVolume,
                        &m_AudioInput, &QAudioInput::setVolume);
        Q_ASSERT(check);
        check = connect(m_pParameters, &CParameterPlayer::sigAudioInput,
                        this, [&](int nIndex) {
                            if(-1 < nIndex
                                && nIndex < QMediaDevices::audioInputs().size())
                                m_AudioInput.setDevice(
                                    QMediaDevices::audioInputs().at(nIndex));
                        });
    } else {
        m_CaptureSessioin.setAudioInput(nullptr);
    }
}

void CConnectPlayer::slotEnableAudioOutput(bool bEnable)
{
    if(bEnable &&  (-1 < m_pParameters->GetAudioOutput()
                    && m_pParameters->GetAudioOutput()
                           < QMediaDevices::audioInputs().size()))
    {
        m_AudioOutput.setDevice(
            QMediaDevices::audioOutputs()
                .at(m_pParameters->GetAudioOutput()));

        m_AudioOutput.disconnect();
        bool check = connect(m_pParameters,
                             &CParameterPlayer::sigAudioOutputMuted,
                             &m_AudioOutput, &QAudioOutput::setMuted);
        Q_ASSERT(check);
        check = connect(m_pParameters, &CParameterPlayer::sigAudioOutputVolume,
                        &m_AudioOutput, &QAudioOutput::setVolume);
        Q_ASSERT(check);
        check = connect(m_pParameters, &CParameterPlayer::sigAudioOutput,
                        this, [&](int nIndex) {
                            if(-1 < nIndex
                                && nIndex < QMediaDevices::audioOutputs().size())
                                m_AudioOutput.setDevice(
                                    QMediaDevices::audioOutputs().at(nIndex));
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
