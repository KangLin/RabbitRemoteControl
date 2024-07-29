#include "ParameterVnc.h"
#include "rfb/encodings.h"

CParameterVnc::CParameterVnc(QObject *parent)
    : CParameterBase(parent),
    m_Proxy(this)
{
    m_Net.SetPort(5900);
    
    m_Net.m_User.SetType((CParameterUser::TYPE)(
        (int)CParameterUser::TYPE::None
        | (int)CParameterUser::TYPE::OnlyPassword));
    m_Net.m_User.SetUsedType(CParameterUser::TYPE::OnlyPassword);

    SetShared(true);
    SetBufferEndRefresh(false);
    SetSupportsDesktopResize(true);
    SetLedState(true);

    SetAutoSelect(true);
    SetColorLevel(CParameterVnc::Full);
    SetPreferredEncoding(rfb::encodingTight);
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
    int nRet = 0;
    nRet = CParameterBase::OnLoad(set);
    if(nRet) return nRet;
    
    set.beginGroup("VNC");
    SetShared(set.value("Shared", GetShared()).toBool());
    SetBufferEndRefresh(set.value("BufferEndRefresh",
                                  GetBufferEndRefresh()).toBool());
    SetSupportsDesktopResize(set.value("SupportsDesktopResize",
                                       GetSupportsDesktopResize()).toBool());
    SetLedState(set.value("LedState", GetLedState()).toBool());
    
    SetAutoSelect(set.value("AutoSelect", GetAutoSelect()).toBool());
    SetColorLevel(static_cast<COLOR_LEVEL>(set.value("ColorLevel",
                                                     GetColorLevel()).toInt()));
    SetPreferredEncoding(set.value("Encoding", GetPreferredEncoding()).toInt());
    SetEnableCompressLevel(set.value("EnableCompressLevel",
                               GetEnableCompressLevel()).toBool());
    SetCompressLevel(set.value("CompressLevel",
                               GetCompressLevel()).toInt());
    SetNoJpeg(set.value("NoJpeg", GetNoJpeg()).toBool());
    SetQualityLevel(set.value("QualityLevel", GetQualityLevel()).toInt());
    set.endGroup();
    
    set.beginGroup("ICE");
    SetIce(set.value("Enable", GetIce()).toBool());
    SetSignalServer(set.value("Signal/Server",
                              GetSignalServer()).toString());
    SetSignalPort(set.value("Signal/Port", GetSignalPort()).toUInt());
    SetSignalUser(set.value("Signal/User", GetSignalUser()).toString());
    if(m_Net.m_User.GetSavePassword())
    {
        QString szPassword;
        if(!LoadPassword(tr("Ice signal password"), "Signal/password",
                         szPassword, set))
            SetSignalPassword(szPassword);
    }
    SetPeerUser(set.value("Peer/User", GetPeerUser()).toString());
    SetStunServer(set.value("Stun/Server", GetStunServer()).toString());
    SetStunPort(set.value("Stun/Port", GetStunPort()).toUInt());
    SetTurnServer(set.value("Turn/Server", GetTurnServer()).toString());
    SetTurnPort(set.value("Turn/Port", GetTurnPort()).toUInt());
    SetTurnUser(set.value("Turn/User", GetTurnUser()).toString());
    if(m_Net.m_User.GetSavePassword())
    {
        QString szPassword;
        if(!LoadPassword(tr("Ice turn password"), "Turn/password",
                         szPassword, set))
            SetTurnPassword(szPassword);
    }
    set.endGroup();
    
    return 0;
}

int CParameterVnc::OnSave(QSettings &set)
{
    int nRet = 0;
    nRet = CParameterBase::OnSave(set);
    if(nRet) return nRet;

    set.beginGroup("VNC");
    set.setValue("Shared", GetShared());
    set.setValue("BufferEndRefresh", GetBufferEndRefresh());
    set.setValue("SupportsDesktopResize", GetSupportsDesktopResize());
    set.setValue("LedState", GetLedState());
    set.setValue("AutoSelect", GetAutoSelect());
    set.setValue("ColorLevel", GetColorLevel());
    set.setValue("Encoding", GetPreferredEncoding());
    set.setValue("EnableCompressLevel", GetEnableCompressLevel());
    set.setValue("CompressLevel", GetCompressLevel());
    set.setValue("NoJpeg", GetNoJpeg());
    set.setValue("QualityLevel", GetQualityLevel());
    set.endGroup();
    
    set.beginGroup("ICE");
    set.setValue("Enable", GetIce());
    set.setValue("Signal/Server", GetSignalServer());
    set.setValue("Signal/Port", GetSignalPort());
    set.setValue("Signal/User", GetSignalUser());
    SavePassword("Signal/password", GetSignalPassword(), set);
    set.setValue("Peer/User", GetPeerUser());
    set.setValue("Stun/Server", GetStunServer());
    set.setValue("Stun/Port", GetStunPort());
    set.setValue("Turn/Server", GetTurnServer());
    set.setValue("Turn/Port", GetTurnPort());
    set.setValue("Turn/User", GetTurnUser());
    SavePassword("Turn/password", GetTurnPassword(), set);
    set.endGroup();
    
    return 0;
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

bool CParameterVnc::GetLedState() const
{
    return m_bLedState;
}

void CParameterVnc::SetLedState(bool state)
{
    if(m_bLedState == state)
        return;
    m_bLedState = state;
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

int CParameterVnc::GetPreferredEncoding() const
{
    return m_nPreferredEncoding;
}

void CParameterVnc::SetPreferredEncoding(int newEncoding)
{
    if(m_nPreferredEncoding == newEncoding)
        return;
    m_nPreferredEncoding = newEncoding;
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
