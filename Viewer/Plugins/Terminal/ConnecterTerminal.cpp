#include "ConnecterTerminal.h"
#include "DlgSettingsTerminal.h"
#include "RabbitCommonLog.h"

#include <QDialog>
#include <QApplication>
#include <QDebug>

CConnecterTerminal::CConnecterTerminal(CPluginFactory *parent)
    : CConnecter(parent)
{
    m_pConsole = new QTermWidget(0);
    bool check = false;
    check = connect(m_pConsole, SIGNAL(titleChanged()),
                    this, SLOT(slotTerminalTitleChanged()));
    Q_ASSERT(check);
    check = connect(m_pConsole, SIGNAL(finished()),
                    this, SLOT(DisConnect()));
    Q_ASSERT(check);
    
    m_Para.font = m_pConsole->getTerminalFont();
    m_Para.colorScheme = "GreenOnBlack";
    m_Para.scrollBarPosition = QTermWidget::NoScrollBar;
}

CConnecterTerminal::~CConnecterTerminal()
{
    qDebug() << "CConnecterTerminal::~CConnecterTerminal()";
    if(m_pConsole) delete m_pConsole;
}

QWidget* CConnecterTerminal::GetViewer()
{
    return m_pConsole;
}

qint16 CConnecterTerminal::Version()
{
    return 0;
}

int CConnecterTerminal::OpenDialogSettings(QWidget *parent)
{
    int nRet = -1;
    QDialog* p = GetDialogSettings(parent);
    if(p)
    {
        return p->exec();
    }
    return nRet;
}

QDialog* CConnecterTerminal::GetDialogSettings(QWidget *parent)
{
    return new CDlgSettingsTerminal(&m_Para, parent);
}

int CConnecterTerminal::Load(QDataStream &d)
{
    int nRet = 0;
    qint16 version = 0;
    d >> version;
    d >> m_Para.szHost
      >> m_Para.nPort
      >> m_Para.szUser
      >> m_Para.bSavePassword;
    
    if(m_Para.bSavePassword)
        d >> m_Para.szPassword;
    
    d >> m_Para.font
      >> m_Para.colorScheme
      >> m_Para.termTransparency
      >> m_Para.flowControl
      ;
    
    int cursorShape = 0;
    d >> cursorShape;
    m_Para.cursorShape = static_cast<Konsole::Emulation::KeyboardCursorShape>(cursorShape);
    int scrollBarPosition = 0;
    d >> scrollBarPosition;
    m_Para.scrollBarPosition = static_cast<QTermWidget::ScrollBarPosition>(scrollBarPosition);
    
    nRet = OnLoad(d);
    return nRet;
}

int CConnecterTerminal::Save(QDataStream &d)
{
    int nRet = 0;
    d << Version()
      << m_Para.szHost
      << m_Para.nPort
      << m_Para.szUser
      << m_Para.bSavePassword;
    
    if(m_Para.bSavePassword)
        d << m_Para.szPassword;
    
    d << m_Para.font
      << m_Para.colorScheme
      << m_Para.termTransparency
      << m_Para.flowControl
      << (int)m_Para.cursorShape
      << (int)m_Para.scrollBarPosition
         ;
    
    nRet = OnSave(d);
    return nRet;
}

int CConnecterTerminal::Connect()
{
    int nRet = 0;
    
    nRet = SetParamter();

    if(m_pConsole)
        m_pConsole->startShellProgram();
    
    nRet = OnConnect();
    
    emit sigConnected();

    if(m_pConsole)
        emit sigServerName("Terminal");
    
    return nRet;
}

int CConnecterTerminal::DisConnect()
{
    int nRet = 0;

    if(m_pConsole) m_pConsole->close();
    
    nRet = OnDisConnect();
    
    emit sigDisconnected();
    
    return nRet;
}

int CConnecterTerminal::SetParamter()
{
    int nRet = 0;

    m_pConsole->setTerminalFont(m_Para.font);
    m_pConsole->setKeyboardCursorShape(m_Para.cursorShape);
    m_pConsole->setColorScheme(m_Para.colorScheme);
    m_pConsole->setScrollBarPosition(m_Para.scrollBarPosition);

    m_pConsole->setTerminalOpacity(1.0 - m_Para.termTransparency / 100.0);
    
    //m_pConsole->setMotionAfterPasting(1);
    //m_pConsole->disableBracketedPasteMode(true);
    return nRet;
}

void CConnecterTerminal::slotTerminalTitleChanged()
{
    m_pConsole->setWindowTitle(m_pConsole->title());
    slotSetServerName(m_pConsole->title());
}

int CConnecterTerminal::OnConnect()
{
    return 0;
}

int CConnecterTerminal::OnDisConnect()
{
    return 0;
}

int CConnecterTerminal::OnLoad(QDataStream& d)
{
    return 0;
}

int CConnecterTerminal::OnSave(QDataStream& d)
{
    return 0;
}
