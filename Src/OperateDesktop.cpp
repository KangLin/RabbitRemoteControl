// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QDesktopServices>
#include <QUrl>
#include <QWidgetAction>
#include <QActionGroup>

#include "OperateDesktop.h"
#include "BackendThread.h"
#include "Plugin.h"

static Q_LOGGING_CATEGORY(log, "Operate.Desktop")

COperateDesktop::COperateDesktop(CPlugin *plugin) : COperate(plugin)
    , m_pPara(nullptr)
    , m_pThread(nullptr)
    , m_pFrmViewer(nullptr)
    , m_pScroll(nullptr)
    , m_pZoomToWindow(nullptr)
    , m_pZoomAspectRatio(nullptr)
    , m_pZoomOriginal(nullptr)
    , m_pZoomIn(nullptr)
    , m_pZoomOut(nullptr)
    , m_psbZoomFactor(nullptr)
    , m_pScreenShot(nullptr)
#if HAVE_QT6_RECORD
    , m_pRecord(nullptr)
    , m_pRecordPause(nullptr)
#endif
{
    qDebug(log) << Q_FUNC_INFO;
}

COperateDesktop::~COperateDesktop()
{
    qDebug(log) << Q_FUNC_INFO;
}

const QString COperateDesktop::Id()
{
    QString szId = COperate::Id();
    if(GetParameter())
    {
        if(!GetParameter()->GetName().isEmpty())
            szId += "_" + GetParameter()->GetName();
        if(!GetParameter()->m_Net.GetHost().isEmpty())
            szId += "_" + GetParameter()->m_Net.GetHost()
                    + "_" + QString::number(GetParameter()->m_Net.GetPort());
        
        CParameterNet* net = nullptr;
        QString szType;
        switch(GetParameter()->m_Proxy.GetUsedType())
        {
        case CParameterProxy::TYPE::Http: {
            net = &GetParameter()->m_Proxy.m_Http;
            szType = "http";
            break;
        }
        case CParameterProxy::TYPE::SockesV5:
        {
            net = &GetParameter()->m_Proxy.m_SockesV5;
            szType = "sockesv5";
            break;
        }
        case CParameterProxy::TYPE::SSHTunnel:
        {
            net = &GetParameter()->m_Proxy.m_SSH.m_Net;
            szType = "ssh";
            break;
        }
        default:
            break;
        }
        
        if(!szType.isEmpty() && !net->GetHost().isEmpty()) {
            szId += "_" + szType + "_";
            szId += net->GetHost() + "_" + QString::number(net->GetPort());
        }
    }
    static QRegularExpression exp("[-@:/#%!^&* \\.]");
    szId = szId.replace(exp, "_");
    return szId;
}

const QString COperateDesktop::Name()
{
    QString szName;
    if(GetParameter() && GetParameter()->GetGlobalParameters()
        && GetParameter()->GetGlobalParameters()->GetShowProtocolPrefix())
        szName = Protocol() + ":";
    
    if(GetParameter() && !(GetParameter()->GetName().isEmpty()))
        szName += GetParameter()->GetName();
    else
        szName += ServerName();
    return szName;
}

