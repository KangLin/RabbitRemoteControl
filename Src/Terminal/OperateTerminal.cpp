// Author: Kang Lin <kl222@126.com>

#include <QDialog>
#include <QApplication>
#include <QSettings>
#include <QLoggingCategory>
#include <QDesktopServices>
#include <QDir>

#if defined(Q_OS_UNIX)
#include <unistd.h>
#endif

#include "ParameterOperate.h"
#include "Backend.h"
#include "BackendThread.h"
#include "OperateTerminal.h"
#include "Plugin.h"

static Q_LOGGING_CATEGORY(log, "Operate.Terminal")

COperateTerminal::COperateTerminal(CPlugin *parent)
    : COperate(parent)
    , m_pTerminal(nullptr)
    , m_pThread(nullptr)
    , m_pActionFind(nullptr)
    , m_pActionOpenWithSystem(nullptr)
    , m_pParameters(nullptr)
    , m_pEditCopy(nullptr)
    , m_pEditPaste(nullptr)
    , m_pZoomIn(nullptr)
    , m_pZoomOut(nullptr)
    , m_pZoomOriginal(nullptr)
    , m_pClear(nullptr)
{
    QTermWidget::addCustomColorSchemeDir(QApplication::applicationDirPath()
                                         + QDir::separator() + "color-schemes");
    QTermWidget::addCustomColorSchemeDir(QApplication::applicationDirPath()
                                         + QDir::separator() + ".."
                                         + QDir::separator() + "share"
                                         + QDir::separator() + "qtermwidget6"
                                         + QDir::separator() + "color-schemes");
    /*QTermWidget::addCustomKeyboardLayoutDir(QApplication::applicationDirPath()
                                            + QDir::separator() + "kb-layouts");
    QTermWidget::addCustomKeyboardLayoutDir(QApplication::applicationDirPath()
                                            + QDir::separator() + ".."
                                            + QDir::separator() + "share"
                                            + QDir::separator() + "qtermwidget6"
                                            + QDir::separator() + "kb-layouts");//*/
    m_pTerminal = new QTermWidget(0);
    if(!m_pTerminal) {
        qCritical(log) << "new QTermWidget() fail";
        return;
    }
    m_pTerminal->setAutoClose(true);

    bool check = false;
    m_pTerminal->setContextMenuPolicy(Qt::CustomContextMenu);
    check = connect(m_pTerminal, SIGNAL(customContextMenuRequested(const QPoint &)),
                         this, SLOT(slotCustomContextMenu(const QPoint &)));
    Q_ASSERT(check);
    check = connect(m_pTerminal, SIGNAL(urlActivated(const QUrl&, bool)),
                    this, SLOT(slotActivateUrl(const QUrl&, bool)));
    Q_ASSERT(check);
    // check = connect(m_pTerminal, SIGNAL(titleChanged()),
    //                 this, SLOT(slotTerminalTitleChanged()));
    // Q_ASSERT(check);
    check = connect(m_pTerminal, SIGNAL(finished()),
                    this, SIGNAL(sigStop()));
    Q_ASSERT(check);
    // check = connect(m_pTerminal, SIGNAL(termKeyPressed(QKeyEvent*)),
    //                 this, SLOT(slotTermKeyPressed(QKeyEvent*)));
    // Q_ASSERT(check);
    check = connect(m_pTerminal, SIGNAL(termGetFocus()),
                    this, SLOT(slotFocusIn()));
    Q_ASSERT(check);
    check = connect(m_pTerminal, SIGNAL(termLostFocus()),
                    this, SLOT(slotFocusOut()));
    Q_ASSERT(check);
}

COperateTerminal::~COperateTerminal()
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_pTerminal)
    {
        m_pTerminal->deleteLater();
        m_pTerminal = nullptr;
    }
}

const QString COperateTerminal::Description()
{
    QString szDescription;
    if(!Name().isEmpty())
        szDescription = tr("Name: ") + Name() + "\n";
    
    if(!GetTypeName().isEmpty())
        szDescription += tr("Type: ") + GetTypeName() + "\n";
    
    if(!Protocol().isEmpty()) {
        szDescription += tr("Protocol: ") + Protocol();
#ifdef DEBUG
        if(!GetPlugin()->DisplayName().isEmpty())
            szDescription += " - " + GetPlugin()->DisplayName();
#endif
        szDescription += "\n";
    }

    if(GetSecurityLevel() != SecurityLevel::No)
        szDescription += tr("Security level: ") + GetSecurityLevelString() + "\n";

    if(!GetPlugin()->Description().isEmpty())
        szDescription += tr("Description: ") + GetPlugin()->Description();
    
    return szDescription;
}

