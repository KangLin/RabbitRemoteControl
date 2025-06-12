#include "ConnecterTerminal.h"
#include "Connect.h"
#include "ConnectThreadTerminal.h"

#include <QDialog>
#include <QApplication>
#include <QDebug>
#include <QTextCodec>
#include <QSettings>

#ifdef BUILD_QUIWidget
    #include "QUIWidget/QUIWidget.h"
#endif

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(Terminal)

CConnecterTerminal::CConnecterTerminal(CPluginClient *parent)
    : CConnecter(parent),
      m_pConsole(nullptr),
      m_pThread(nullptr),
      m_bThread(false),
      m_bExit(false),
      m_pPara(nullptr)
{
    m_pConsole = new CFrmTermWidget();
    m_pConsole->setAutoClose(true);
    
    bool check = false;
    check = connect(m_pConsole, SIGNAL(titleChanged()),
                    this, SLOT(slotTerminalTitleChanged()));
    Q_ASSERT(check);
    check = connect(m_pConsole, SIGNAL(finished()),
                    this, SIGNAL(sigDisconnected()));
    Q_ASSERT(check);
    check = connect(m_pConsole, SIGNAL(sigZoomReset()),
                    this, SLOT(slotZoomReset()));
    Q_ASSERT(check);
}

CConnecterTerminal::~CConnecterTerminal()
{
    qDebug(Terminal) << "CConnecterTerminal::~CConnecterTerminal()";
    
    if(m_pConsole)
    {
        m_pConsole->deleteLater();
        m_pConsole = nullptr;
    }
}

QWidget* CConnecterTerminal::GetViewer()
{
    return m_pConsole;
}

qint16 CConnecterTerminal::Version()
{
    return 0;
}

int CConnecterTerminal::Load(QSettings &set)
{
    int nRet = 0;
    CParameterConnecter* pPara = GetParameter();
    Q_ASSERT(pPara);
    if(!pPara) return -1;
    pPara->Load(set);
    return nRet;
}

int CConnecterTerminal::Save(QSettings &set)
{
    int nRet = 0;
    
    CParameterConnecter* pPara = GetParameter();
    Q_ASSERT(pPara);
    if(!pPara) return -1;
    pPara->Save(set);
    return nRet;
}

int CConnecterTerminal::Connect()
{
    int nRet = 0;

    nRet = SetParameter();

    if(m_bThread)
    {
        m_pThread = new CConnectThreadTerminal(this);
        bool check = connect(m_pThread, SIGNAL(finished()),
                        m_pThread, SLOT(deleteLater()));
        Q_ASSERT(check);
        if(m_pThread)
            m_pThread->start();
        return 0;
    }
    
    nRet = OnConnect();
    if(nRet < 0)
        emit sigDisconnected();
    else if(0 == nRet)
        emit sigConnected();
    
    if(m_pConsole)
        emit sigUpdateName(Name());
    
    return nRet;
}

int CConnecterTerminal::DisConnect()
{
    int nRet = 0;

    if(m_bThread && m_pThread)
    {
        m_pThread->quit();
    } else
        nRet = OnDisConnect();
    emit sigDisconnected();
    return nRet;
}

int CConnecterTerminal::SetParameter()
{
    int nRet = 0;

    CParameterTerminal* pPara = GetParameter();
    Q_ASSERT(pPara);
    if(!pPara) return -1;
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
    m_pConsole->setTextCodec(QTextCodec::codecForName(pPara->GetTextCodec().toStdString().c_str()));
    m_pConsole->setHistorySize(pPara->GetHistorySize());
    
//    m_pConsole->setMonitorActivity(false);
//    m_pConsole->setMonitorSilence(false);
//    m_pConsole->setBlinkingCursor(true);

    return nRet;
}

void CConnecterTerminal::slotTerminalTitleChanged()
{
    m_pConsole->setWindowTitle(m_pConsole->title());
    slotSetServerName(m_pConsole->title());
}

void CConnecterTerminal::slotZoomReset()
{
    if(!m_pConsole) return;
    CParameterTerminal* pPara = dynamic_cast<CParameterTerminal*>(GetParameter());
    if(!pPara) return;
    m_pConsole->setTerminalFont(pPara->GetFont());
}

int CConnecterTerminal::OnConnect()
{
    return 0;
}

int CConnecterTerminal::OnDisConnect()
{
    return 0;
}

QString CConnecterTerminal::ServerName()
{
    if(GetParameter())
        if(!GetParameter()->GetShowServerName()
                || CConnecter::ServerName().isEmpty())
        {
            if(!GetParameter()->GetHost().isEmpty())
                return GetParameter()->GetHost() + ":"
                        + QString::number(GetParameter()->GetPort());
        }
    return CConnecter::ServerName();
}

CConnect* CConnecterTerminal::InstanceConnect()
{
    return nullptr;
}

CParameterTerminal* CConnecterTerminal::GetParameter()
{
    return m_pPara;
}
