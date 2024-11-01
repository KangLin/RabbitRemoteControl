// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QDesktopServices>
#include <QUrl>

#include <QWidgetAction>
#include <QActionGroup>

#include "ConnecterThread.h"
#include "ConnectThread.h"
#include "PluginClient.h"

static Q_LOGGING_CATEGORY(log, "Client.Connecter.Thread")

CConnecterThread::CConnecterThread(CPluginClient *plugin)
    : CConnecterConnect(plugin)
    , m_pThread(nullptr)
    , m_pView(nullptr)
    , m_pScroll(nullptr)
    , m_psbZoomFactor(nullptr)
{
    bool check = false;

    m_pView = new CFrmViewer();
    m_pScroll = new CFrmScroll(m_pView);

    m_Menu.setIcon(plugin->Icon());
    m_Menu.setTitle(plugin->DisplayName());
    m_Menu.setToolTip(plugin->DisplayName());
    m_Menu.setStatusTip(plugin->DisplayName());
    m_Menu.addAction(QIcon::fromTheme("camera-photo"), tr("ScreenShot"),
                     this, [&](){
                         if(!GetParameter() || !m_pView)
                             return;
                         auto &record = GetParameter()->m_Record;
                         QString szFile = record.GetImageFile(true);
                         bool bRet = m_pView->GrabImage().save(szFile);
                         if(bRet)
                             qDebug(log) << "Success: save screenshot to" << szFile;
                         else
                             qCritical(log) << "Fail: save screenshot to" << szFile;
                         if(record.GetEnable() != CParameterRecord::ENDACTION::No)
                             QDesktopServices::openUrl(QUrl::fromLocalFile(szFile));
                     });
#if HAVE_QT6_RECORD
    QAction* pRecord = m_Menu.addAction(
        QIcon::fromTheme("media-record"), tr("Record"));
    pRecord->setCheckable(true);
    check = connect(pRecord, SIGNAL(toggled(bool)), this, SLOT(slotRecord(bool)));
    Q_ASSERT(check);
#endif
    m_Menu.addSeparator();
    QMenu* pMenuZoom = new QMenu(&m_Menu);
    pMenuZoom->setTitle(tr("Zoom"));
    pMenuZoom->setIcon(QIcon::fromTheme("zoom"));
    m_Menu.addMenu(pMenuZoom);

    QAction* pZoomToWindow = pMenuZoom->addAction(
        QIcon::fromTheme("zoom-fit-best"), tr("Zoom to window"),
        [&](){
            m_pScroll->slotSetAdaptWindows(
                CFrmViewer::ADAPT_WINDOWS::ZoomToWindow);
        });
    QAction* pZoomAspectRatio = pMenuZoom->addAction(
        QIcon::fromTheme("zoom-aspect-ratio"), tr("Keep aspect ration to windows"),
        [&](){
            m_pScroll->slotSetAdaptWindows(
                CFrmViewer::ADAPT_WINDOWS::KeepAspectRationToWindow);
        });
    QAction* pZoomOriginal = pMenuZoom->addAction(
        QIcon::fromTheme("zoom-original"), tr("Original"),
        [&](){
            if(m_psbZoomFactor)
                m_psbZoomFactor->setValue(100);
            m_pScroll->slotSetAdaptWindows(
                CFrmViewer::ADAPT_WINDOWS::Original);
        });
    QAction* pZoomIn = pMenuZoom->addAction(
        QIcon::fromTheme("zoom-in"), tr("Zoom in"),
        [&](){
            if(m_psbZoomFactor)
                m_psbZoomFactor->setValue((m_pView->GetZoomFactor() + 0.1) * 100);
        });
    QAction* pZoomOut = pMenuZoom->addAction(
        QIcon::fromTheme("zoom-out"), tr("Zoom out"),
        [&](){
            if(m_psbZoomFactor)
                m_psbZoomFactor->setValue((m_pView->GetZoomFactor() - 0.1) * 100);
        });

    m_psbZoomFactor = new QSpinBox(pMenuZoom);
    m_psbZoomFactor->setRange(0, 9999999);
    m_psbZoomFactor->setValue(100);
    m_psbZoomFactor->setSuffix("%");
    m_psbZoomFactor->setEnabled(false);
    m_psbZoomFactor->setFocusPolicy(Qt::NoFocus);
    check = connect(
        m_psbZoomFactor, &QSpinBox::valueChanged,
        this, [&](int v)
        {
            if(!m_pScroll || !m_pView) return;
            m_pView->slotSetZoomFactor(((double)v) / 100);
            m_pScroll->slotSetAdaptWindows(CFrmViewer::ADAPT_WINDOWS::Zoom);
        });
    Q_ASSERT(check);
    QWidgetAction* pFactor = new QWidgetAction(pMenuZoom);
    pFactor->setDefaultWidget(m_psbZoomFactor);
    pMenuZoom->insertAction(pZoomOut, pFactor);

    QActionGroup* pGBViewZoom = new QActionGroup(this);
    if(pGBViewZoom) {
        pGBViewZoom->addAction(pZoomToWindow);
        pGBViewZoom->addAction(pZoomAspectRatio);
        pGBViewZoom->addAction(pZoomOriginal);
        pGBViewZoom->addAction(pZoomIn);
        pGBViewZoom->addAction(pZoomOut);
    }
    pZoomToWindow->setChecked(true);
}

