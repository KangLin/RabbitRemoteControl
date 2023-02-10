#include "ParameterRabbitVNC.h"
#include "rfb/encodings.h"

CParameterRabbitVNC::CParameterRabbitVNC(QObject *parent) : CParameterConnecter(parent)
{
    SetPort(5900);
    SetShared(true);
    SetBufferEndRefresh(false);
    SetSupportsDesktopResize(true);
    
    SetAutoSelect(true);
    SetColorLevel(CParameterRabbitVNC::Full);
    SetEncoding(rfb::encodingTight);
    SetEnableCompressLevel(true);
    SetCompressLevel(2);
    SetNoJpeg(false);
    SetQualityLevel(8);
    
    SetIce(false);
}

int CParameterRabbitVNC::Load(QSettings &set)
{
    int nRet = CParameterConnecter::Load(set);

    SetShared(set.value("RabbitVNC/Shared", GetShared()).toBool());
    SetBufferEndRefresh(set.value("RabbitVNC/BufferEndRefresh",
                                  GetBufferEndRefresh()).toBool());
    SetSupportsDesktopResize(set.value("RabbitVNC/SupportsDesktopResize",
                                       GetSupportsDesktopResize()).toBool());
    SetAutoSelect(set.value("RabbitVNC/AutoSelect", GetAutoSelect()).toBool());
    SetColorLevel(static_cast<COLOR_LEVEL>(set.value("RabbitVNC/ColorLevel",
                                                     GetColorLevel()).toInt()));
    SetEncoding(set.value("RabbitVNC/Encoding", GetEncoding()).toInt());
    SetEnableCompressLevel(set.value("RabbitVNC/EnableCompressLevel",
                               GetEnableCompressLevel()).toBool());
    SetCompressLevel(set.value("RabbitVNC/CompressLevel",
                               GetCompressLevel()).toInt());
    SetNoJpeg(set.value("RabbitVNC/NoJpeg", GetNoJpeg()).toBool());
    SetQualityLevel(set.value("RabbitVNC/QualityLevel", GetQualityLevel()).toInt());
    SetIce(set.value("ICE/Enable", GetIce()).toBool());  
    SetPeerUser(set.value("ICE/Peer/User", GetPeerUser()).toString());

    return nRet;
}

int CParameterRabbitVNC::Save(QSettings &set)
{
    int nRet = CParameterConnecter::Save(set);
    set.setValue("RabbitVNC/Shared", GetShared());
    set.setValue("RabbitVNC/BufferEndRefresh", GetBufferEndRefresh());
    set.setValue("RabbitVNC/SupportsDesktopResize", GetSupportsDesktopResize());
    set.setValue("RabbitVNC/AutoSelect", GetAutoSelect());
    set.setValue("RabbitVNC/ColorLevel", GetColorLevel());
    set.setValue("RabbitVNC/Encoding", GetEncoding());
    set.setValue("RabbitVNC/EnableCompressLevel", GetEnableCompressLevel());
    set.setValue("RabbitVNC/CompressLevel", GetCompressLevel());
    set.setValue("RabbitVNC/NoJpeg", GetNoJpeg());
    set.setValue("RabbitVNC/QualityLevel", GetQualityLevel());
    set.setValue("ICE/Enable", GetIce());
    set.setValue("ICE/Peer/User", GetPeerUser());
    return nRet;
}

bool CParameterRabbitVNC::GetCheckCompleted()
{
    if(GetIce())
    {
        return true;
    }
    
    if(GetHost().isEmpty() || GetPort() <= 0 || GetPassword().isEmpty())
        return false;
    return true;
}

bool CParameterRabbitVNC::GetShared() const
{
    return m_bShared;
}

void CParameterRabbitVNC::SetShared(bool newShared)
{
    if(m_bShared == newShared)
        return;
    m_bShared = newShared;
    SetModified(true);
}

bool CParameterRabbitVNC::GetBufferEndRefresh() const
{
    return m_bBufferEndRefresh;
}

void CParameterRabbitVNC::SetBufferEndRefresh(bool newBufferEndRefresh)
{
    if(m_bBufferEndRefresh == newBufferEndRefresh)
        return;
    m_bBufferEndRefresh = newBufferEndRefresh;
    SetModified(true);
}

bool CParameterRabbitVNC::GetSupportsDesktopResize() const
{
    return m_bSupportsDesktopResize;
}

void CParameterRabbitVNC::SetSupportsDesktopResize(bool newSupportsDesktopResize)
{
    if(m_bSupportsDesktopResize == newSupportsDesktopResize)
    m_bSupportsDesktopResize = newSupportsDesktopResize;
    SetModified(true);
}

bool CParameterRabbitVNC::GetAutoSelect() const
{
    return m_bAutoSelect;
}

void CParameterRabbitVNC::SetAutoSelect(bool newAutoSelect)
{
    if(m_bAutoSelect == newAutoSelect)
        return;
    m_bAutoSelect = newAutoSelect;
    SetModified(true);
}

CParameterRabbitVNC::COLOR_LEVEL CParameterRabbitVNC::GetColorLevel() const
{
    return m_nColorLevel;
}

void CParameterRabbitVNC::SetColorLevel(COLOR_LEVEL newColorLevel)
{
    if(m_nColorLevel == newColorLevel)
        return;
    m_nColorLevel = newColorLevel;
    SetModified(true);
}

int CParameterRabbitVNC::GetEncoding() const
{
    return m_nEncoding;
}

void CParameterRabbitVNC::SetEncoding(int newEncoding)
{
    if(m_nEncoding == newEncoding)
        return;
    m_nEncoding = newEncoding;
    SetModified(true);
}

bool CParameterRabbitVNC::GetEnableCompressLevel() const
{
    return m_bCompressLevel;
}

void CParameterRabbitVNC::SetEnableCompressLevel(bool newCompressLevel)
{
    if(m_bCompressLevel == newCompressLevel)
        return;
    m_bCompressLevel = newCompressLevel;
    SetModified(true);
}

int CParameterRabbitVNC::GetCompressLevel() const
{
    return m_nCompressLevel;
}

void CParameterRabbitVNC::SetCompressLevel(int newCompressLevel)
{
    if(m_nCompressLevel == newCompressLevel)
        return;
    m_nCompressLevel = newCompressLevel;
    SetModified(true);
}

bool CParameterRabbitVNC::GetNoJpeg() const
{
    return m_bNoJpeg;
}

void CParameterRabbitVNC::SetNoJpeg(bool newNoJpeg)
{
    if(m_bNoJpeg == newNoJpeg)
        return;
    m_bNoJpeg = newNoJpeg;
    SetModified(true);
}

int CParameterRabbitVNC::GetQualityLevel() const
{
    return m_nQualityLevel;
}

void CParameterRabbitVNC::SetQualityLevel(int newQualityLevel)
{
    if(m_nQualityLevel == newQualityLevel)
        return;
    m_nQualityLevel = newQualityLevel;
    SetModified(true);
}

bool CParameterRabbitVNC::GetIce() const
{
    return m_bIce;
}

void CParameterRabbitVNC::SetIce(bool newIce)
{
    if(m_bIce == newIce)
        return;
    m_bIce = newIce;
    SetModified(true);
}

const QString &CParameterRabbitVNC::GetPeerUser() const
{
    return m_szPeerUser;
}

void CParameterRabbitVNC::SetPeerUser(const QString &newPeerUser)
{
    if(m_szPeerUser == newPeerUser)
        return;
    m_szPeerUser = newPeerUser;
    SetModified(true);
}
