#include "Parameter.h"
#include "RabbitCommonDir.h"

#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Client.Parameter")

CParameter::CParameter(QObject* parent, const QString& szPrefix)
    : QObject(parent),
    m_bModified(false),
    m_szPrefix(szPrefix)
{
    CParameter* p = qobject_cast<CParameter*>(parent);
    if(p) {
        p->AddCategory(this);
    }
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

    nRet = OnLoad(set);

    if(!nRet) {
        foreach (auto p, m_Category) {
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
    if(!GetModified() && !bForce) return 0;

    if(!GetPrefix().isEmpty())
        set.beginGroup(GetPrefix());

    nRet = OnSave(set);

    if(!nRet) {
        foreach (auto p, m_Category) {
            nRet = p->Save(set);
            if(nRet) break;
        }
    }

    if(!GetPrefix().isEmpty())
        set.endGroup();
    return nRet;
}

bool CParameter::CheckValidity()
{
    bool bRet = false;
    foreach (auto p, m_Category) {
        bRet = p->OnCheckValidity();
        if(!bRet) break;
    }
    return bRet;
}

bool CParameter::OnCheckValidity()
{
    qDebug(log) << " Not implemented CParameter::onCheckValidity()";
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

int CParameter::AddCategory(CParameter* p)
{
    if(!m_Category.contains(p))
        m_Category.push_back(p);
    return 0;
}
