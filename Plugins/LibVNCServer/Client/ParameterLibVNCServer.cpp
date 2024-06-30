#include "ParameterLibVNCServer.h"

CParameterLibVNCServer::CParameterLibVNCServer(QObject *parent)
    : CParameterBase(parent)
{
    SetPort(5900);
    SetShared(true);
    
    SetClipboard(true);
    SetEnableCompressLevel(true);
    SetCompressLevel(3);
    SetJpeg(true);
    SetQualityLevel(5);
}

int CParameterLibVNCServer::onLoad(QSettings &set)
{
    return CParameterConnecter::onLoad(set);
}

int CParameterLibVNCServer::onSave(QSettings &set)
{
    return CParameterConnecter::onSave(set);
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
