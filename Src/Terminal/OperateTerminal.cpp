// Author: Kang Lin <kl222@126.com>

#include <QDialog>
#include <QApplication>
#include <QSettings>
#include <QLoggingCategory>
#include <QDesktopServices>

#include "ParameterOperate.h"
#include "Backend.h"
#include "BackendThread.h"
#include "OperateTerminal.h"

static Q_LOGGING_CATEGORY(log, "Operate.Terminal")

COperateTerminal::COperateTerminal(CPlugin *parent)
    : COperate(parent),
      m_pConsole(nullptr),
      m_pThread(nullptr),
      m_pParameters(nullptr)
{
    m_pConsole = new QTermWidget(0);
    if(!m_pConsole)
        qCritical(log) << "new QTermWidget() fail";

    m_pConsole->setAutoClose(true);

    bool check = false;
    m_pConsole->setContextMenuPolicy(Qt::CustomContextMenu);
    check = connect(m_pConsole, SIGNAL(customContextMenuRequested(const QPoint &)),
                         this, SLOT(slotCustomContextMenu(const QPoint &)));
    Q_ASSERT(check);
    check = connect(m_pConsole, SIGNAL(urlActivated(const QUrl&, bool)),
                    this, SLOT(slotActivateUrl(const QUrl&, bool)));
    Q_ASSERT(check);
    // check = connect(m_pConsole, SIGNAL(titleChanged()),
    //                 this, SLOT(slotTerminalTitleChanged()));
    // Q_ASSERT(check);
    check = connect(m_pConsole, SIGNAL(finished()),
                    this, SIGNAL(sigFinished()));
    Q_ASSERT(check);
    check = connect(m_pConsole, SIGNAL(termKeyPressed(QKeyEvent*)),
                    this, SLOT(slotTermKeyPressed(QKeyEvent*)));
    Q_ASSERT(check);
    check = connect(m_pConsole, SIGNAL(termGetFocus()),
                    this, SLOT(slotFocusIn()));
    Q_ASSERT(check);
}

COperateTerminal::~COperateTerminal()
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_pConsole)
    {
        m_pConsole->deleteLater();
        m_pConsole = nullptr;
    }
}

QWidget* COperateTerminal::GetViewer()
{
    return m_pConsole;
}

int COperateTerminal::Load(QSettings &set)
{
    int nRet = 0;
    auto pPara = GetParameter();
    Q_ASSERT(pPara);
    if(!pPara) return -1;
    nRet = pPara->Load(set);
    return nRet;
}

int COperateTerminal::Save(QSettings &set)
{
    int nRet = 0;
    auto pPara = GetParameter();
    Q_ASSERT(pPara);
    if(!pPara) return -1;
    nRet = pPara->Save(set);
    return nRet;
}

int COperateTerminal::Initial()
{
    int nRet = 0;

    nRet = COperate::Initial();
    if(nRet)
        return nRet;

    if(m_pActionSettings)
        m_Menu.addAction(m_pActionSettings);

    return nRet;
}

int COperateTerminal::Clean()
{
    int nRet = 0;
    nRet = COperate::Clean();
    return nRet;
}

int COperateTerminal::Start()
{
    int nRet = 0;

    slotUpdateParameter(this);
    m_pThread = new CBackendThread(this);
    if(!m_pThread) {
        qCritical(log) << "new CBackendThread fail";
        return -2;
    }
    m_pThread->start();
    return nRet;
}

int COperateTerminal::Stop()
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

CParameterTerminalBase* COperateTerminal::GetParameter()
{
    return m_pParameters;
}

int COperateTerminal::SetParameter(CParameterTerminalBase* pPara)
{
    int nRet = 0;

    Q_ASSERT(pPara);
    Q_ASSERT(!m_pParameters);

    m_pParameters = pPara;

    if(!pPara || !m_pConsole) return -1;

    bool check = connect(GetParameter(), &CParameter::sigChanged,
                    this, [&](){
                        emit this->sigUpdateParameters(this);
                    });
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigUpdateParameters(COperate*)),
                         this, SLOT(slotUpdateParameter(COperate*)));
    Q_ASSERT(check);

    return nRet;
}

