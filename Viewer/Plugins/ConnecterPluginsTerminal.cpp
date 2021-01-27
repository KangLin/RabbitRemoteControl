#include "ConnecterPluginsTerminal.h"
#include "RabbitCommonLog.h"

#include <QDialog>
#include <QApplication>
#include <QDebug>

CConnecterPluginsTerminal::CConnecterPluginsTerminal(CPluginFactory *parent)
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
}

CConnecterPluginsTerminal::~CConnecterPluginsTerminal()
{
    qDebug() << "CConnecterPluginsTerminal::~CConnecterPluginsTerminal()";
    if(m_pConsole) delete m_pConsole;
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
        return p->exec();
    }
    return nRet;
}

int CConnecterPluginsTerminal::Load(QDataStream &d)
{
    int nRet = 0;
    CParameterTerminalAppearance* pPara = GetPara();
    Q_ASSERT(pPara);
    if(!pPara) return -1;
    
    qint16 version = 0;
    d >> version;
    d >> pPara->szHost
      >> pPara->nPort
      >> pPara->szUser
      >> pPara->bSavePassword;
    
    if(pPara->bSavePassword)
        d >> pPara->szPassword;
    
    d >> pPara->font
      >> pPara->colorScheme
      >> pPara->termTransparency
      >> pPara->flowControl
      ;
    
    int cursorShape = 0;
    d >> cursorShape;
    pPara->cursorShape = static_cast<Konsole::Emulation::KeyboardCursorShape>(cursorShape);
    int scrollBarPosition = 0;
    d >> scrollBarPosition;
    pPara->scrollBarPosition = static_cast<QTermWidget::ScrollBarPosition>(scrollBarPosition);
    
    d >> pPara->backgroupImage;
    
    nRet = OnLoad(d);
    return nRet;
}

int CConnecterPluginsTerminal::Save(QDataStream &d)
{
    int nRet = 0;
    
    CParameterTerminalAppearance* pPara = GetPara();
    Q_ASSERT(pPara);
    if(!pPara) return -1;
    
    d << Version()
      << pPara->szHost
      << pPara->nPort
      << pPara->szUser
      << pPara->bSavePassword;
    
    if(pPara->bSavePassword)
        d << pPara->szPassword;
    
    d << pPara->font
      << pPara->colorScheme
      << pPara->termTransparency
      << pPara->flowControl
      << (int)pPara->cursorShape
      << (int)pPara->scrollBarPosition
      << pPara->backgroupImage
         ;
    
    nRet = OnSave(d);
    return nRet;
}

int CConnecterPluginsTerminal::Connect()
{
    int nRet = 0;
    
    nRet = SetParamter();

    if(m_pConsole)
        m_pConsole->startShellProgram();
    
    nRet = OnConnect();
    
    emit sigConnected();

    if(m_pConsole)
        emit sigServerName(Name());
    
    return nRet;
}

int CConnecterPluginsTerminal::DisConnect()
{
    int nRet = 0;

    if(m_pConsole) m_pConsole->close();
    
    nRet = OnDisConnect();
    
    emit sigDisconnected();
    
    return nRet;
}

int CConnecterPluginsTerminal::SetParamter()
{
    int nRet = 0;

    CParameterTerminalAppearance* pPara = GetPara();
    Q_ASSERT(pPara);
    if(!pPara) return -1;
    
    m_pConsole->setTerminalFont(pPara->font);
    m_pConsole->setKeyboardCursorShape(pPara->cursorShape);
    m_pConsole->setColorScheme(pPara->colorScheme);
    m_pConsole->setScrollBarPosition(pPara->scrollBarPosition);

    m_pConsole->setTerminalOpacity(1.0 - pPara->termTransparency / 100.0);
    m_pConsole->setTerminalBackgroundImage(pPara->backgroupImage);
    
    //m_pConsole->setMotionAfterPasting(1);
    //m_pConsole->disableBracketedPasteMode(true);
    return nRet;
}

void CConnecterPluginsTerminal::slotTerminalTitleChanged()
{
    m_pConsole->setWindowTitle(m_pConsole->title());
    slotSetServerName(m_pConsole->title());
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
