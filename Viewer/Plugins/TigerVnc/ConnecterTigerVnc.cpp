//! @author: Kang Lin(kl222@126.com)

#include "ConnecterTigerVnc.h"
#include "rfb/encodings.h"
#include <QDebug>

CConnecterTigerVnc::CConnecterTigerVnc(CPluginFactory *parent)
    : CConnecterPlugins(parent)
{
    m_Para.bSave = true;
    m_Para.bShared = true;
    m_Para.bBufferEndRefresh = true;
    m_Para.bLocalCursor = true;
    m_Para.bClipboard = true;
    m_Para.bSupportsDesktopResize = true;
    
    m_Para.bAutoSelect = true;
    m_Para.nColorLevel = CConnectTigerVnc::Full;
    m_Para.nEncoding = rfb::encodingTight;
    m_Para.bCompressLevel = true;
    m_Para.nCompressLevel = 2;
    m_Para.bNoJpeg = false;
    m_Para.nQualityLevel = 8;
}

CConnecterTigerVnc::~CConnecterTigerVnc()
{
    qDebug() << "CConnecterTigerVnc::~CConnecterTigerVnc()";
}

QString CConnecterTigerVnc::GetServerName()
{
    if(m_szServerName.isEmpty())
    {
        return m_Para.szServerName;
    }
    return m_szServerName;
}

qint16 CConnecterTigerVnc::Version()
{
    return 0;
}

QDialog *CConnecterTigerVnc::GetDialogSettings(QWidget *parent)
{
    Q_UNUSED(parent)
    CDlgSettings* p = new CDlgSettings(&m_Para);
    p->setAttribute(Qt::WA_DeleteOnClose);
    return p;
}

int CConnecterTigerVnc::Save(QDataStream & d)
{
    d << Version()
      << m_Para.szServerName
      << m_Para.szUser
      << m_Para.szPassword
      << m_Para.bSave
      << m_Para.bShared
      << m_Para.bBufferEndRefresh
      << m_Para.bLocalCursor
      << m_Para.bSupportsDesktopResize
      << m_Para.bClipboard
      << m_Para.bAutoSelect
      << static_cast<int>(m_Para.nColorLevel)
      << m_Para.nEncoding
      << m_Para.bCompressLevel
      << m_Para.nCompressLevel
      << m_Para.bNoJpeg
      << m_Para.nQualityLevel
         ;
    return 0;
}

int CConnecterTigerVnc::Load(QDataStream &d)
{
    int nColorLevel = CConnectTigerVnc::Full;
    qint16 version = 0;
    d >> version;
    d >> m_Para.szServerName
           >> m_Para.szUser
           >> m_Para.szPassword
           >> m_Para.bSave
           >> m_Para.bShared
           >> m_Para.bBufferEndRefresh
           >> m_Para.bLocalCursor
           >> m_Para.bSupportsDesktopResize
           >> m_Para.bClipboard
           >> m_Para.bAutoSelect
           >> nColorLevel
           >> m_Para.nEncoding
           >> m_Para.bCompressLevel
           >> m_Para.nCompressLevel
           >> m_Para.bNoJpeg
           >> m_Para.nQualityLevel
           ;
    //TODO: if version
    m_Para.nColorLevel = static_cast<CConnectTigerVnc::COLOR_LEVEL>(nColorLevel);
    return 0;
}

CConnect* CConnecterTigerVnc::InstanceConnect()
{
    return new CConnectTigerVnc(this);
}
