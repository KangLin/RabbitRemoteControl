#include "ConnecterTerminal.h"
#include "DlgSettingsTerminal.h"

#include <QDialog>
#include <QApplication>

CConnecterTerminal::CConnecterTerminal(CPluginFactory *parent)
    : CConnecter(parent)
{
    bool check = false;
    check = connect(&m_Console, SIGNAL(titleChanged()),
                    this, SLOT(slotTerminalTitleChanged()));
    Q_ASSERT(check);
}

CConnecterTerminal::~CConnecterTerminal()
{}

QWidget* CConnecterTerminal::GetViewer()
{
    return &m_Console;
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
    return new CDlgSettingsTerminal(parent);
}

int CConnecterTerminal::Load(QDataStream &d)
{
    int nRet = 0;
    
    return nRet;
}

int CConnecterTerminal::Save(QDataStream &d)
{
    int nRet = 0;
    
    return nRet;
}

int CConnecterTerminal::Connect()
{
    int nRet = 0;
    
    nRet = SetParamter();
    
    return nRet;
}

int CConnecterTerminal::DisConnect()
{
    int nRet = 0;
    
    return nRet;
}

int CConnecterTerminal::SetParamter()
{
    int nRet = 0;
    
    QFont font = QApplication::font();
#ifdef Q_OS_MACOS
    font.setFamily(QStringLiteral("Monaco"));
#elif defined(Q_WS_QWS)
    font.setFamily(QStringLiteral("fixed"));
#else
    font.setFamily(QStringLiteral("Monospace"));
#endif
    font.setPointSize(12);

    m_Console.setTerminalFont(font);

   // console->setColorScheme(COLOR_SCHEME_BLACK_ON_LIGHT_YELLOW);
    m_Console.setScrollBarPosition(QTermWidget::ScrollBarRight);
    
    return nRet;
}

void CConnecterTerminal::slotTerminalTitleChanged()
{
    slotSetServerName(m_Console.title());
}
