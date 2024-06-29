#include "Parameter.h"
#include "RabbitCommonDir.h"

CParameter::CParameter(QObject *parent)
    : QObject(parent),
    m_bModified(false)
{}

CParameter::CParameter(CParameter* parent, const QString& szPrefix)
    : CParameter(parent)
{
    if(parent) {
        parent->AddMember(this);
    }
    SetPrefix(szPrefix);
}

CParameter::~CParameter()
{}

QString CParameter::GetPrefix() const
{
    return m_szPrefix;
}

int CParameter::SetPrefix(const QString& szName)
{
    m_szPrefix = szName;
    return 0;
}

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

    if(!GetPrefix().isEmpty())
        set.beginGroup(GetPrefix());

    nRet = onLoad(set);

    if(!nRet) {
        foreach (auto p, m_Member) {
            nRet = p->Load(set);
            if(nRet) break;
        }
    }

    if(!GetPrefix().isEmpty())
        set.endGroup();
    return nRet;
}

int CParameter::Save(QSettings &set, bool bForce)
{
    int nRet = 0;
    if(GetModified()) emit sigChanged();
    if(!GetModified() && !bForce) return 0;

    if(!GetPrefix().isEmpty())
        set.beginGroup(GetPrefix());

    nRet = onSave(set);

    if(!nRet) {
        foreach (auto p, m_Member) {
            nRet = p->Save(set);
            if(nRet) break;
        }
    }

    if(!GetPrefix().isEmpty())
        set.endGroup();
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
