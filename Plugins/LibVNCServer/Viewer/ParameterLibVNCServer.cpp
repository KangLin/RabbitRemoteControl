#include "ParameterLibVNCServer.h"

CParameterLibVNCServer::CParameterLibVNCServer(QObject *parent) : CParameterConnecter(parent)
{
    SetPort(5900);
    SetShared(true);
    
    SetClipboard(true);
    SetEnableCompressLevel(true);
    SetCompressLevel(3);
    SetJpeg(true);
    SetQualityLevel(5);
}

int CParameterLibVNCServer::Load(QSettings &set)
{
    return CParameterConnecter::Load(set);
}

int CParameterLibVNCServer::Save(QSettings &set)
{
    return CParameterConnecter::Save(set);
}

int CParameterLibVNCServer::GetQualityLevel() const
{
    return m_bQualityLevel;
}

void CParameterLibVNCServer::SetQualityLevel(int newQualityLevel)
{
    m_bQualityLevel = newQualityLevel;
}

bool CParameterLibVNCServer::GetJpeg() const
{
    return m_bJpeg;
}

void CParameterLibVNCServer::SetJpeg(bool newJpeg)
{
    m_bJpeg = newJpeg;
}

int CParameterLibVNCServer::GetCompressLevel() const
{
    return m_nCompressLevel;
}

void CParameterLibVNCServer::SetCompressLevel(int newCompressLevel)
{
    m_nCompressLevel = newCompressLevel;
}

bool CParameterLibVNCServer::GetEnableCompressLevel() const
{
    return m_bCompressLevel;
}

void CParameterLibVNCServer::SetEnableCompressLevel(bool newCompressLevel)
{
    m_bCompressLevel = newCompressLevel;
}

bool CParameterLibVNCServer::GetShared() const
{
    return m_bShared;
}

void CParameterLibVNCServer::SetShared(bool newShared)
{
    m_bShared = newShared;
}
