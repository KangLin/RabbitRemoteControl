// Author: Kang Lin <kl222@126.com>

#include "ParameterSSH.h"

CParameterSSH::CParameterSSH(CParameterOperate *parent, const QString &szPrefix)
    : CParameterOperate(parent, szPrefix)
    , m_Net(this)
{
    m_Net.SetPort(22);
}

int CParameterSSH::OnLoad(QSettings &set)
{
    SetPcapFile(set.value("PacpFile", GetPcapFile()).toString());
    return 0;
}

int CParameterSSH::OnSave(QSettings &set)
{
    set.setValue("PacpFile", GetPcapFile());
    return 0;
}

QString CParameterSSH::GetPcapFile() const
{
    return m_pcapFile;
}

int CParameterSSH::SetPcapFile(const QString &szFile)
{
    m_pcapFile = szFile;
    return 0;
}

CParameterSSHTunnel::CParameterSSHTunnel(CParameterOperate *parent, const QString &szPrefix)
    : CParameterSSH(parent, szPrefix)
    , m_szSourceHost("localhost")
    , m_nSourcePort(0)
{}

QString CParameterSSHTunnel::GetSourceHost() const
{
    return m_szSourceHost;
}

int CParameterSSHTunnel::SetSourceHost(const QString szHost)
{
    m_szSourceHost = szHost;
    return 0;
}

quint16 CParameterSSHTunnel::GetSourcePort() const
{
    return m_nSourcePort;
}

int CParameterSSHTunnel::SetSourcePort(const quint16 nPort)
{
    m_nSourcePort = nPort;
    return 0;
}

int CParameterSSHTunnel::OnLoad(QSettings &set)
{
    SetSourceHost(set.value("SourceHost", GetSourceHost()).toString());
    SetSourcePort(set.value("SourcePort", GetSourcePort()).toUInt());
    return 0;
}

int CParameterSSHTunnel::OnSave(QSettings &set)
{
    set.setValue("SourceHost", GetSourceHost());
    set.setValue("SourcePort", GetSourcePort());
    return 0;
}