QWidget* COperateTerminal::GetViewer()
{
    return m_pTerminal;
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
    
    m_pEditCopy = m_Menu.addAction(QIcon::fromTheme("edit-copy"),
                                   tr("Copy selection to clipboard") + "\tCtrl+Shift+C",
                                   m_pTerminal, SLOT(copyClipboard()));
    m_pEditPaste = m_Menu.addAction(QIcon::fromTheme("edit-paste"),
                                    tr("Paste clipboard") + "\tCtrl+V",
                                    m_pTerminal, SLOT(pasteClipboard()));
    m_Menu.addAction(tr("Paste selection"), m_pTerminal, SLOT(pasteSelection()));
    m_Menu.addSeparator();
    m_pZoomIn = m_Menu.addAction(QIcon::fromTheme("zoom-in"), tr("Zoom in") + "\tCtrl++",
                     m_pTerminal, SLOT(zoomIn()));
    m_pZoomOut = m_Menu.addAction(QIcon::fromTheme("zoom-out"), tr("Zoom out") + "\tCtrl+-",
                     m_pTerminal, SLOT(zoomOut()));
    m_pZoomOriginal = m_Menu.addAction(QIcon::fromTheme("zoom-original"), tr("Zoom reset") + "\tCtrl+0",
                     this, SLOT(slotZoomReset()));

    m_Menu.addSeparator();
    // m_pActionOpenWithSystem = m_Menu.addAction(
    //     QIcon::fromTheme("folder-open"),
    //     tr("Open the selection with the System Associated Program"),
    //     QKeySequence(Qt::CTRL | Qt::Key_O),
    //     [&](){
    //         QString szText = m_pTerminal->selectedText(false);
    //         QUrl url(szText);
    //         qDebug(log) << szText << url;
    //         QDesktopServices::openUrl(url);
    //     });
    m_pActionFind = m_Menu.addAction(
        QIcon::fromTheme("edit-find"), tr("Find ......") + "\tCtrl+F",
        m_pTerminal, &QTermWidget::toggleShowSearchBar);
    
    m_Menu.addSeparator();
    m_pClear = m_Menu.addAction(QIcon::fromTheme("edit-clear"),
                                tr("Clear") + "\tCtrl+R",
                                m_pTerminal, SLOT(clear()));

    if(m_pActionSettings) {
        m_Menu.addSeparator();
        m_Menu.addAction(m_pActionSettings);
    }

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
        //Don't delete m_pThread, See CBackendThread
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

    if(!pPara || !m_pTerminal) return -1;

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
    if(!GetParameter() || !m_pTerminal) return;

    auto pPara = &GetParameter()->m_Terminal;

#if QTERMWIDGET_VERSION >= QT_VERSION_CHECK(0, 9, 0)
    m_pTerminal->setTerminalSizeHint(pPara->GetSizeHint());
    m_pTerminal->setBidiEnabled(pPara->GetDirectional());
    m_pTerminal->disableBracketedPasteMode(pPara->GetDisableBracketedPasteMode());
#endif
    m_pTerminal->setTerminalFont(pPara->GetFont());
    m_pTerminal->setKeyboardCursorShape(pPara->GetCursorShape());
    m_pTerminal->setColorScheme(pPara->GetColorScheme());
    m_pTerminal->setScrollBarPosition(pPara->GetScrollBarPosition());
    m_pTerminal->setFlowControlEnabled(pPara->GetFlowControl());
    m_pTerminal->setFlowControlWarningEnabled(pPara->GetFlowControlWarning());
    m_pTerminal->setMotionAfterPasting(pPara->GetMotionAfterPasting());
    m_pTerminal->setTerminalOpacity(1.0 - pPara->GetTransparency() / 100.0);
    m_pTerminal->setTerminalBackgroundImage(pPara->GetBackgroupImage());

    m_pTerminal->setKeyBindings(pPara->GetKeyBindings());
    m_pTerminal->setHistorySize(pPara->GetHistorySize());

    //    m_pTerminal->setMonitorActivity(false);
    //    m_pTerminal->setMonitorSilence(false);
    //    m_pTerminal->setBlinkingCursor(true);
}