const QString COperateDesktop::Description()
{
    QString szDescription;
    if(!Name().isEmpty())
        szDescription = tr("Name: ") + Name() + "\n";
    
    if(!GetTypeName().isEmpty())
        szDescription += tr("Type:") + GetTypeName() + "\n";
    
    if(!Protocol().isEmpty()) {
        szDescription += tr("Protocol: ") + Protocol();
#ifdef DEBUG
        if(!GetPlugin()->DisplayName().isEmpty())
            szDescription += " - " + GetPlugin()->DisplayName();
#endif
        szDescription += "\n";
    }
    
    if(!ServerName().isEmpty())
        szDescription += tr("Server name: ") + ServerName() + "\n";
    
    if(GetParameter()) {
        if(!GetParameter()->m_Net.GetHost().isEmpty())
            szDescription += tr("Server address: ") + GetParameter()->m_Net.GetHost() + ":"
                             + QString::number(GetParameter()->m_Net.GetPort()) + "\n";
        
        QString szProxy(tr("Proxy") + " ");
        auto &proxy = GetParameter()->m_Proxy;
        switch(proxy.GetUsedType()) {
        case CParameterProxy::TYPE::SSHTunnel:
        {
            auto &sshNet = proxy.m_SSH.m_Net;
            szProxy += "(" + tr("SSH tunnel") + "): " + sshNet.GetHost() + ":"
                       + QString::number(sshNet.GetPort());
            break;
        }
        case CParameterProxy::TYPE::SockesV5:
        {
            auto &sockesV5 = proxy.m_SockesV5;
            szProxy += "(" + tr("Sockes v5") + "): " + sockesV5.GetHost() + ":"
                       + QString::number(sockesV5.GetPort());
            break;
        }
        default:
            szProxy.clear();
            break;
        }
        
        if(!szProxy.isEmpty())
            szDescription += szProxy + "\n";
    }
    
    if(GetSecurityLevel() != SecurityLevel::No)
        szDescription += tr("Security level: ") + GetSecurityLevelString() + "\n";

    if(!GetPlugin()->Description().isEmpty())
        szDescription += tr("Description: ") + GetPlugin()->Description();
    
    return szDescription;
}

const qint16 COperateDesktop::Version() const
{
    return 0;
}

int COperateDesktop::Initial()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    bool check = false;
    
    nRet = COperate::Initial();
    if(nRet)
        return nRet;
    
    Q_ASSERT(!(m_pFrmViewer && m_pScroll));
    m_pFrmViewer = new CFrmViewer(); // The owner is m_pScroll
    m_pScroll = new CFrmScroll(m_pFrmViewer);
    
    check = connect(m_pFrmViewer, SIGNAL(sigViewerFocusIn(QWidget*)),
                    this, SIGNAL(sigViewerFocusIn(QWidget*)));
    Q_ASSERT(check);
    
    nRet = InitialMenu();
    
    return nRet;
}

int COperateDesktop::Clean()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    if(m_pScroll)
    {
        delete m_pScroll;
        m_pScroll = nullptr;
    }
    
    nRet = COperate::Clean();
    return nRet;
}

