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
    SetSignalPort(5222);
    SetStunPort(3748);
    SetTurnPort(3748);
}

int CParameterRabbitVNC::Load(QSettings &set)
{
    int nRet = CParameter::Load(set);

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

int CParameterRabbitVNC::Save(QSettings &set)
{
    int nRet = CParameter::Save(set);
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

bool CParameterRabbitVNC::GetCheckCompleted()
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

const QString &CParameterRabbitVNC::GetSignalServer() const
{
    return m_szSignalServer;
}

void CParameterRabbitVNC::SetSignalServer(const QString &newSignalServer)
{
    m_szSignalServer = newSignalServer;
}

quint16 CParameterRabbitVNC::GetSignalPort() const
{
    return m_nSignalPort;
}

void CParameterRabbitVNC::SetSignalPort(quint16 newSignalPort)
{
    m_nSignalPort = newSignalPort;
}

const QString &CParameterRabbitVNC::GetSignalUser() const
{
    return m_szSignalUser;
}

void CParameterRabbitVNC::SetSignalUser(const QString &newSignalUser)
{
    m_szSignalUser = newSignalUser;
}

const QString &CParameterRabbitVNC::GetSignalPassword() const
{
    return m_szSignalPassword;
}

void CParameterRabbitVNC::SetSignalPassword(const QString &newSignalPassword)
{
    m_szSignalPassword = newSignalPassword;
}

const QString &CParameterRabbitVNC::GetPeerUser() const
{
    return m_szPeerUser;
}

void CParameterRabbitVNC::SetPeerUser(const QString &newPeerUser)
{
    m_szPeerUser = newPeerUser;
}

const QString &CParameterRabbitVNC::GetStunServer() const
{
    return m_szStunServer;
}

void CParameterRabbitVNC::SetStunServer(const QString &newStunServer)
{
    m_szStunServer = newStunServer;
}

quint16 CParameterRabbitVNC::GetStunPort() const
{
    return m_nStunPort;
}

void CParameterRabbitVNC::SetStunPort(quint16 newStunPort)
{
    m_nStunPort = newStunPort;
}

const QString &CParameterRabbitVNC::GetTurnServer() const
{
    return m_szTurnServer;
}

void CParameterRabbitVNC::SetTurnServer(const QString &newTurnServer)
{
    m_szTurnServer = newTurnServer;
}

quint16 CParameterRabbitVNC::GetTurnPort() const
{
    return m_nTurnPort;
}

void CParameterRabbitVNC::SetTurnPort(quint16 newTurnPort)
{
    m_nTurnPort = newTurnPort;
}

const QString &CParameterRabbitVNC::GetTurnUser() const
{
    return m_szTurnUser;
}

void CParameterRabbitVNC::SetTurnUser(const QString &newTurnUser)
{
    m_szTurnUser = newTurnUser;
}

const QString &CParameterRabbitVNC::GetTurnPassword() const
{
    return m_szTurnPassword;
}

void CParameterRabbitVNC::SetTurnPassword(const QString &newTurnPassword)
{
    m_szTurnPassword = newTurnPassword;
}
