#include "ParameterRabbitVNC.h"
#include "rfb/encodings.h"

CParameterRabbitVNC::CParameterRabbitVNC(QObject *parent) : CParameter(parent)
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
    int nRet = CParameter::Load(set);

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
    int nRet = CParameter::Save(set);
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
    m_bShared = newShared;
}

bool CParameterRabbitVNC::GetBufferEndRefresh() const
{
    return m_bBufferEndRefresh;
}

void CParameterRabbitVNC::SetBufferEndRefresh(bool newBufferEndRefresh)
{
    m_bBufferEndRefresh = newBufferEndRefresh;
}

bool CParameterRabbitVNC::GetSupportsDesktopResize() const
{
    return m_bSupportsDesktopResize;
}

void CParameterRabbitVNC::SetSupportsDesktopResize(bool newSupportsDesktopResize)
{
    m_bSupportsDesktopResize = newSupportsDesktopResize;
}

bool CParameterRabbitVNC::GetAutoSelect() const
{
    return m_bAutoSelect;
}

void CParameterRabbitVNC::SetAutoSelect(bool newAutoSelect)
{
    m_bAutoSelect = newAutoSelect;
}

CParameterRabbitVNC::COLOR_LEVEL CParameterRabbitVNC::GetColorLevel() const
{
    return m_nColorLevel;
}

void CParameterRabbitVNC::SetColorLevel(COLOR_LEVEL newColorLevel)
{
    m_nColorLevel = newColorLevel;
}

int CParameterRabbitVNC::GetEncoding() const
{
    return m_nEncoding;
}

void CParameterRabbitVNC::SetEncoding(int newEncoding)
{
    m_nEncoding = newEncoding;
}

bool CParameterRabbitVNC::GetEnableCompressLevel() const
{
    return m_bCompressLevel;
}

void CParameterRabbitVNC::SetEnableCompressLevel(bool newCompressLevel)
{
    m_bCompressLevel = newCompressLevel;
}

int CParameterRabbitVNC::GetCompressLevel() const
{
    return m_nCompressLevel;
}

void CParameterRabbitVNC::SetCompressLevel(int newCompressLevel)
{
    m_nCompressLevel = newCompressLevel;
}

bool CParameterRabbitVNC::GetNoJpeg() const
{
    return m_bNoJpeg;
}

void CParameterRabbitVNC::SetNoJpeg(bool newNoJpeg)
{
    m_bNoJpeg = newNoJpeg;
}

int CParameterRabbitVNC::GetQualityLevel() const
{
    return m_nQualityLevel;
}

void CParameterRabbitVNC::SetQualityLevel(int newQualityLevel)
{
    m_nQualityLevel = newQualityLevel;
}

bool CParameterRabbitVNC::GetIce() const
{
    return m_bIce;
}

void CParameterRabbitVNC::SetIce(bool newIce)
{
    m_bIce = newIce;
}

const QString &CParameterRabbitVNC::GetPeerUser() const
{
    return m_szPeerUser;
}

void CParameterRabbitVNC::SetPeerUser(const QString &newPeerUser)
{
    m_szPeerUser = newPeerUser;
}