int COperateDesktop::InitialMenu()
{
    int nRet = 0;
    bool check = false;
    
    QMenu* pMenuZoom = new QMenu(&m_Menu);
    pMenuZoom->setTitle(tr("Zoom"));
    pMenuZoom->setIcon(QIcon::fromTheme("zoom"));
    m_pMenuZoom = m_Menu.addMenu(pMenuZoom);
    m_pZoomToWindow = pMenuZoom->addAction(
        QIcon::fromTheme("zoom-fit-best"), tr("Zoom to window"));
    m_pZoomToWindow->setCheckable(true);
    check = connect(m_pZoomToWindow, &QAction::triggered, this,
                    [&](){
                        m_pScroll->slotSetAdaptWindows(
                            CFrmViewer::ADAPT_WINDOWS::ZoomToWindow);
                    });
    Q_ASSERT(check);
    m_pZoomAspectRatio = pMenuZoom->addAction(
        QIcon::fromTheme("zoom-aspect-ratio"),
        tr("Keep aspect ration to windows"));
    m_pZoomAspectRatio->setCheckable(true);
    check = connect(m_pZoomAspectRatio, &QAction::triggered, this,
                    [&](){
                        m_pScroll->slotSetAdaptWindows(
                            CFrmViewer::ADAPT_WINDOWS::KeepAspectRationToWindow);
                    });
    Q_ASSERT(check);
    m_pZoomOriginal = pMenuZoom->addAction(
        QIcon::fromTheme("zoom-original"), tr("Original"));
    m_pZoomOriginal->setCheckable(true);
    check = connect(m_pZoomOriginal, &QAction::triggered, this,
                    [&](){
                        m_pScroll->slotSetAdaptWindows(
                            CFrmViewer::ADAPT_WINDOWS::Original);
                    });
    Q_ASSERT(check);
    m_pZoomIn = pMenuZoom->addAction(QIcon::fromTheme("zoom-in"), tr("Zoom in"));
    m_pZoomIn->setCheckable(true);
    check = connect(
        m_pZoomIn, &QAction::triggered, this,
        [&](){
            double factor = 0;
            if(m_psbZoomFactor) {
                factor = (m_pFrmViewer->GetZoomFactor() + 0.1) * 100;
                qDebug(log) << "Zoom in:" << factor;
                m_psbZoomFactor->setValue(factor);
            }
        });
    Q_ASSERT(check);
    m_pZoomOut = pMenuZoom->addAction(
        QIcon::fromTheme("zoom-out"), tr("Zoom out"));
    m_pZoomOut->setCheckable(true);
    check = connect(
        m_pZoomOut, &QAction::triggered, this,
        [&](){
            double factor = 100;
            if(m_psbZoomFactor) {
                factor = (m_pFrmViewer->GetZoomFactor() - 0.1) * 100;
                qDebug(log) << "Zoom out:" << factor;
                m_psbZoomFactor->setValue(factor);
            }
        });
    Q_ASSERT(check);
    QActionGroup* pGBViewZoom = new QActionGroup(this);
    if(pGBViewZoom) {
        pGBViewZoom->addAction(m_pZoomToWindow);
        pGBViewZoom->addAction(m_pZoomAspectRatio);
        pGBViewZoom->addAction(m_pZoomOriginal);
        pGBViewZoom->addAction(m_pZoomIn);
        pGBViewZoom->addAction(m_pZoomOut);
        check = connect(pGBViewZoom, &QActionGroup::triggered,
                        this, [&](QAction* a){
                            if(a == m_pZoomIn || a == m_pZoomOut)
                                m_psbZoomFactor->setEnabled(true);
                            else {
                                m_psbZoomFactor->setEnabled(false);
                            }
                        });
    }
    m_psbZoomFactor = new QSpinBox(pMenuZoom);
    m_psbZoomFactor->setRange(0, 9999999);
    m_psbZoomFactor->setValue(100);
    m_psbZoomFactor->setSuffix("%");
    m_psbZoomFactor->setEnabled(false);
    //m_psbZoomFactor->setFocusPolicy(Qt::NoFocus);
    check = connect(
        m_psbZoomFactor, SIGNAL(valueChanged(int)),
        this, SLOT(slotValueChanged(int)));
    Q_ASSERT(check);
    QWidgetAction* pFactor = new QWidgetAction(pMenuZoom);
    pFactor->setDefaultWidget(m_psbZoomFactor);
    pMenuZoom->insertAction(m_pZoomOut, pFactor);
    
    QMenu* pMenuShortCut = new QMenu(&m_Menu);
    pMenuShortCut->setTitle(tr("Send shortcut key"));
    m_Menu.addMenu(pMenuShortCut);
    pMenuShortCut->addAction(
        tr("Send Ctl+Alt+Del"), this, SLOT(slotShortcutCtlAltDel()));
    pMenuShortCut->addAction(
        tr("Send lock screen (Win+L)"), this, SLOT(slotShortcutLock()));
    
    m_Menu.addSeparator();
    m_pScreenShot = new QAction(QIcon::fromTheme("camera-photo"),
                                tr("ScreenShot"), &m_Menu);
    check = connect(m_pScreenShot, SIGNAL(triggered()),
                    this, SLOT(slotScreenShot()));
    Q_ASSERT(check);
    m_Menu.addAction(m_pScreenShot);
#if HAVE_QT6_RECORD
    m_pRecord = new QAction(
        QIcon::fromTheme("media-record"), tr("Start record"), &m_Menu);
    m_pRecord->setCheckable(true);
    check = connect(m_pRecord, SIGNAL(triggered(bool)),
                    this, SLOT(slotRecord(bool)));
    Q_ASSERT(check);
    m_Menu.addAction(m_pRecord);
    m_pRecordPause = new QAction(
        QIcon::fromTheme("media-playback-pause"), tr("Record pause"), &m_Menu);
    m_pRecordPause->setCheckable(true);
    m_pRecordPause->setEnabled(false);
    check = connect(m_pRecordPause, SIGNAL(triggered(bool)),
                    this, SIGNAL(sigRecordPause(bool)));
    Q_ASSERT(check);
    m_Menu.addAction(m_pRecordPause);
#endif
    
    m_Menu.addSeparator();
    if(m_pActionSettings)
        m_Menu.addAction(m_pActionSettings);
    
    return nRet;
}