void COperateTerminal::slotUpdateParameter(COperate* pOperate)
{
    qDebug(log) << Q_FUNC_INFO;
    if(this != pOperate) return;
    if(!GetParameter() || !m_pConsole) return;

    auto pPara = &GetParameter()->m_Terminal;

#if QTERMWIDGET_VERSION >= QT_VERSION_CHECK(0, 9, 0)
    m_pConsole->setTerminalSizeHint(pPara->GetSizeHint());
    m_pConsole->setBidiEnabled(pPara->GetDirectional());
    m_pConsole->disableBracketedPasteMode(pPara->GetDisableBracketedPasteMode());
#endif
    m_pConsole->setTerminalFont(pPara->GetFont());
    m_pConsole->setKeyboardCursorShape(pPara->GetCursorShape());
    m_pConsole->setColorScheme(pPara->GetColorScheme());
    m_pConsole->setScrollBarPosition(pPara->GetScrollBarPosition());
    m_pConsole->setFlowControlEnabled(pPara->GetFlowControl());
    m_pConsole->setFlowControlWarningEnabled(pPara->GetFlowControlWarning());
    m_pConsole->setMotionAfterPasting(pPara->GetMotionAfterPasting());
    m_pConsole->setTerminalOpacity(1.0 - pPara->GetTransparency() / 100.0);
    m_pConsole->setTerminalBackgroundImage(pPara->GetBackgroupImage());

    m_pConsole->setKeyBindings(pPara->GetKeyBindings());
    m_pConsole->setHistorySize(pPara->GetHistorySize());

    //    m_pConsole->setMonitorActivity(false);
    //    m_pConsole->setMonitorSilence(false);
    //    m_pConsole->setBlinkingCursor(true);
}

void COperateTerminal::slotTerminalTitleChanged()
{
    if(!m_pConsole) return;
    emit sigUpdateName(m_pConsole->title());
}

void COperateTerminal::slotZoomReset()
{
    if(!m_pConsole) return;
    CParameterTerminal* pPara = &GetParameter()->m_Terminal;
    if(!pPara) return;
    m_pConsole->setTerminalFont(pPara->GetFont());
}

void COperateTerminal::slotCustomContextMenu(const QPoint & pos)
{
    if(!m_pConsole) return;

    QMenu menu;
    menu.addAction(tr("Copy selection to clipboard"), m_pConsole, SLOT(copyClipboard()));
    menu.addAction(tr("Paste clipboard"), m_pConsole, SLOT(pasteClipboard()));
    menu.addAction(tr("Paste selection"), m_pConsole, SLOT(pasteSelection()));
    menu.addSeparator();
    menu.addAction(tr("Zoom in"), m_pConsole, SLOT(zoomIn()));
    menu.addAction(tr("Zoom out"), m_pConsole, SLOT(zoomOut()));
    menu.addAction(tr("Zoom reset"), this, SIGNAL(slotZoomReset()));
    menu.addSeparator();
    menu.addAction(tr("Clear"), m_pConsole, SLOT(clear()));

    menu.exec(m_pConsole->mapToGlobal(pos));
}

void COperateTerminal::slotActivateUrl(const QUrl& url, bool fromContextMenu)
{
    if (QApplication::keyboardModifiers() & Qt::ControlModifier || fromContextMenu) {
        QDesktopServices::openUrl(url);
    }
}

void COperateTerminal::slotTermKeyPressed(QKeyEvent* e)
{
    qDebug(log) << Q_FUNC_INFO << e;
}

void COperateTerminal::slotFocusIn()
{
    emit sigViewerFocusIn(GetViewer());
}

const qint16 COperateTerminal::Version() const
{
    return 0;
}

int COperateTerminal::SetGlobalParameters(CParameterPlugin *pPara)
{
    Q_ASSERT(pPara);
    if(GetParameter())
    {
        GetParameter()->SetGlobalParameters(pPara);
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
