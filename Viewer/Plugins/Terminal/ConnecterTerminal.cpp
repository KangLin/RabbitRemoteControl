#include "ConnecterTerminal.h"
#include "DlgSettingsTerminal.h"

#include <QDialog>
#include <QApplication>
#include <QDebug>

CConnecterTerminal::CConnecterTerminal(CPluginFactory *parent)
    : CConnecter(parent)
{
    m_pConsole = new QTermWidget();
    bool check = false;
    check = connect(m_pConsole, SIGNAL(titleChanged()),
                    this, SLOT(slotTerminalTitleChanged()));
    Q_ASSERT(check);
    check = connect(m_pConsole, SIGNAL(finished()),
                    this, SLOT(DisConnect()));
    Q_ASSERT(check);
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
    return QDialog::Accepted;
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
    
    emit sigConnected();
    
    return nRet;
}

int CConnecterTerminal::DisConnect()
{
    int nRet = 0;
    
    if(m_pConsole) m_pConsole->close();
    
    emit sigDisconnected();
    
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

    m_pConsole->setTerminalFont(font);

    //m_pConsole->setColorScheme(COLOR_SCHEME_BLACK_ON_LIGHT_YELLOW);
    m_pConsole->setScrollBarPosition(QTermWidget::ScrollBarRight);
    
    return nRet;
}

void CConnecterTerminal::slotTerminalTitleChanged()
{
    m_pConsole->setWindowTitle(m_pConsole->title());
    slotSetServerName(m_pConsole->title());
}