QWidget *COperateDesktop::GetViewer()
{
    return m_pScroll;
}

int COperateDesktop::Start()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    m_pThread = new CBackendThread(this);
    if(!m_pThread) {
        qCritical(log) << "new CBackendThread fail";
        return -2;
    }
    
    m_pThread->start();
    
    return nRet;
}

int COperateDesktop::Stop()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    if(m_pThread)
    {
        m_pThread->quit();
        //Don't delete m_pThread, See CConnectThread
        m_pThread = nullptr;
    }
    return nRet;
}

int COperateDesktop::SetGlobalParameters(CParameterPlugin *pPara)
{
    if(GetParameter())
    {
        GetParameter()->SetGlobalParameters(pPara);
        COperate::SetGlobalParameters(pPara);
        LoadAdaptWindows();
        return 0;
    } else {
        QString szMsg = "There is not parameters! "
                        "please first create parameters, "
                        "then call SetParameter() in the ";
        szMsg += metaObject()->className() + QString("::")
                 + metaObject()->className();
        szMsg += QString("() or ") + metaObject()->className()
                 + QString("::") + "Initial()";
        szMsg += " to set the parameters pointer. "
                 "Default set CParameterClient for the parameters of operate "
                 "(CParameterOperate or its derived classes) "
                 "See CManager::CreateOperate. "
                 "If you are sure the parameter of operate "
                 "does not need CParameterClient. "
                 "Please overload the SetGlobalParameters() in the ";
        szMsg += QString(metaObject()->className()) + " . don't set it";
        qCritical(log) << szMsg.toStdString().c_str();
        Q_ASSERT(false);
    }
    return -1;
}

CParameterBase* COperateDesktop::GetParameter()
{
    return m_pPara;
}

int COperateDesktop::SetParameter(CParameterBase *p)
{
    Q_ASSERT(!m_pPara);
    m_pPara = p;
    if(GetParameter())
    {
        bool check = false;
        check = connect(GetParameter(), SIGNAL(sigNameChanged()),
                        this, SLOT(slotUpdateName()));
        Q_ASSERT(check);
        check = connect(GetParameter(), SIGNAL(sigShowServerNameChanged()),
                        this, SLOT(slotUpdateName()));
        Q_ASSERT(check);
        check = connect(GetParameter(), &CParameter::sigChanged,
                        this, [&](){
                            emit this->sigUpdateParameters(this);
                        });
        Q_ASSERT(check);
        CFrmViewer* pViewer = m_pFrmViewer;
        if(pViewer) {
            check = connect(GetParameter(), SIGNAL(sigZoomFactorChanged(double)),
                            pViewer, SLOT(slotSetZoomFactor(double)));
            Q_ASSERT(check);
            check = connect(
                GetParameter(),
                SIGNAL(sigAdaptWindowsChanged(CFrmViewer::ADAPT_WINDOWS)),
                pViewer, SLOT(slotSetAdaptWindows(CFrmViewer::ADAPT_WINDOWS)));
            Q_ASSERT(check);
            check = connect(GetParameter(), SIGNAL(sigEnableInputMethod(bool)),
                            pViewer, SLOT(slotEnableInputMethod(bool)));
            Q_ASSERT(check);
        }
    }
    return 0;
}

