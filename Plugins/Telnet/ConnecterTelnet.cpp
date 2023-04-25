#include "ConnecterTelnet.h"
#include "DlgSettingsTelnet.h"

#include <unistd.h>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(Telnet)

CConnecterTelnet::CConnecterTelnet(CPluginClient *parent)
    : CConnecterTerminal(parent),
      m_pSocket(nullptr)
{
    m_pPara = new CParameterTelnet();
    if(m_pPara)
    {
        m_pPara->SetPort(23);
    }
}

CConnecterTelnet::~CConnecterTelnet()
{}

QDialog *CConnecterTelnet::GetDialogSettings(QWidget *parent)
{
    return new CDlgSettingsTelnet(m_pPara, parent);
}

int CConnecterTelnet::SetParameter()
{
    return CConnecterTerminal::SetParameter();
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
    
    check = connect(m_pSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                    this, SLOT(slotError(QAbstractSocket::SocketError)));
    Q_ASSERT(check);
    
    check = connect(m_pSocket, SIGNAL(disconnected()),
                    this, SLOT(DisConnect()));
    Q_ASSERT(check);
    
    // Here we start an empty pty.
    m_pConsole->startTerminalTeletype();

    m_pSocket->connectToHost(m_pPara->GetHost(), m_pPara->GetPort());
    
    return nRet; 
}

int CConnecterTelnet::OnDisConnect()
{
    int nRet = 0;
    
    if(m_pSocket)
    {
        if(m_pSocket->state() == QTcpSocket::ConnectedState)
            m_pSocket->close();
        delete m_pSocket;
        m_pSocket = nullptr;
    }
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
        if(!data.isEmpty())
            write(m_pConsole->getPtySlaveFd(), data.data(), data.size());
    }
}

void CConnecterTelnet::slotError(QAbstractSocket::SocketError err)
{
    QString szErr;
    if(!m_pSocket) return;
    szErr = m_pSocket->errorString();
    emit sigError(err, szErr);
    qCritical(Telnet) << "error:" << err << "-" << szErr;
    if(err == QTcpSocket::ConnectionRefusedError
            || err == QTcpSocket::HostNotFoundError)
        DisConnect();
}
