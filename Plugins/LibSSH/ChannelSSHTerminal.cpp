// Author: Kang Lin <kl222@126.com>

#include "ChannelSSHTerminal.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Plugin.SSH.Channel")

CChannelSSHTerminal::CChannelSSHTerminal(CBackend *pBackend, CParameterSSH *pPara)
    : CChannelSSH{pBackend, pPara, true, pBackend}
{
    qDebug(log) << Q_FUNC_INFO;
}

CChannelSSHTerminal::~CChannelSSHTerminal()
{
    qDebug(log) << Q_FUNC_INFO;
}

qint64 CChannelSSHTerminal::readData(char *data, qint64 maxlen)
{
    qint64 nRet = 0;
    if(!m_Channel || !ssh_channel_is_open(m_Channel))
        return -1;
    nRet = ssh_channel_read(m_Channel, data, maxlen, 0);
    qDebug(log) << "read data:" << nRet << maxlen;
    return nRet;
}

qint64 CChannelSSHTerminal::writeData(const char *data, qint64 len)
{
    qint64 nRet = 0;
    if(!m_Channel || !ssh_channel_is_open(m_Channel))
        return -1;
    if(len > 0)
        nRet = ssh_channel_write(m_Channel, data, len);
    qDebug(log) << "write data:" << nRet << len;
    return nRet;
}

int CChannelSSHTerminal::OnOpen(ssh_session session)
{
    int nRet = 0;
    QString szErr;

    m_Channel = ssh_channel_new(session);
    if (m_Channel == NULL) {
        szErr = QString("Don't create channel: ") + ssh_get_error(session);
        qCritical(log) << szErr;
        setErrorString(szErr);
        return -1;
    }
    
    if (ssh_channel_open_session(m_Channel)) {
        szErr = QString("Error opening channel: ") + ssh_get_error(session);
        qCritical(log) << szErr;
        setErrorString(szErr);
        return -1;
    }
    
    ssh_channel_request_pty(m_Channel);
    ssh_channel_change_pty_size(m_Channel, m_nColumn, m_nRow);
    qDebug(log) << "row:" << m_nRow << "; column:" << m_nColumn;

    if (ssh_channel_request_shell(m_Channel)) {
        szErr = QString("Requesting shell: ") + ssh_get_error(session);
        qCritical(log) << szErr;
        setErrorString(szErr);
        return -1;
    }

    return 0;
}

void CChannelSSHTerminal::OnClose()
{
    if(m_Channel) {
        if(ssh_channel_is_open(m_Channel)) {
            ssh_channel_close(m_Channel);
        }
        ssh_channel_free(m_Channel);
        m_Channel = NULL;
    }
}

void CChannelSSHTerminal::SetSize(int row, int column)
{
    m_nRow = row;
    m_nColumn = column;
}

int CChannelSSHTerminal::OnProcess(int tm)
{
    int nRet = 0;

    if(!m_Channel || !ssh_channel_is_open(m_Channel)
        || ssh_channel_is_eof(m_Channel)) {
        QString szErr = "The channel is not open";
        qCritical(log) << szErr;
        setErrorString(szErr);
        return -1;
    }

    struct timeval timeout = {0, tm};
    ssh_channel channels[2], channel_out[2];
    channels[0] = m_Channel;
    channels[1] = nullptr;

    fd_set set;
    FD_ZERO(&set);
    socket_t fd = SSH_INVALID_SOCKET;
    if(m_pEvent)
        fd = m_pEvent->GetFd();
    if(SSH_INVALID_SOCKET != fd)
        FD_SET(fd, &set);

    //qDebug(log) << "ssh_select:" << fd;
    nRet = ssh_select(channels, channel_out, fd + 1, &set, &timeout);
    //qDebug(log) << "ssh_select end:" << nRet;
    if(EINTR == nRet)
        return 0;

    if(SSH_OK != nRet) {
        QString szErr;
        szErr = "ssh_channel_select failed: " + QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        qCritical(log) << szErr;
        setErrorString(szErr);
        return -3;
    }

    if(SSH_INVALID_SOCKET != fd && FD_ISSET(fd, &set)) {
        //qDebug(log) << "fires event";
        if(m_pEvent) {
            nRet = m_pEvent->Reset();
            if(nRet) return -4;
        }
    }

    if(!channel_out[0]) {
        //qDebug(log) << "The channel is not select";
        return 0;
    }

    if(ssh_channel_is_eof(m_Channel)) {
        qWarning(log) << "Channel is eof";
        setErrorString(tr("The channel is eof"));
        // Stop
        return -1;
    }

    // Get channel data length
    nRet = ssh_channel_poll(m_Channel, 0);
    //qDebug(log) << "Get channel data length:" << nRet;
    if(SSH_ERROR == nRet)
    {
        QString szErr;
        szErr = "ssh_channel_poll failed. nRet:";
        szErr += QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        setErrorString(szErr);
        qCritical(log) << szErr;
        return -6;
    } else if(SSH_EOF == nRet) {
        // Stop
        return -1;
    } else if(0 > nRet) {
        QString szErr;
        szErr = "ssh_channel_poll failed. nRet:";
        szErr += QString::number(nRet);
        szErr += ssh_get_error(m_Session);
        setErrorString(szErr);
        qCritical(log) << szErr;
        // Error
        return -7;
    } else if(0 == nRet) {
        //qDebug(log) << "The channel has not data";
        return 0;
    }

    emit readyRead();

    return 0;
}