int COperateDesktop::LoadAdaptWindows()
{
    if(m_pFrmViewer && GetParameter())
    {
        m_pFrmViewer->slotSetZoomFactor(GetParameter()->GetZoomFactor());
        if(m_psbZoomFactor)
            m_psbZoomFactor->setValue(m_pFrmViewer->GetZoomFactor() * 100);
        CFrmViewer::ADAPT_WINDOWS aw = GetParameter()->GetAdaptWindows();
        if(CFrmViewer::ADAPT_WINDOWS::ZoomToWindow == aw) {
            if(m_pZoomToWindow) {
                m_pZoomToWindow->trigger();
            }
        } else if(CFrmViewer::ADAPT_WINDOWS::KeepAspectRationToWindow == aw) {
            if(m_pZoomAspectRatio) {
                m_pZoomAspectRatio->trigger();
            }
        } else if(CFrmViewer::ADAPT_WINDOWS::Original == aw) {
            if(m_pZoomOriginal) {
                m_pZoomOriginal->trigger();
            }
        } else if(CFrmViewer::ADAPT_WINDOWS::Zoom == aw) {
            if(m_pZoomIn) {
                m_pZoomIn->trigger();
            }
        }
    }
    return 0;
}

int COperateDesktop::Load(QSettings &set)
{
    int nRet = 0;
    Q_ASSERT(m_pFrmViewer);
    nRet = COperate::Load(set);
    if(m_pPara)
        nRet = m_pPara->Load(set);
    else {
        QString szMsg = "There is not parameters! "
                        "please first create parameters, "
                        "then call SetParameter() in the ";
        szMsg += metaObject()->className() + QString("::")
                 + metaObject()->className();
        szMsg += QString("() or ") + metaObject()->className()
                 + QString("::") + "Initial()";
        szMsg += " to set the parameters pointer. ";
        qWarning(log) << szMsg.toStdString().c_str();
        Q_ASSERT(false);//TODO: delete it
    }
    
    LoadAdaptWindows();
    return nRet;
}

int COperateDesktop::Save(QSettings &set)
{
    int nRet = 0;
    Q_ASSERT(GetParameter());
    if(GetParameter() && m_pFrmViewer)
    {
        GetParameter()->SetAdaptWindows(m_pFrmViewer->GetAdaptWindows());
        GetParameter()->SetZoomFactor(m_pFrmViewer->GetZoomFactor());
    }
    nRet = COperate::Save(set);
    if(m_pPara)
        nRet = m_pPara->Save(set);
    return nRet;
}

#if HAVE_QT6_RECORD
void COperateDesktop::slotRecord(bool checked)
{
    qDebug(log) << Q_FUNC_INFO << checked;
    QAction* pRecord = qobject_cast<QAction*>(sender());
    if(pRecord)
    {
        if(checked) {
            //pRecord->setIcon(QIcon::fromTheme("media-playback-stop"));
            pRecord->setText(tr("Stop record"));
        } else {
            //pRecord->setIcon(QIcon::fromTheme("media-playback-start"));
            pRecord->setText(tr("Start record"));
        }
        m_pRecordPause->setEnabled(checked);
        emit sigRecord(checked);
    }
}

void COperateDesktop::slotRecorderStateChanged(
    QMediaRecorder::RecorderState state)
{
    qDebug(log) << Q_FUNC_INFO << state;
    if(QMediaRecorder::StoppedState == state)
    {
        m_pRecord->setChecked(false);
        m_pRecordPause->setChecked(false);
    }
}
#endif

void COperateDesktop::slotValueChanged(int v)
{
    qDebug(log) << "zoom:" << v;
    if(!m_pScroll || !m_pFrmViewer) return;
    m_pFrmViewer->slotSetZoomFactor(((double)v) / 100);
    m_pScroll->slotSetAdaptWindows(CFrmViewer::ADAPT_WINDOWS::Zoom);
}

