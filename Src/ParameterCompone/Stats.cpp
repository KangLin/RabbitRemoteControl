// Author: Kang Lin <kl222@126.com>
#include "Stats.h"

CStats::CStats(CParameterOperate *parent, const QString &szPrefix)
    : CParameterOperate{parent}
    , m_tmInterval(1)
    , m_lastTime(QDateTime::currentDateTime())
    , m_dbSendRate(0)
    , m_dbReceiveRate(0)
{
    SetInterval();
}

QString CStats::Convertbytes(quint64 bytes)
{
    QString szBytes;
    if((1 << 10) >= bytes)
        szBytes = QString::number(bytes) + " " + tr("B");
    else if((1 << 20) >= bytes)
        szBytes = QString::number((qreal)bytes / (1 << 10), 'f', 2) + " " + tr("KB");
    else if((1 << 30) >= bytes)
        szBytes = QString::number((qreal)bytes / (1 << 20), 'f', 2) + " " + tr("MB");
    else
        szBytes = QString::number((qreal)bytes / (1 << 30), 'f', 2) + " " + tr("GB");
    return szBytes;
}

QString CStats::TotalSends()
{
    return Convertbytes(GetTotalSends());
}

QString CStats::TotalReceives()
{
    return Convertbytes(GetTotalReceives());
}

quint64 CStats::GetTotalSends()
{
    return m_TotalSends;
}

quint64 CStats::GetTotalReceives()
{
    return m_TotalReceives;
}

void CStats::AddSends(quint64 size)
{
    m_TotalSends += size;
}

void CStats::AddReceives(quint64 size)
{
    m_TotalReceives += size;
}

QString CStats::SendRate()
{
    return Convertbytes(GetSendRate()) + "/" + tr("S");
}

QString CStats::ReceiveRate()
{
    return Convertbytes(GetReceiveRate()) + "/" + tr("S");
}

double CStats::GetSendRate()
{
    return m_dbSendRate;
}

double CStats::GetReceiveRate()
{
    return m_dbReceiveRate;
}

int CStats::GetInterval()
{
    return m_tmInterval;
}

int CStats::SetInterval(int interval)
{
    if(m_tmInterval == interval)
        return m_tmInterval;
    int old = interval;
    m_tmInterval = interval;
    SetModified(true);
    return old;
}

void CStats::slotCalculating()
{
    if(m_lastTime.secsTo(QDateTime::currentDateTime()) < m_tmInterval)
        return;

    m_dbSendRate = (double)(m_TotalSends - m_lastSends) / (double)m_tmInterval;
    m_lastSends = m_TotalSends;
    m_dbReceiveRate = (double)(m_TotalReceives - m_lastReceives) / (double)m_tmInterval;
    m_lastReceives = m_TotalReceives;
}

int CStats::OnLoad(QSettings &set)
{
    return 0;
}

int CStats::OnSave(QSettings &set)
{
    return 0;
}

CSecurityLevel::CSecurityLevel(CSecurityLevel::Level level, QObject* parent)
    : QObject(parent)
    , m_Level(level)
{
}

CSecurityLevel::~CSecurityLevel()
{}

CSecurityLevel::Level CSecurityLevel::GetLevel() const
{
    return m_Level;
}

QString CSecurityLevel::GetString() const
{
    return GetString(GetLevel());
}

QColor CSecurityLevel::GetColor() const
{
    return GetColor(GetLevel());
}

QString CSecurityLevel::GetUnicodeIcon() const
{
    return GetUnicodeIcon(GetLevel());
}

QIcon CSecurityLevel::GetIcon() const
{
    return GetIcon(GetLevel());
}

QString CSecurityLevel::GetString(Level level)
{
    if(Level::No == level)
        return QString();
    if(Level::Secure == level)
        return tr("Secure") + ": "
               + tr("Secure channel") + " + " + tr("Authentication");
    if(Level::Risky == level)
        return tr("Risk");
    if(Level::Secure & level) {
        QString s = tr("Normal") + ": ";
        if(Level::SecureChannel & level)
            s += tr("Secure channel");
        else if(Level::Authentication & level)
            s+= tr("Authentication");
        return s;
    }
    if(Level::Normal & level)
        return tr("Normal");
    return QString();
}

/*!
 * \brief COperate::GetSecurityLevelUnicodeIcon
 * \param level
 * \return
 * | 符号　| Unicode 编码 | 说明 | 含义 |
 * |:----:|-------------|------|------|
 * | 🔓 | `U+1F513` | 开锁 | 未加密、无保护 |
 * | 🔐 | `U+1F510` | 带钥匙的锁（关闭） | 安全（有保护） |
 * | 🔒 | `U+1F512` | 闭锁 | 安全（已锁定） |
 * | 🔏 | `U+1F50F` | 带笔的锁 | 隐私模式 |
 * | ⚠ | `U+26A0` | 警告 | 不安全警告 |
 * | ❌ | `U+274C` | 红叉 | 错误、不安全 |
 * | 🛡 | `U+1F6E1` | 盾牌 | 有保护（安全） |
 * | 🔴 | `U+1F534` | 红圈 | 危险、警告 |
 * | 🟡 | `U+1F7E1` | 黄圈 | 注意、谨慎 |
 * | 🟢 | `U+1F7E2` | 绿圈 | 安全 |
 */
QString CSecurityLevel::GetUnicodeIcon(Level level)
{
    if(Level::No == level)
        return QString();
    if(Level::Secure == level)
        return "🟢🛡🔐";
    if(Level::Risky == level)
        return "🔴";
    if(Level::Secure & level || Level::Normal & level) {
        QString s = "🟡";
        if(Level::SecureChannel & level)
            s += "🛡";
        else if(Level::Authentication & level)
            s+= "🔐";
        return s;
    }
    return QString();
}

QColor CSecurityLevel::GetColor(Level level)
{
    if(Level::No == level)
        return QColor();
    if(Level::Secure == level)
        return Qt::GlobalColor::green;
    if(Level::Risky == level)
        return Qt::GlobalColor::red;
    if(Level::Secure & level)
        return Qt::GlobalColor::yellow;
    if(Level::Normal & level)
        return Qt::GlobalColor::yellow;
    return QColor();
}

QIcon CSecurityLevel::GetIcon(Level level)
{
    if(Level::No == level)
        return QIcon();
    if(Level::Secure == level)
        return QIcon::fromTheme("lock");
    if(Level::Risky == level)
        return QIcon::fromTheme("unlock");
    if(Level::Secure & level)
        return QIcon::fromTheme("dialog-warning");
    if(Level::Normal & level)
        return QIcon::fromTheme("dialog-warning");
    return QIcon();
}
