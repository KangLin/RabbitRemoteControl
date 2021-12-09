#include "ParameterTigerVnc.h"
#include "rfb/encodings.h"

CParameterTigerVnc::CParameterTigerVnc(QObject *parent) : CParameter(parent)
{
    SetPort(5900);
    SetShared(true);
    SetBufferEndRefresh(false);
    SetSupportsDesktopResize(true);
    
    SetAutoSelect(true);
    SetColorLevel(CParameterTigerVnc::Full);
    SetEncoding(rfb::encodingTight);
    SetEnableCompressLevel(true);
    SetCompressLevel(2);
    SetNoJpeg(false);
    SetQualityLevel(8);
    
    SetIce(false);
    SetSignalPort(5222);
    SetStunPort(3748);
    SetTurnPort(3748);
}

int CParameterTigerVnc::Load(QSettings &set)
{
    SetShared(set.value("TigerVNC/Shared", GetShared()).toBool());
    SetBufferEndRefresh(set.value("TigerVNC/BufferEndRefresh",
                                  GetBufferEndRefresh()).toBool());
    SetSupportsDesktopResize(set.value("TigerVNC/SupportsDesktopResize",
                                       GetSupportsDesktopResize()).toBool());
    SetAutoSelect(set.value("TigerVNC/AutoSelect", GetAutoSelect()).toBool());
    SetColorLevel(static_cast<COLOR_LEVEL>(set.value("TigerVNC/ColorLevel",
                                                     GetColorLevel()).toInt()));
    SetEncoding(set.value("TigerVNC/Encoding", GetEncoding()).toInt());
    SetEnableCompressLevel(set.value("TigerVNC/EnableCompressLevel",
                               GetEnableCompressLevel()).toBool());
    SetCompressLevel(set.value("TigerVNC/CompressLevel",
                               GetCompressLevel()).toInt());
    SetNoJpeg(set.value("TigerVNC/NoJpeg", GetNoJpeg()).toBool());
    SetQualityLevel(set.value("TigerVNC/QualityLevel", GetQualityLevel()).toInt());
    SetIce(set.value("ICE/Enable", GetIce()).toBool());
    SetSignalServer(set.value("ICE/Signal/Server",
                              GetSignalServer()).toString());
    SetSignalPort(set.value("ICE/Signal/Port", GetSignalPort()).toUInt());
    SetSignalUser(set.value("ICE/Signal/User", GetSignalUser()).toString());
    SetSignalPassword(set.value("ICE/Signal/password",
                                GetSignalPassword()).toString());
    SetPeerUser(set.value("ICE/Peer/User", GetPeerUser()).toString());
    SetStunServer(set.value("ICE/Stun/Server", GetStunServer()).toString());
    SetStunPort(set.value("ICE/Stun/Port", GetStunPort()).toUInt());
    SetTurnServer(set.value("ICE/Turn/Server", GetTurnServer()).toString());
    SetTurnPort(set.value("ICE/Turn/Port", GetTurnPort()).toUInt());
    SetTurnUser(set.value("ICE/Turn/User", GetTurnUser()).toString());
    SetTurnPassword(set.value("ICE/Turn/password",
                              GetTurnPassword()).toString());
    return CParameter::Load(set);
}

int CParameterTigerVnc::Save(QSettings &set)
{
    set.setValue("TigerVNC/Shared", GetShared());
    set.setValue("TigerVNC/BufferEndRefresh", GetBufferEndRefresh());
    set.setValue("TigerVNC/SupportsDesktopResize", GetSupportsDesktopResize());
    set.setValue("TigerVNC/AutoSelect", GetAutoSelect());
    set.setValue("TigerVNC/ColorLevel", GetColorLevel());
    set.setValue("TigerVNC/Encoding", GetEncoding());
    set.setValue("TigerVNC/EnableCompressLevel", GetEnableCompressLevel());
    set.setValue("TigerVNC/CompressLevel", GetCompressLevel());
    set.setValue("TigerVNC/NoJpeg", GetNoJpeg());
    set.setValue("TigerVNC/QualityLevel", GetQualityLevel());
    set.setValue("ICE/Enable", GetIce());
    set.setValue("ICE/Signal/Server", GetSignalServer());
    set.setValue("ICE/Signal/Port", GetSignalPort());
    set.setValue("ICE/Signal/User", GetSignalUser());
    set.setValue("ICE/Signal/password", GetSignalPassword());
    set.setValue("ICE/Peer/User", GetPeerUser());
    set.setValue("ICE/Stun/Server", GetStunServer());
    set.setValue("ICE/Stun/Port", GetStunPort());
    set.setValue("ICE/Turn/Server", GetTurnServer());
    set.setValue("ICE/Turn/Port", GetTurnPort());
    set.setValue("ICE/Turn/User", GetTurnUser());
    set.setValue("ICE/Turn/password", GetTurnPassword());
    return CParameter::Save(set);
}

bool CParameterTigerVnc::GetShared() const
{
    return m_bShared;
}

void CParameterTigerVnc::SetShared(bool newShared)
{
    m_bShared = newShared;
}