CConnecterThread::~CConnecterThread()
{
    qDebug(log) << __FUNCTION__;
    if(m_pScroll)
    {
        delete m_pScroll;
        m_pScroll = nullptr;
    }
}

QWidget *CConnecterThread::GetViewer()
{
    return m_pScroll;
}

int CConnecterThread::Connect()
{
    qDebug(log) << "CConnecterThread::Connect()";
    int nRet = 0;
    m_pThread = new CConnectThread(this);
    if(!m_pThread) {
        qCritical(log) << "new CConnectThread fail";
        return -2;
    }
    
    m_pThread->start();
    
    return nRet;
}

int CConnecterThread::DisConnect()
{
    qDebug(log) << "CConnecterThread::DisConnect()";
    int nRet = 0;
    if(m_pThread)
    {
        m_pThread->quit();
        //Don't delete m_pThread, See CConnectThread
        m_pThread = nullptr;
    }
    return nRet;
}

QString CConnecterThread::ServerName()
{
    if(GetParameter())
        if(!GetParameter()->GetShowServerName()
            || CConnecter::ServerName().isEmpty())
        {
            if(!GetParameter()->m_Net.GetHost().isEmpty())
                return GetParameter()->m_Net.GetHost() + ":"
                       + QString::number(GetParameter()->m_Net.GetPort());
        }
    return CConnecter::ServerName();
}

int CConnecterThread::Load(QSettings &set)
{
    int nRet = 0;
    Q_ASSERT(m_pView);
    if(m_pView && GetParameter())
    {
        m_pView->slotSetAdaptWindows(GetParameter()->GetAdaptWindows());
        m_pView->slotSetZoomFactor(GetParameter()->GetZoomFactor());
    }
    nRet = CConnecterConnect::Load(set);
    return nRet;
}

int CConnecterThread::Save(QSettings &set)
{
    int nRet = 0;
    Q_ASSERT(GetParameter());
    if(GetParameter() && m_pView)
    {
        GetParameter()->SetAdaptWindows(m_pView->GetAdaptWindows());
        GetParameter()->SetZoomFactor(m_pView->GetZoomFactor());
    }
    nRet = CConnecterConnect::Save(set);
    return nRet;
}

QMenu *CConnecterThread::GetMenu(QWidget *parent)
{
    if(m_Menu.actions().isEmpty())
        return nullptr;
    return &m_Menu;
}

void CConnecterThread::slotRecord(bool checked)
{
    qDebug(log) << __FUNCTION__ << checked;
    QAction* pRecord = qobject_cast<QAction*>(sender());
    if(pRecord)
    {
        if(checked) {
            pRecord->setIcon(QIcon::fromTheme("media-playback-stop"));
            pRecord->setText(tr("Stop record"));
        }
        else {
            pRecord->setIcon(QIcon::fromTheme("media-playback-start"));
            pRecord->setText(tr("Start record"));
        }
        emit sigRecord(checked);
    }
}

#if HAVE_QT6_RECORD
void CConnecterThread::slotRecorderStateChanged(
    QMediaRecorder::RecorderState state)
{
    if(QMediaRecorder::StoppedState == state)
        slotRecord(false);
}
#endif
