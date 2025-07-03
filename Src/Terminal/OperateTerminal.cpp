// Author: Kang Lin <kl222@126.com>

#include <QDialog>
#include <QApplication>
#include <QSettings>
#include <QLoggingCategory>
#include <QDesktopServices>
#include <QDir>

#include "ParameterOperate.h"
#include "Backend.h"
#include "BackendThread.h"
#include "OperateTerminal.h"
#include "Plugin.h"

static Q_LOGGING_CATEGORY(log, "Operate.Terminal")

COperateTerminal::COperateTerminal(CPlugin *parent)
    : COperate(parent),
      m_pTerminal(nullptr),
      m_pThread(nullptr),
      m_pParameters(nullptr)
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
                    this, SIGNAL(sigFinished()));
    Q_ASSERT(check);
    // check = connect(m_pTerminal, SIGNAL(termKeyPressed(QKeyEvent*)),
    //                 this, SLOT(slotTermKeyPressed(QKeyEvent*)));
    // Q_ASSERT(check);
    check = connect(m_pTerminal, SIGNAL(termGetFocus()),
                    this, SLOT(slotFocusIn()));
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

const QString COperateTerminal::Id()
{
    QString szId = COperate::Id();
    if(GetParameter())
    {
        auto &sshNet = GetParameter()->m_SSH.m_Net;
        if(!sshNet.GetHost().isEmpty())
            szId += "_" + sshNet.GetHost()
                    + "_" + QString::number(sshNet.GetPort());
    }
    static QRegularExpression exp("[-@:/#%!^&* \\.]");
    szId = szId.replace(exp, "_");
    return szId;
}

const QString COperateTerminal::Name()
{
    QString szName;
    if(GetParameter())
    {
        auto &sshNet = GetParameter()->m_SSH.m_Net;
        if(!sshNet.GetHost().isEmpty()) {
            if(GetParameter()->GetGlobalParameters()
                && GetParameter()->GetGlobalParameters()->GetShowProtocolPrefix())
                szName = Protocol() + ":";
            szName += sshNet.GetHost()
                      + ":" + QString::number(sshNet.GetPort());
        }
    }
    if(szName.isEmpty())
        szName = COperate::Name();
    return szName;
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

    if(GetParameter()) {
        auto &sshNet = GetParameter()->m_SSH.m_Net;
        if(!sshNet.GetHost().isEmpty())
            szDescription += tr("Server address: ") + sshNet.GetHost()
                    + ":" + QString::number(sshNet.GetPort()) + "\n";
        else {
            szDescription += tr("Shell path: ") + GetParameter()->GetShell() + "\n";
            if(!GetParameter()->GetShellParameters().isEmpty())
                szDescription += tr("Shell parameters: ") + GetParameter()->GetShellParameters() + "\n";
        }
    }

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

    m_Menu.addAction(tr("Copy selection to clipboard"),
                     QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C),
                     m_pTerminal, SLOT(copyClipboard()));
    m_Menu.addAction(tr("Paste clipboard"), QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_V),
                     m_pTerminal, SLOT(pasteClipboard()));
    m_Menu.addAction(tr("Paste selection"), m_pTerminal, SLOT(pasteSelection()));
    m_Menu.addSeparator();
    m_Menu.addAction(tr("Zoom in"), QKeySequence(Qt::CTRL | Qt::Key_Plus),
                     m_pTerminal, SLOT(zoomIn()));
    m_Menu.addAction(tr("Zoom out"), QKeySequence(Qt::CTRL | Qt::Key_Minus),
                     m_pTerminal, SLOT(zoomOut()));
    m_Menu.addAction(tr("Zoom reset"), QKeySequence(Qt::CTRL | Qt::Key_0),
                     this, SLOT(slotZoomReset()));

    m_Menu.addSeparator();
    m_Menu.addAction(tr("Find ......"), QKeySequence(Qt::CTRL | Qt::Key_F),
                       m_pTerminal, &QTermWidget::toggleShowSearchBar);
    m_Menu.addSeparator();
    m_Menu.addAction(tr("Clear"), m_pTerminal, SLOT(clear()));

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

void COperateTerminal::slotTerminalTitleChanged()
{
    if(!m_pTerminal) return;
    emit sigUpdateName(m_pTerminal->title());
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
                 "Default set CParameterPlugin for the parameters of operate "
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