void COperateDesktop::slotScreenShot()
{
    if(!GetParameter() || !m_pFrmViewer)
        return;
    auto &record = GetParameter()->m_Record;
    QString szFile = record.GetImageFile(true);
    bool bRet = m_pFrmViewer->GrabImage().save(szFile);
    if(bRet)
        qDebug(log) << "Success: save screenshot to" << szFile;
    else
        qCritical(log) << "Fail: save screenshot to" << szFile;
    if(record.GetEndAction() != CParameterRecord::ENDACTION::No)
        QDesktopServices::openUrl(QUrl::fromLocalFile(szFile));
}

void COperateDesktop::slotShortcutCtlAltDel()
{
    if(!m_pFrmViewer)
        return;
    // Send ctl+alt+del
    emit m_pFrmViewer->sigKeyPressEvent(new QKeyEvent(QKeyEvent::KeyPress, Qt::Key_Control, Qt::ControlModifier));
    emit m_pFrmViewer->sigKeyPressEvent(new QKeyEvent(QKeyEvent::KeyPress, Qt::Key_Alt, Qt::AltModifier));
    emit m_pFrmViewer->sigKeyPressEvent(new QKeyEvent(QKeyEvent::KeyPress, Qt::Key_Delete, Qt::ControlModifier | Qt::AltModifier));
    emit m_pFrmViewer->sigKeyPressEvent(new QKeyEvent(QKeyEvent::KeyRelease, Qt::Key_Control, Qt::ControlModifier));
    emit m_pFrmViewer->sigKeyPressEvent(new QKeyEvent(QKeyEvent::KeyRelease, Qt::Key_Alt, Qt::AltModifier));
    emit m_pFrmViewer->sigKeyPressEvent(new QKeyEvent(QKeyEvent::KeyRelease, Qt::Key_Delete, Qt::ControlModifier | Qt::AltModifier));
}

void COperateDesktop::slotShortcutLock()
{
    if(!m_pFrmViewer)
        return;
    // Send ctl+alt+del
    emit m_pFrmViewer->sigKeyPressEvent(new QKeyEvent(QKeyEvent::KeyPress, Qt::Key_Super_L, Qt::NoModifier));
    emit m_pFrmViewer->sigKeyPressEvent(new QKeyEvent(QKeyEvent::KeyPress, Qt::Key_L, Qt::NoModifier));
    emit m_pFrmViewer->sigKeyPressEvent(new QKeyEvent(QKeyEvent::KeyRelease, Qt::Key_Super_L, Qt::NoModifier));
    emit m_pFrmViewer->sigKeyPressEvent(new QKeyEvent(QKeyEvent::KeyRelease, Qt::Key_L, Qt::NoModifier));
}


QString COperateDesktop::ServerName()
{
    if(GetParameter())
        if(!GetParameter()->GetShowServerName()
            || m_szServerName.isEmpty())
        {
            if(!GetParameter()->m_Net.GetHost().isEmpty())
                return GetParameter()->m_Net.GetHost() + ":"
                       + QString::number(GetParameter()->m_Net.GetPort());
        }
    if(GetParameter() && GetParameter()->GetGlobalParameters()
        && GetParameter()->GetGlobalParameters()->GetShowIpPortInName())
    {
        return GetParameter()->m_Net.GetHost()
        + ":" + QString::number(GetParameter()->m_Net.GetPort());
    }
    
    if(m_szServerName.isEmpty() && GetParameter())
        return GetParameter()->GetServerName();
    return m_szServerName;
}

void COperateDesktop::slotSetServerName(const QString& szName)
{
    if(m_szServerName == szName)
        return;
    
    m_szServerName = szName;
    if(GetParameter())
    {
        if(GetParameter()->GetServerName() == szName)
            return;
        GetParameter()->SetServerName(szName);
    }
    
    emit sigUpdateName(Name());
}
