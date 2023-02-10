#include "ParameterTigerVnc.h"
#include "rfb/encodings.h"

CParameterTigerVnc::CParameterTigerVnc(QObject *parent) : CParameterConnecter(parent)
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
    int nRet = CParameterConnecter::Load(set);

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
    if(GetSavePassword())
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
    if(GetSavePassword())
    {
        QString szPassword;
        if(!LoadPassword(tr("Ice turn password"), "ICE/Turn/password",
                         szPassword, set))
            SetTurnPassword(szPassword);
    }

    return nRet;
}

int CParameterTigerVnc::Save(QSettings &set)
{
    int nRet = CParameterConnecter::Save(set);
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

bool CParameterTigerVnc::GetCheckCompleted()
{
    if(GetIce())
    {
        if(GetSignalServer().isEmpty() || GetSignalPort() <= 0
                || GetSignalUser().isEmpty() || GetSignalPassword().isEmpty()
                || GetStunServer().isEmpty() || GetStunPort() <= 0)
            return false;
        return true;
    }
    
    if(GetHost().isEmpty() || GetPort() <= 0 || GetPassword().isEmpty())
        return false;
    return true;
}

bool CParameterTigerVnc::GetShared() const
{
    return m_bShared;
}

void CParameterTigerVnc::SetShared(bool newShared)
{
    if(m_bShared == newShared)
        return;
    m_bShared = newShared;
    SetModified(true);
}

bool CParameterTigerVnc::GetBufferEndRefresh() const
{
    return m_bBufferEndRefresh;
}

void CParameterTigerVnc::SetBufferEndRefresh(bool newBufferEndRefresh)
{
    if(m_bBufferEndRefresh == newBufferEndRefresh)
        return;
    m_bBufferEndRefresh = newBufferEndRefresh;
    SetModified(true);
}

bool CParameterTigerVnc::GetSupportsDesktopResize() const
{
    return m_bSupportsDesktopResize;
}

void CParameterTigerVnc::SetSupportsDesktopResize(bool newSupportsDesktopResize)
{
    if(m_bSupportsDesktopResize == newSupportsDesktopResize)
        return;
    m_bSupportsDesktopResize = newSupportsDesktopResize;
    SetModified(true);
}

bool CParameterTigerVnc::GetAutoSelect() const
{
    return m_bAutoSelect;
}

void CParameterTigerVnc::SetAutoSelect(bool newAutoSelect)
{
    if(m_bAutoSelect == newAutoSelect)
        return;
    m_bAutoSelect = newAutoSelect;
    SetModified(true);
}

CParameterTigerVnc::COLOR_LEVEL CParameterTigerVnc::GetColorLevel() const
{
    return m_nColorLevel;
}

void CParameterTigerVnc::SetColorLevel(COLOR_LEVEL newColorLevel)
{
    if(m_nColorLevel == newColorLevel)
        return;
    m_nColorLevel = newColorLevel;
    SetModified(true);
}

int CParameterTigerVnc::GetEncoding() const
{
    return m_nEncoding;
}

void CParameterTigerVnc::SetEncoding(int newEncoding)
{
    if(m_nEncoding == newEncoding)
        return;
    m_nEncoding = newEncoding;
    SetModified(true);
}

bool CParameterTigerVnc::GetEnableCompressLevel() const
{
    return m_bCompressLevel;
}

void CParameterTigerVnc::SetEnableCompressLevel(bool newCompressLevel)
{
    if(m_bCompressLevel == newCompressLevel)
        return;
    m_bCompressLevel = newCompressLevel;
    SetModified(true);
}

int CParameterTigerVnc::GetCompressLevel() const
{
    return m_nCompressLevel;
}

void CParameterTigerVnc::SetCompressLevel(int newCompressLevel)
{
    if(m_nCompressLevel == newCompressLevel)
        return;
    m_nCompressLevel = newCompressLevel;
    SetModified(true);
}

bool CParameterTigerVnc::GetNoJpeg() const
{
    return m_bNoJpeg;
}

void CParameterTigerVnc::SetNoJpeg(bool newNoJpeg)
{
    if(m_bNoJpeg == newNoJpeg)
        return;
    m_bNoJpeg = newNoJpeg;
    SetModified(true);
}

int CParameterTigerVnc::GetQualityLevel() const
{
    return m_nQualityLevel;
}

void CParameterTigerVnc::SetQualityLevel(int newQualityLevel)
{
    if(m_nQualityLevel == newQualityLevel)
        return;
    m_nQualityLevel = newQualityLevel;
    SetModified(true);
}

bool CParameterTigerVnc::GetIce() const
{
    return m_bIce;
}

void CParameterTigerVnc::SetIce(bool newIce)
{
    if(m_bIce == newIce)
        return;
    m_bIce = newIce;
    SetModified(true);
}

const QString &CParameterTigerVnc::GetSignalServer() const
{
    return m_szSignalServer;
}

void CParameterTigerVnc::SetSignalServer(const QString &newSignalServer)
{
    if(m_szSignalServer == newSignalServer)
        return;
    m_szSignalServer = newSignalServer;
    SetModified(true);
}

quint16 CParameterTigerVnc::GetSignalPort() const
{
    return m_nSignalPort;
}

void CParameterTigerVnc::SetSignalPort(quint16 newSignalPort)
{
    if(m_nSignalPort == newSignalPort)
        return;
    m_nSignalPort = newSignalPort;
    SetModified(true);
}

const QString &CParameterTigerVnc::GetSignalUser() const
{
    return m_szSignalUser;
}

void CParameterTigerVnc::SetSignalUser(const QString &newSignalUser)
{
    if(m_szSignalUser == newSignalUser)
        return;
    m_szSignalUser = newSignalUser;
    SetModified(true);
}

const QString &CParameterTigerVnc::GetSignalPassword() const
{
    return m_szSignalPassword;
}

void CParameterTigerVnc::SetSignalPassword(const QString &newSignalPassword)
{
    if(m_szSignalPassword == newSignalPassword)
        return;
    m_szSignalPassword = newSignalPassword;
    SetModified(true);
}

const QString &CParameterTigerVnc::GetPeerUser() const
{
    return m_szPeerUser;
}

void CParameterTigerVnc::SetPeerUser(const QString &newPeerUser)
{
    if(m_szPeerUser == newPeerUser)
        return;
    m_szPeerUser = newPeerUser;
    SetModified(true);
}

const QString &CParameterTigerVnc::GetStunServer() const
{
    return m_szStunServer;
}

void CParameterTigerVnc::SetStunServer(const QString &newStunServer)
{
    if(m_szStunServer == newStunServer)
        return;
    m_szStunServer = newStunServer;
    SetModified(true);
}

quint16 CParameterTigerVnc::GetStunPort() const
{
    return m_nStunPort;
}

void CParameterTigerVnc::SetStunPort(quint16 newStunPort)
{
    if(m_nStunPort == newStunPort)
        return;
    m_nStunPort = newStunPort;
    SetModified(true);
}

const QString &CParameterTigerVnc::GetTurnServer() const
{
    return m_szTurnServer;
}

void CParameterTigerVnc::SetTurnServer(const QString &newTurnServer)
{
    if(m_szTurnServer == newTurnServer)
        return;
    m_szTurnServer = newTurnServer;
    SetModified(true);
}

quint16 CParameterTigerVnc::GetTurnPort() const
{
    return m_nTurnPort;
}

void CParameterTigerVnc::SetTurnPort(quint16 newTurnPort)
{
    if(m_nTurnPort == newTurnPort)
        return;
    m_nTurnPort = newTurnPort;
    SetModified(true);
}

const QString &CParameterTigerVnc::GetTurnUser() const
{
    return m_szTurnUser;
}

void CParameterTigerVnc::SetTurnUser(const QString &newTurnUser)
{
    if(m_szTurnUser == newTurnUser)
        return;
    m_szTurnUser = newTurnUser;
    SetModified(true);
}

const QString &CParameterTigerVnc::GetTurnPassword() const
{
    return m_szTurnPassword;
}

void CParameterTigerVnc::SetTurnPassword(const QString &newTurnPassword)
{
    if(m_szTurnPassword == newTurnPassword)
        return;
    m_szTurnPassword = newTurnPassword;
    SetModified(true);
}
