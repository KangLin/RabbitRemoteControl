// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QApplication>
#include <QDesktopServices>
#include "ScreenCapture.h"
#include "DlgCapture.h"
#include "RabbitCommonTools.h"
#include "PluginClient.h"

static Q_LOGGING_CATEGORY(log, "Screen.Capture")
CScreenCapture::CScreenCapture(CPluginClient *plugin)
    : CConnecter(plugin)
    , m_pWidget(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
}

CScreenCapture::~CScreenCapture()
{
    qDebug(log) << Q_FUNC_INFO;
}

qint16 CScreenCapture::Version()
{
    return 0;
}

int CScreenCapture::Initial()
{
    qDebug(log) << Q_FUNC_INFO;
    Q_ASSERT(!m_pWidget);
    m_pWidget = new QVideoWidget();
    int nRet = SetParameter(&m_Parameter);
    if(nRet) return nRet;
    bool check = false;

    CPluginClient* plugin = GetPlugClient();
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
        &m_Recorder, &QMediaRecorder::errorOccurred,
        this, [&](QMediaRecorder::Error error, const QString &errorString) {
            qDebug(log) << "Recorder error occurred:" << error << errorString;
            slotStop();
            emit sigError(error, errorString);
        });
    Q_ASSERT(check);
    check = connect(
        &m_Recorder, &QMediaRecorder::recorderStateChanged,
        this, [&](QMediaRecorder::RecorderState state){
            qDebug(log) << "Recorder state changed:" << state;
            if(QMediaRecorder::StoppedState == state)
                switch(m_Parameter.m_Record.GetEndAction())
                {
                case CParameterRecord::ENDACTION::OpenFile:
                    QDesktopServices::openUrl(QUrl::fromLocalFile(m_szRecordFile));
                    break;
                case CParameterRecord::ENDACTION::OpenFolder: {
                    QFileInfo fi(m_szRecordFile);
                    QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absolutePath()));
                    break;
                }
                default:
                    break;
                }
        });
    Q_ASSERT(check);
    check = connect(
        &m_Recorder, &QMediaRecorder::actualLocationChanged,
        this, [&](const QUrl &location){
            qInfo(log) << "Recorder actual location changed:" << location;
            m_szRecordFile = location.toLocalFile();
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
            if(!image.save(szFile, "PNG"))
            {
                qCritical(log) << "Capture image save to file fail." << szFile;
                slotStop();
                return;
            }
            qDebug(log) << "Capture image to file:" << szFile;
            switch(m_Parameter.m_Record.GetEndAction())
            {
            case CParameterRecord::ENDACTION::OpenFile: {
                bool bRet = QDesktopServices::openUrl(QUrl::fromLocalFile(szFile));
                if(!bRet)
                    qCritical(log) << "Fail: Open capture image file" << szFile;
                break;
            }
            case CParameterRecord::ENDACTION::OpenFolder: {
                QFileInfo fi(szFile);
                QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absolutePath()));
                break;
            }
            default:
                break;
            }
        });
    Q_ASSERT(check);
    return 0;
}

int CScreenCapture::Clean()
{
    if(m_pWidget)
        delete m_pWidget;
    return 0;
}

QWidget *CScreenCapture::GetViewer()
{
    return m_pWidget;
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
