#include <QLoggingCategory>
#include "ParameterFilter.h"
#include "DatabaseFilter.h"

static Q_LOGGING_CATEGORY(log, "Client.Parameter.Filter")
CParameterFilter::CParameterFilter(QObject *parent, const QString &szPrefix)
    : CParameter{parent, szPrefix}
    , m_pDatabase(nullptr)
    , m_szSuffix(szPrefix)
{}

CParameterFilter::~CParameterFilter()
{
    if(m_pDatabase) {
        m_pDatabase->CloseDatabase();
        delete m_pDatabase;
        m_pDatabase = nullptr;
    }
}

bool CParameterFilter::InitDatabase(CParameterDatabase *pDB)
{
    if(!pDB) return false;
    m_pDatabase = new CDatabaseFilter(m_szSuffix);
    if(m_pDatabase) {
        QString szConnectName = "connect_filter_"  + m_szSuffix;
        bool ok = m_pDatabase->OpenDatabase(pDB, szConnectName);
        if(!ok) {
            qCritical(log) << "Failed to open database:" << szConnectName;
            return false;
        }
    }
    return true;
}

bool CParameterFilter::contains(const QString &szKey)
{
    if(m_pDatabase)
        return m_pDatabase->contains(szKey);

    if(m_Key.contains(szKey))
        return true;
    return false;
}

int CParameterFilter::AddKey(const QString& szKey)
{
    if(m_pDatabase)
        return m_pDatabase->AddKey(szKey);

    if(contains(szKey))
        return 0;
    m_Key.insert(szKey);
    SetModified(true);
    return 0;
}

int CParameterFilter::RemoveKey(const QString& szKey)
{
    if(m_pDatabase)
        return m_pDatabase->RemoveKey(szKey);

    m_Key.remove(szKey);
    return 0;
}

int CParameterFilter::Clear()
{
    if(m_pDatabase)
        return m_pDatabase->Clear();

    m_Key.clear();
    return 0;
}

bool CParameterFilter::isEmpty()
{
    if(m_pDatabase)
        return m_pDatabase->isEmpty();

    return m_Key.isEmpty();
}

int CParameterFilter::OnProcess(std::function<int (const QString &)> cb, bool bErrExit)
{
    if(!cb) return -1;
    if(m_pDatabase) {
        return m_pDatabase->OnProcess(cb, bErrExit);
    }
    foreach(auto k, m_Key) {
        int nRet = cb(k);
        if(bErrExit && nRet)
            return nRet;
    }
    return 0;
}

int CParameterFilter::OnLoad(QSettings &set)
{
    if(m_pDatabase) return 0;

    QStringList s = set.value("Key").toStringList();
    foreach(auto k, s) {
        AddKey(k);
    }
    return 0;
}

int CParameterFilter::OnSave(QSettings &set)
{
    if(m_pDatabase) return 0;

    QStringList s;
    foreach (auto k, m_Key) {
        s << k;
    }
    set.setValue("Key", s);
    return 0;
}
