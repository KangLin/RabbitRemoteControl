#include "ConnecterTelnet.h"
#include "DlgSettingsTelnet.h"

#include <unistd.h>

CConnecterTelnet::CConnecterTelnet(CPluginFactory *parent)
    : CConnecterPluginsTerminal(parent),
      m_pPara(nullptr),
      m_pSocket(nullptr)
{
    m_pPara = new CParameterTelnet();
    if(m_pPara)
    {
        m_pPara->nPort = 23;
        m_pPara->font = m_pConsole->getTerminalFont();
        m_pPara->colorScheme = "Linux";
        m_pPara->scrollBarPosition = QTermWidget::NoScrollBar;
    }
}

QDialog *CConnecterTelnet::GetDialogSettings(QWidget *parent)
{
    return new CDlgSettingsTelnet(m_pPara, parent);
}

CParameterTerminalAppearance *CConnecterTelnet::GetPara()
{
    return m_pPara;
}

int CConnecterTelnet::SetParamter()
{
    return CConnecterPluginsTerminal::SetParamter();
}

int CConnecterTelnet::OnConnect()
{
    int nRet = 0;
    
    Q_ASSERT(!m_pSocket);
    m_pSocket = new QTcpSocket(this);
    
    // Write what we input to remote terminal via socket
    bool check = connect(m_pConsole, SIGNAL(sendData(const char *,int)),
            this, SLOT(slotSendData(const char *,int)));
    Q_ASSERT(check);
    
    // Read anything from remote terminal via socket and show it on widget.
    check = connect(m_pSocket, SIGNAL(readyRead()),
                    this, SLOT(slotReadyRead()));
    Q_ASSERT(check);
    
    connect(m_pSocket, SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(atError()));

    // Here we start an empty pty.
    m_pConsole->startTerminalTeletype();

    m_pSocket->connectToHost(m_pPara->szHost, m_pPara->nPort);
    
    return nRet; 
}

int CConnecterTelnet::OnDisConnect()
{
    int nRet = 0;
    
    if(m_pSocket)
    {
        m_pSocket->close();
        delete m_pSocket;
        m_pSocket = nullptr;
    }
    return nRet;
}

int CConnecterTelnet::OnLoad(QDataStream &d)
{
    int nRet = 0;
    
    return nRet;
}

int CConnecterTelnet::OnSave(QDataStream &d)
{
    int nRet = 0;
    
    return nRet;
}

void CConnecterTelnet::slotSendData(const char *data, int size)
{
    if(m_pSocket)
        m_pSocket->write(data, size);
}

void CConnecterTelnet::slotReadyRead()
{
    if(m_pSocket && m_pConsole)
    {
        QByteArray data = m_pSocket->readAll();
        write(m_pConsole->getPtySlaveFd(), data.data(), data.size());
    }
}
