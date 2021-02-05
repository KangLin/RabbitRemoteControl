#include "ConnecterPluginsTerminal.h"
#include "RabbitCommonLog.h"
#include "Connect.h"
#include "ConnectThreadTerminal.h"

#include <QDialog>
#include <QApplication>
#include <QDebug>
#include <QTextCodec>

CConnecterPluginsTerminal::CConnecterPluginsTerminal(CPluginFactory *parent)
    : CConnecter(parent),
      m_pConsole(nullptr),
      m_pThread(nullptr),
      m_bThread(false),
      m_bExit(false)
{
    m_pConsole = new CFrmTermWidget();
    m_pConsole->setAutoClose(true);
    
    bool check = false;
    check = connect(m_pConsole, SIGNAL(titleChanged()),
                    this, SLOT(slotTerminalTitleChanged()));
    Q_ASSERT(check);
    check = connect(m_pConsole, SIGNAL(finished()),
                    this, SLOT(DisConnect()));
    Q_ASSERT(check);
    check = connect(m_pConsole, SIGNAL(sigZoomReset()),
                    this, SLOT(slotZoomReset()));
    Q_ASSERT(check);
}

CConnecterPluginsTerminal::~CConnecterPluginsTerminal()
{
    qDebug() << "CConnecterPluginsTerminal::~CConnecterPluginsTerminal()";
    
    if(m_pThread)
    {
        m_bExit = true;
        m_pThread->wait();
        delete m_pThread;
        m_pThread = nullptr;
    }
    
    if(m_pConsole)
    {
        delete m_pConsole;
        m_pConsole = nullptr;
    }
}

QWidget* CConnecterPluginsTerminal::GetViewer()
{
    return m_pConsole;
}

qint16 CConnecterPluginsTerminal::Version()
{
    return 0;
}

int CConnecterPluginsTerminal::OpenDialogSettings(QWidget *parent)
{
    int nRet = -1;
    QDialog* p = GetDialogSettings(parent);
    if(p)
    {
        p->setAttribute(Qt::WA_DeleteOnClose);
        return p->exec();
    }
    return nRet;
}

int CConnecterPluginsTerminal::Load(QDataStream &d)
{
    int nRet = 0;
    CParameterTerminal* pPara = GetPara();
    Q_ASSERT(pPara);
    if(!pPara) return -1;

    qint16 version = 0;
    d >> version
      >> *pPara;

    nRet = OnLoad(d);
    return nRet;
}

int CConnecterPluginsTerminal::Save(QDataStream &d)
{
    int nRet = 0;
    
    CParameterTerminal* pPara = GetPara();
    Q_ASSERT(pPara);
    if(!pPara) return -1;
    
    d << Version()
      << *pPara;
    
    nRet = OnSave(d);
    return nRet;
}

int CConnecterPluginsTerminal::Connect()
{
    int nRet = 0;

    nRet = SetParamter();

    if(m_bThread)
    {
        m_pThread = new CConnectThreadTerminal(this);
        if(m_pThread)
            m_pThread->start();
    }
    
    nRet = OnConnect();
    if(nRet < 0)
        emit sigDisconnected();
    else if(0 == nRet)
        emit sigConnected();
    
    if(m_pConsole)
        emit sigServerName(GetServerName());
    
    return nRet;
}

int CConnecterPluginsTerminal::DisConnect()
{
    int nRet = 0;

    if(m_bThread)
        m_bExit = true;
    
    nRet = OnDisConnect();
    
    emit sigDisconnected();
    return nRet;
}

int CConnecterPluginsTerminal::SetParamter()
{
    int nRet = 0;

    CParameterTerminal* pPara = GetPara();
    Q_ASSERT(pPara);
    if(!pPara) return -1;
#if QTERMWIDGET_VERSION >= QT_VERSION_CHECK(0, 9, 0)
    m_pConsole->setTerminalSizeHint(pPara->sizeHint);
    m_pConsole->setBidiEnabled(pPara->biDirectional);
    m_pConsole->disableBracketedPasteMode(pPara->disableBracketedPasteMode);
#endif
    m_pConsole->setTerminalFont(pPara->font);
    m_pConsole->setKeyboardCursorShape(pPara->cursorShape);
    m_pConsole->setColorScheme(pPara->colorScheme);
    m_pConsole->setScrollBarPosition(pPara->scrollBarPosition);
    m_pConsole->setFlowControlEnabled(pPara->flowControl);
    m_pConsole->setFlowControlWarningEnabled(pPara->flowControlWarning);
    m_pConsole->setMotionAfterPasting(pPara->motionAfterPasting);
    m_pConsole->setTerminalOpacity(1.0 - pPara->termTransparency / 100.0);
    m_pConsole->setTerminalBackgroundImage(pPara->backgroupImage);
        
    m_pConsole->setKeyBindings(pPara->szKeyBindings);
    m_pConsole->setTextCodec(QTextCodec::codecForName(pPara->textCodec.toStdString().c_str()));
    m_pConsole->setHistorySize(pPara->historySize);
    
//    m_pConsole->setMonitorActivity(false);
//    m_pConsole->setMonitorSilence(false);
//    m_pConsole->setBlinkingCursor(true);
    

    return nRet;
}

void CConnecterPluginsTerminal::slotTerminalTitleChanged()
{
    m_pConsole->setWindowTitle(m_pConsole->title());
    slotSetServerName(m_pConsole->title());
}

void CConnecterPluginsTerminal::slotZoomReset()
{
    if(m_pConsole)
        m_pConsole->setTerminalFont(GetPara()->font);
}

int CConnecterPluginsTerminal::OnConnect()
{
    return 0;
}

int CConnecterPluginsTerminal::OnDisConnect()
{
    return 0;
}

int CConnecterPluginsTerminal::OnLoad(QDataStream& d)
{
    return 0;
}

int CConnecterPluginsTerminal::OnSave(QDataStream& d)
{
    return 0;
}

QString CConnecterPluginsTerminal::GetServerName()
{
    CParameterTerminal* pPara = GetPara();
    if(m_szServerName.isEmpty())
    {
        if(pPara && !pPara->szHost.isEmpty())
            m_szServerName = pPara->szHost + ":" + QString::number(pPara->nPort);
        else
            return Name();
    }
    return CConnecter::GetServerName();
}

CConnect* CConnecterPluginsTerminal::InstanceConnect()
{
    return nullptr;
}

int CConnecterPluginsTerminal::OnRun()
{
    //LOG_MODEL_DEBUG("CConnecterBackThread", "Current thread: 0x%X", QThread::currentThreadId());
    int nRet = -1;
    CConnect* pConnect = InstanceConnect();
    
    do{
        CConnect* pConnect = InstanceConnect();
        if(nullptr == pConnect) break;
        
        nRet = pConnect->Initialize();
        if(nRet) break;
        
        /**
          nRet < 0 : error
          nRet = 0 : emit sigConnected
          nRet = 1 : emit sigConnected in CConnect
          */
        nRet = pConnect->Connect();
        if(nRet < 0) break;
        if(0 == nRet) emit sigConnected();
        
        while (!m_bExit) {
            try {
                // 0 : continue
                // 1: exit
                // < 0: error
                int nRet = pConnect->Process();
                if(nRet) break;
            }  catch (...) {
                LOG_MODEL_ERROR("ConnecterBackThread", "process fail:%d", nRet);
                break;
            }
        }
        
    }while (0);

    emit sigDisconnected();

    pConnect->Clean();
    delete pConnect;
    qDebug() << "CConnecterPlugins::OnRun() end";
    return nRet;
}