void COperateTerminal::slotZoomReset()
{
    if(!m_pTerminal) return;
    CParameterTerminal* pPara = &GetParameter()->m_Terminal;
    if(!pPara) return;
    m_pTerminal->setTerminalFont(pPara->GetFont());
}

void COperateTerminal::slotCustomContextMenu(const QPoint & pos)
{
    if(!m_pTerminal) return;
    
    // QString szText = m_pTerminal->selectedText(false);
    // m_pActionOpenWithSystem->setVisible(
    //     !szText.isEmpty()
    //     && QUrl(szText).isValid());
    m_Menu.exec(m_pTerminal->mapToGlobal(pos));
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
    SetShotcuts(true);
}

void COperateTerminal::slotFocusOut()
{
    SetShotcuts(false);
}

const qint16 COperateTerminal::Version() const
{
    return 0;
}

const QString COperateTerminal::Details()
{
    QString szDetails;
    szDetails = "-  " + tr("QTermWidget:") + " "
                + QString("%1.%2.%3").arg(QTERMWIDGET_VERSION_MAJOR)
                      .arg(QTERMWIDGET_VERSION_MINOR)
                      .arg(QTERMWIDGET_VERSION_PATCH) + "\n";
    return szDetails;
}

int COperateTerminal::SetGlobalParameters(CParameterPlugin *pPara)
{
    Q_ASSERT(pPara);
    if(GetParameter())
    {
        GetParameter()->SetGlobalParameters(pPara);
        return COperate::SetGlobalParameters(pPara);
    } else {
        QString szMsg = "There is not parameters! "
                        "please first create parameters, "
                        "then call SetParameter() in the ";
        szMsg += metaObject()->className() + QString("::")
                 + metaObject()->className();
        szMsg += QString("() or ") + metaObject()->className()
                 + QString("::") + "Initial()";
        szMsg += " to set the parameters pointer. "
                 "Default set CParameterPlugin for the parameters of operate "
                 "(CParameterOperate or its derived classes) "
                 "See CManager::CreateOperate. "
                 "If you are sure the parameter of operate "
                 "does not need CParameterPlugin. "
                 "Please overload the SetGlobalParameters() in the ";
        szMsg += QString(metaObject()->className()) + " . don't set it";
        qCritical(log) << szMsg.toStdString().c_str();
        Q_ASSERT(false);
    }
    return -1;
}

int COperateTerminal::WriteTerminal(const char *buf, int len)
{
    int nRet = 0;
#if defined(Q_OS_WIN)
    if(m_pTerminal) {
        m_pTerminal->receiveData(buf, len);
    }
#else
    nRet = write(m_pTerminal->getPtySlaveFd(), buf, len);
#endif
    return nRet;
}

void COperateTerminal::SetShotcuts(bool bEnable)
{
    if(bEnable) {
        m_pEditCopy->setShortcuts(
            QList<QKeySequence>() << QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C)
                                  << QKeySequence(Qt::CTRL | Qt::Key_Insert));
        m_pEditPaste->setShortcuts(QList<QKeySequence>()
                                   << QKeySequence(QKeySequence::Paste)
                                   << QKeySequence(Qt::SHIFT | Qt::Key_Insert));
        m_pZoomIn->setShortcut(QKeySequence(QKeySequence::ZoomIn));
        m_pZoomOut->setShortcut(QKeySequence(QKeySequence::ZoomOut));
        m_pZoomOriginal->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
        m_pActionFind->setShortcut(QKeySequence(QKeySequence::Find));
        m_pClear->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
        return;
    }

    m_pEditCopy->setShortcuts(QList<QKeySequence>());
    m_pEditPaste->setShortcut(QKeySequence());
    m_pZoomIn->setShortcut(QKeySequence());
    m_pZoomOut->setShortcut(QKeySequence());
    m_pZoomOriginal->setShortcut(QKeySequence());
    m_pActionFind->setShortcut(QKeySequence());
    m_pClear->setShortcut(QKeySequence());
}

const QString COperateTerminal::Id()
{
    QString szId = COperate::Id();
    if(GetParameter()) {
        if(GetParameter()->GetName().isEmpty())
            szId += "_" + Name();
        else
            szId += "_" + GetParameter()->GetName();
    }
    static QRegularExpression exp("[-@:/#%!^&* \\.]");
    szId = szId.replace(exp, "_");
    return szId;
}

const QString COperateTerminal::Name()
{
    QString szName;
    if(GetParameter())
        szName = GetParameter()->GetName();
    if(szName.isEmpty())
        szName = COperate::Name();
    return szName;
}
