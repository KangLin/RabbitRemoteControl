// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QApplication>
#include "ScreenCapture.h"
#include "DlgCapture.h"
#include "RabbitCommonTools.h"
#include "PluginClient.h"

static Q_LOGGING_CATEGORY(log, "Screen.Capture.Connect")
CScreenCapture::CScreenCapture(CPluginClient *plugin)
    : CConnecter(plugin)
    , m_pWidget(new QVideoWidget())
{
    bool check = false;
    SetParameter(&m_Parameter);

    QString szTitle(plugin->DisplayName());
    m_Menu.setTitle(szTitle);
    m_Menu.setToolTip(szTitle);
    m_Menu.setStatusTip(szTitle);
    m_Menu.setWhatsThis(szTitle);
    m_Menu.setIcon(plugin->Icon());
    check = m_Menu.addAction(
        QIcon::fromTheme("media-playback-start"), tr("Start"),
        this, SLOT(slotStart()));
    Q_ASSERT(check);
    check = m_Menu.addAction(
        QIcon::fromTheme("media-playback-stop"), tr("Stop"),
        this, SLOT(slotStop()));
    Q_ASSERT(check);

    check = connect(
        &m_ScreenCapture, &QScreenCapture::errorOccurred,
        this, [&](QScreenCapture::Error error, const QString &errorString){
            qDebug(log) << "Capture screen error occurred:" << error << errorString;
            slotStop();
            emit sigError(error, errorString);
        });
    Q_ASSERT(check);
    check = connect(
        &m_WindowCapture, &QWindowCapture::errorOccurred,
        this, [&](QWindowCapture::Error error, const QString &errorString){
            qDebug(log) << "Capture windows error occurred:" << error << errorString;
            slotStop();
            emit sigError(error, errorString);
        });
    Q_ASSERT(check);
    check = connect(
        &m_ImageCapture, &QImageCapture::imageSaved,
        this, [&](int id, const QString &fileName) {
            qDebug(log) << "Capture image to file:" << fileName;
        });
    Q_ASSERT(check);
    check = connect(
        &m_Recorder, &QMediaRecorder::errorOccurred,
        this, [&](QMediaRecorder::Error error, const QString &errorString) {
            qDebug(log) << "Recorder error occurred:" << error << errorString;
            slotStop();
            emit sigError(error, errorString);
        });
    Q_ASSERT(check);
    check = connect(&m_Recorder, &QMediaRecorder::recorderStateChanged,
                    this, [&](QMediaRecorder::RecorderState state){
                        qDebug(log) << "Recorder state changed:" << state;
                    });
    Q_ASSERT(check);
    check = connect(&m_Recorder, &QMediaRecorder::actualLocationChanged,
                    this, [&](const QUrl &location){
                        qInfo(log) << "Recorder actual location changed:" << location;
                    });
    Q_ASSERT(check);
    check = connect(
        &m_ImageCapture, &QImageCapture::errorOccurred,
        this, [&](int id, QImageCapture::Error error, const QString &errorString) {
            qDebug(log) << "Capture image error occurred:" << id << error << errorString;
            slotStop();
            emit sigError(error, errorString);
        });
    Q_ASSERT(check);
    check = connect(
        &m_ImageCapture, &QImageCapture::imageCaptured,
        this, [&](int id, const QImage &image){
            qDebug(log) << "Capture image:" << id << image;
            QString szFile = m_Parameter.m_Record.GetImageFile(true);
            if(image.save(szFile, "PNG"))
                qDebug(log) << "Capture image to file:" << szFile;
            else
                qCritical(log) << "Capture image save to file fail." << szFile;
            slotStop();
        });
    Q_ASSERT(check);
}

qint16 CScreenCapture::Version()
{
    return 0;
}

QWidget *CScreenCapture::GetViewer()
{
    return m_pWidget; // new CDlgCapture(&m_Parameter);
}

QDialog *CScreenCapture::OnOpenDialogSettings(QWidget *parent)
{
    return nullptr;
}

int CScreenCapture::Connect()
{
    int nRet = 0;
    slotStart();
    emit sigConnected();
    return nRet;
}

int CScreenCapture::DisConnect()
{
    int nRet = 0;
    slotStop();
    emit sigDisconnected();
    return nRet;
}

QMenu *CScreenCapture::GetMenu(QWidget *parent)
{
    return &m_Menu;
}

int CScreenCapture::slotStart()
{
    qDebug(log) << "CScreenCapture::slotStart()";
    int nRet = 0;
    CDlgCapture dlg(&m_Parameter);
    nRet = RC_SHOW_WINDOW(&dlg);
    if(QDialog::Accepted != nRet)
        return 0;

    int nIndex = m_Parameter.GetScreen();
    if(m_Parameter.GetTarget() == CParameterScreenCapture::TARGET::Screen
        && -1 < nIndex && nIndex < QApplication::screens().size()) {
        m_ScreenCapture.setScreen(QApplication::screens().at(nIndex));
        m_CaptureSessioin.setScreenCapture(&m_ScreenCapture);
        m_ScreenCapture.start();
    }else
        m_CaptureSessioin.setScreenCapture(nullptr);

    if(m_Parameter.GetTarget() == CParameterScreenCapture::TARGET::Window) {
        m_CaptureSessioin.setWindowCapture(&m_WindowCapture);
        m_WindowCapture.setWindow(m_Parameter.GetWindow());
        m_WindowCapture.start();
    } else
        m_CaptureSessioin.setWindowCapture(nullptr);

    if(m_Parameter.GetOperate() == CParameterScreenCapture::OPERATE::Record) {
        m_CaptureSessioin.setRecorder(&m_Recorder);
        m_Parameter.m_Record >> m_Recorder;
        m_Recorder.record();
        qDebug(log) << "Record to file:" << m_Recorder.actualLocation();
    }
    else
        m_CaptureSessioin.setRecorder(nullptr);

    if(m_Parameter.GetOperate() == CParameterScreenCapture::OPERATE::Shot) {
        m_CaptureSessioin.setImageCapture(&m_ImageCapture);
        //m_ImageCapture.captureToFile(m_Parameter.m_Record.GetUrl());
        m_ImageCapture.capture();
    } else
        m_CaptureSessioin.setImageCapture(nullptr);

    if(m_pWidget)
        m_CaptureSessioin.setVideoOutput(m_pWidget);

    return 0;
}

int CScreenCapture::slotStop()
{
    qDebug(log) << "CScreenCapture::slotStop()";
    if(m_Parameter.GetTarget() == CParameterScreenCapture::TARGET::Screen)
        m_ScreenCapture.stop();
    if(m_Parameter.GetTarget() == CParameterScreenCapture::TARGET::Window)
        m_WindowCapture.stop();

    if(m_Parameter.GetOperate() == CParameterScreenCapture::OPERATE::Record)
        m_Recorder.stop();

    return 0;
}