bool CParameterTigerVnc::GetBufferEndRefresh() const
{
    return m_bBufferEndRefresh;
}

void CParameterTigerVnc::SetBufferEndRefresh(bool newBufferEndRefresh)
{
    m_bBufferEndRefresh = newBufferEndRefresh;
}

bool CParameterTigerVnc::GetSupportsDesktopResize() const
{
    return m_bSupportsDesktopResize;
}

void CParameterTigerVnc::SetSupportsDesktopResize(bool newSupportsDesktopResize)
{
    m_bSupportsDesktopResize = newSupportsDesktopResize;
}

bool CParameterTigerVnc::GetAutoSelect() const
{
    return m_bAutoSelect;
}

void CParameterTigerVnc::SetAutoSelect(bool newAutoSelect)
{
    m_bAutoSelect = newAutoSelect;
}

CParameterTigerVnc::COLOR_LEVEL CParameterTigerVnc::GetColorLevel() const
{
    return m_nColorLevel;
}

void CParameterTigerVnc::SetColorLevel(COLOR_LEVEL newColorLevel)
{
    m_nColorLevel = newColorLevel;
}

int CParameterTigerVnc::GetEncoding() const
{
    return m_nEncoding;
}

void CParameterTigerVnc::SetEncoding(int newEncoding)
{
    m_nEncoding = newEncoding;
}

bool CParameterTigerVnc::GetEnableCompressLevel() const
{
    return m_bCompressLevel;
}

void CParameterTigerVnc::SetEnableCompressLevel(bool newCompressLevel)
{
    m_bCompressLevel = newCompressLevel;
}

int CParameterTigerVnc::GetCompressLevel() const
{
    return m_nCompressLevel;
}

void CParameterTigerVnc::SetCompressLevel(int newCompressLevel)
{
    m_nCompressLevel = newCompressLevel;
}

bool CParameterTigerVnc::GetNoJpeg() const
{
    return m_bNoJpeg;
}

void CParameterTigerVnc::SetNoJpeg(bool newNoJpeg)
{
    m_bNoJpeg = newNoJpeg;
}

int CParameterTigerVnc::GetQualityLevel() const
{
    return m_nQualityLevel;
}

void CParameterTigerVnc::SetQualityLevel(int newQualityLevel)
{
    m_nQualityLevel = newQualityLevel;
}

bool CParameterTigerVnc::GetIce() const
{
    return m_bIce;
}

void CParameterTigerVnc::SetIce(bool newIce)
{
    m_bIce = newIce;
}

const QString &CParameterTigerVnc::GetSignalServer() const
{
    return m_szSignalServer;
}

void CParameterTigerVnc::SetSignalServer(const QString &newSignalServer)
{
    m_szSignalServer = newSignalServer;
}

quint16 CParameterTigerVnc::GetSignalPort() const
{
    return m_nSignalPort;
}

void CParameterTigerVnc::SetSignalPort(quint16 newSignalPort)
{
    m_nSignalPort = newSignalPort;
}

const QString &CParameterTigerVnc::GetSignalUser() const
{
    return m_szSignalUser;
}

void CParameterTigerVnc::SetSignalUser(const QString &newSignalUser)
{
    m_szSignalUser = newSignalUser;
}

const QString &CParameterTigerVnc::GetSignalPassword() const
{
    return m_szSignalPassword;
}

void CParameterTigerVnc::SetSignalPassword(const QString &newSignalPassword)
{
    m_szSignalPassword = newSignalPassword;
}

const QString &CParameterTigerVnc::GetPeerUser() const
{
    return m_szPeerUser;
}

void CParameterTigerVnc::SetPeerUser(const QString &newPeerUser)
{
    m_szPeerUser = newPeerUser;
}

const QString &CParameterTigerVnc::GetStunServer() const
{
    return m_szStunServer;
}

void CParameterTigerVnc::SetStunServer(const QString &newStunServer)
{
    m_szStunServer = newStunServer;
}

quint16 CParameterTigerVnc::GetStunPort() const
{
    return m_nStunPort;
}

void CParameterTigerVnc::SetStunPort(quint16 newStunPort)
{
    m_nStunPort = newStunPort;
}

const QString &CParameterTigerVnc::GetTurnServer() const
{
    return m_szTurnServer;
}

void CParameterTigerVnc::SetTurnServer(const QString &newTurnServer)
{
    m_szTurnServer = newTurnServer;
}

quint16 CParameterTigerVnc::GetTurnPort() const
{
    return m_nTurnPort;
}

void CParameterTigerVnc::SetTurnPort(quint16 newTurnPort)
{
    m_nTurnPort = newTurnPort;
}

const QString &CParameterTigerVnc::GetTurnUser() const
{
    return m_szTurnUser;
}

void CParameterTigerVnc::SetTurnUser(const QString &newTurnUser)
{
    m_szTurnUser = newTurnUser;
}

const QString &CParameterTigerVnc::GetTurnPassword() const
{
    return m_szTurnPassword;
}

void CParameterTigerVnc::SetTurnPassword(const QString &newTurnPassword)
{
    m_szTurnPassword = newTurnPassword;
}
