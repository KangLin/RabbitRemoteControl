#include "ParameterVnc.h"
#include "rfb/encodings.h"

CParameterVnc::CParameterVnc(QObject *parent)
    : CParameterBase(parent),
    m_Proxy(this)
{
    m_Net.SetPort(5900);

    SetShared(true);
    SetBufferEndRefresh(false);
    SetSupportsDesktopResize(true);
    
    SetAutoSelect(true);
    SetColorLevel(CParameterVnc::Full);
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

int CParameterVnc::OnLoad(QSettings &set)
{
    int nRet = CParameterBase::OnLoad(set);
    if(nRet) return nRet;

    SetShared(set.value("VNC/Shared", GetShared()).toBool());
    SetBufferEndRefresh(set.value("VNC/BufferEndRefresh",
                                  GetBufferEndRefresh()).toBool());
    SetSupportsDesktopResize(set.value("VNC/SupportsDesktopResize",
                                       GetSupportsDesktopResize()).toBool());
    SetAutoSelect(set.value("VNC/AutoSelect", GetAutoSelect()).toBool());
    SetColorLevel(static_cast<COLOR_LEVEL>(set.value("VNC/ColorLevel",
                                                     GetColorLevel()).toInt()));
    SetEncoding(set.value("VNC/Encoding", GetEncoding()).toInt());
    SetEnableCompressLevel(set.value("VNC/EnableCompressLevel",
                               GetEnableCompressLevel()).toBool());
    SetCompressLevel(set.value("VNC/CompressLevel",
                               GetCompressLevel()).toInt());
    SetNoJpeg(set.value("VNC/NoJpeg", GetNoJpeg()).toBool());
    SetQualityLevel(set.value("VNC/QualityLevel", GetQualityLevel()).toInt());
    SetIce(set.value("ICE/Enable", GetIce()).toBool());
    SetSignalServer(set.value("ICE/Signal/Server",
                              GetSignalServer()).toString());
    SetSignalPort(set.value("ICE/Signal/Port", GetSignalPort()).toUInt());
    SetSignalUser(set.value("ICE/Signal/User", GetSignalUser()).toString());
    if(m_Net.m_User.GetSavePassword())
    {
        QString szPassword;
        if(!LoadPassword(tr("Ice signal password"), "ICE/Signal/password",
                         szPassword, set))
            SetSignalPassword(szPassword);
    }
    SetPeerUser(set.value("ICE/Peer/User", GetPeerUser()).toString());
    SetStunServer(set.value("ICE/Stun/Server", GetStunServer()).toString());
    SetStunPort(set.value("ICE/Stun/Port", GetStunPort()).toUInt());
    SetTurnServer(set.value("ICE/Turn/Server", GetTurnServer()).toString());
    SetTurnPort(set.value("ICE/Turn/Port", GetTurnPort()).toUInt());
    SetTurnUser(set.value("ICE/Turn/User", GetTurnUser()).toString());
    if(m_Net.m_User.GetSavePassword())
    {
        QString szPassword;
        if(!LoadPassword(tr("Ice turn password"), "ICE/Turn/password",
                         szPassword, set))
            SetTurnPassword(szPassword);
    }

    return nRet;
}

int CParameterVnc::OnSave(QSettings &set)
{
    int nRet = CParameterBase::OnSave(set);
    if(nRet) return nRet;
    
    set.setValue("VNC/Shared", GetShared());
    set.setValue("VNC/BufferEndRefresh", GetBufferEndRefresh());
    set.setValue("VNC/SupportsDesktopResize", GetSupportsDesktopResize());
    set.setValue("VNC/AutoSelect", GetAutoSelect());
    set.setValue("VNC/ColorLevel", GetColorLevel());
    set.setValue("VNC/Encoding", GetEncoding());
    set.setValue("VNC/EnableCompressLevel", GetEnableCompressLevel());
    set.setValue("VNC/CompressLevel", GetCompressLevel());
    set.setValue("VNC/NoJpeg", GetNoJpeg());
    set.setValue("VNC/QualityLevel", GetQualityLevel());
    set.setValue("ICE/Enable", GetIce());
    set.setValue("ICE/Signal/Server", GetSignalServer());
    set.setValue("ICE/Signal/Port", GetSignalPort());
    set.setValue("ICE/Signal/User", GetSignalUser());
    SavePassword("ICE/Signal/password", GetSignalPassword(), set);
    set.setValue("ICE/Peer/User", GetPeerUser());
    set.setValue("ICE/Stun/Server", GetStunServer());
    set.setValue("ICE/Stun/Port", GetStunPort());
    set.setValue("ICE/Turn/Server", GetTurnServer());
    set.setValue("ICE/Turn/Port", GetTurnPort());
    set.setValue("ICE/Turn/User", GetTurnUser());
    SavePassword("ICE/Turn/password", GetTurnPassword(), set);
    return nRet;
}

bool CParameterVnc::OnCheckValidity()
{
    if(GetIce())
    {
        if(GetSignalServer().isEmpty() || GetSignalPort() <= 0
                || GetSignalUser().isEmpty() || GetSignalPassword().isEmpty()
                || GetStunServer().isEmpty() || GetStunPort() <= 0)
            return false;
        return true;
    }
    
    if(m_Net.GetHost().isEmpty()
        || m_Net.GetPort() <= 0
        || m_Net.m_User.GetPassword().isEmpty())
        return false;
    return true;
}

bool CParameterVnc::GetShared() const
{
    return m_bShared;
}

void CParameterVnc::SetShared(bool newShared)
{
    if(m_bShared == newShared)
        return;
    m_bShared = newShared;
    SetModified(true);
}

bool CParameterVnc::GetBufferEndRefresh() const
{
    return m_bBufferEndRefresh;
}

void CParameterVnc::SetBufferEndRefresh(bool newBufferEndRefresh)
{
    if(m_bBufferEndRefresh == newBufferEndRefresh)
        return;
    m_bBufferEndRefresh = newBufferEndRefresh;
    SetModified(true);
}

bool CParameterVnc::GetSupportsDesktopResize() const
{
    return m_bSupportsDesktopResize;
}

void CParameterVnc::SetSupportsDesktopResize(bool newSupportsDesktopResize)
{
    if(m_bSupportsDesktopResize == newSupportsDesktopResize)
        return;
    m_bSupportsDesktopResize = newSupportsDesktopResize;
    SetModified(true);
}

bool CParameterVnc::GetAutoSelect() const
{
    return m_bAutoSelect;
}

void CParameterVnc::SetAutoSelect(bool newAutoSelect)
{
    if(m_bAutoSelect == newAutoSelect)
        return;
    m_bAutoSelect = newAutoSelect;
    SetModified(true);
}

CParameterVnc::COLOR_LEVEL CParameterVnc::GetColorLevel() const
{
    return m_nColorLevel;
}

void CParameterVnc::SetColorLevel(COLOR_LEVEL newColorLevel)
{
    if(m_nColorLevel == newColorLevel)
        return;
    m_nColorLevel = newColorLevel;
    SetModified(true);
}

int CParameterVnc::GetEncoding() const
{
    return m_nEncoding;
}

void CParameterVnc::SetEncoding(int newEncoding)
{
    if(m_nEncoding == newEncoding)
        return;
    m_nEncoding = newEncoding;
    SetModified(true);
}

bool CParameterVnc::GetEnableCompressLevel() const
{
    return m_bCompressLevel;
}

void CParameterVnc::SetEnableCompressLevel(bool newCompressLevel)
{
    if(m_bCompressLevel == newCompressLevel)
        return;
    m_bCompressLevel = newCompressLevel;
    SetModified(true);
}

int CParameterVnc::GetCompressLevel() const
{
    return m_nCompressLevel;
}

void CParameterVnc::SetCompressLevel(int newCompressLevel)
{
    if(m_nCompressLevel == newCompressLevel)
        return;
    m_nCompressLevel = newCompressLevel;
    SetModified(true);
}

bool CParameterVnc::GetNoJpeg() const
{
    return m_bNoJpeg;
}

void CParameterVnc::SetNoJpeg(bool newNoJpeg)
{
    if(m_bNoJpeg == newNoJpeg)
        return;
    m_bNoJpeg = newNoJpeg;
    SetModified(true);
}

int CParameterVnc::GetQualityLevel() const
{
    return m_nQualityLevel;
}

void CParameterVnc::SetQualityLevel(int newQualityLevel)
{
    if(m_nQualityLevel == newQualityLevel)
        return;
    m_nQualityLevel = newQualityLevel;
    SetModified(true);
}

bool CParameterVnc::GetIce() const
{
    return m_bIce;
}

void CParameterVnc::SetIce(bool newIce)
{
    if(m_bIce == newIce)
        return;
    m_bIce = newIce;
    SetModified(true);
}

const QString &CParameterVnc::GetSignalServer() const
{
    return m_szSignalServer;
}

void CParameterVnc::SetSignalServer(const QString &newSignalServer)
{
    if(m_szSignalServer == newSignalServer)
        return;
    m_szSignalServer = newSignalServer;
    SetModified(true);
}

quint16 CParameterVnc::GetSignalPort() const
{
    return m_nSignalPort;
}

void CParameterVnc::SetSignalPort(quint16 newSignalPort)
{
    if(m_nSignalPort == newSignalPort)
        return;
    m_nSignalPort = newSignalPort;
    SetModified(true);
}

const QString &CParameterVnc::GetSignalUser() const
{
    return m_szSignalUser;
}

void CParameterVnc::SetSignalUser(const QString &newSignalUser)
{
    if(m_szSignalUser == newSignalUser)
        return;
    m_szSignalUser = newSignalUser;
    SetModified(true);
}

const QString &CParameterVnc::GetSignalPassword() const
{
    return m_szSignalPassword;
}

void CParameterVnc::SetSignalPassword(const QString &newSignalPassword)
{
    if(m_szSignalPassword == newSignalPassword)
        return;
    m_szSignalPassword = newSignalPassword;
    SetModified(true);
}

const QString &CParameterVnc::GetPeerUser() const
{
    return m_szPeerUser;
}

void CParameterVnc::SetPeerUser(const QString &newPeerUser)
{
    if(m_szPeerUser == newPeerUser)
        return;
    m_szPeerUser = newPeerUser;
    SetModified(true);
}

const QString &CParameterVnc::GetStunServer() const
{
    return m_szStunServer;
}

void CParameterVnc::SetStunServer(const QString &newStunServer)
{
    if(m_szStunServer == newStunServer)
        return;
    m_szStunServer = newStunServer;
    SetModified(true);
}

quint16 CParameterVnc::GetStunPort() const
{
    return m_nStunPort;
}

void CParameterVnc::SetStunPort(quint16 newStunPort)
{
    if(m_nStunPort == newStunPort)
        return;
    m_nStunPort = newStunPort;
    SetModified(true);
}

const QString &CParameterVnc::GetTurnServer() const
{
    return m_szTurnServer;
}

void CParameterVnc::SetTurnServer(const QString &newTurnServer)
{
    if(m_szTurnServer == newTurnServer)
        return;
    m_szTurnServer = newTurnServer;
    SetModified(true);
}

quint16 CParameterVnc::GetTurnPort() const
{
    return m_nTurnPort;
}

void CParameterVnc::SetTurnPort(quint16 newTurnPort)
{
    if(m_nTurnPort == newTurnPort)
        return;
    m_nTurnPort = newTurnPort;
    SetModified(true);
}

const QString &CParameterVnc::GetTurnUser() const
{
    return m_szTurnUser;
}

void CParameterVnc::SetTurnUser(const QString &newTurnUser)
{
    if(m_szTurnUser == newTurnUser)
        return;
    m_szTurnUser = newTurnUser;
    SetModified(true);
}

const QString &CParameterVnc::GetTurnPassword() const
{
    return m_szTurnPassword;
}

void CParameterVnc::SetTurnPassword(const QString &newTurnPassword)
{
    if(m_szTurnPassword == newTurnPassword)
        return;
    m_szTurnPassword = newTurnPassword;
    SetModified(true);
}
