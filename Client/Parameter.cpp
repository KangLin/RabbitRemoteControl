#include "Parameter.h"
#include "RabbitCommonDir.h"

CParameter::CParameter(QObject *parent)
    : QObject(parent),
    m_bModified(false)
{
    CParameter* p = qobject_cast<CParameter*>(parent);
    if(p) {
        p->AddMember(this);
    }
}

CParameter::~CParameter()
{}

int CParameter::Load(QString szFile)
{
    if(szFile.isEmpty())
        szFile = RabbitCommon::CDir::Instance()->GetFileUserConfigure();
    if(szFile.isEmpty()) return -1;
    QSettings set(szFile, QSettings::IniFormat);
    int nRet = Load(set);
    SetModified(false);
    return nRet;
}

int CParameter::Save(QString szFile, bool bForce)
{
    if(!GetModified() && !bForce) return 0;
    if(szFile.isEmpty())
        szFile = RabbitCommon::CDir::Instance()->GetFileUserConfigure();
    if(szFile.isEmpty()) return -1;
    QSettings set(szFile, QSettings::IniFormat);
    int nRet = Save(set);
    SetModified(false);
    return nRet;
}

int CParameter::Load(QSettings &set)
{
    int nRet = 0;
    foreach (auto p, m_Member) {
        nRet = p->Load(set);
        if(nRet) return nRet;
    }
    nRet = onLoad(set);
    return nRet;
}

int CParameter::Save(QSettings &set, bool bForce)
{
    int nRet = 0;
    if(GetModified()) emit sigChanged();
    if(!GetModified() && !bForce) return 0;
    foreach (auto p, m_Member) {
        nRet = p->Save(set);
        if(nRet) return nRet;
    }
    nRet = onSave(set);
    return nRet;
}

bool CParameter::CheckCompleted()
{
    return true;
}

bool CParameter::GetModified()
{
    return m_bModified;
}

int CParameter::SetModified(bool bModified)
{
    if(m_bModified == bModified)
        return 0;
    m_bModified = bModified;
    return 0;
}

int CParameter::AddMember(CParameter* p)
{
    if(!m_Member.contains(p))
        m_Member.push_back(p);
    return 0;
}
