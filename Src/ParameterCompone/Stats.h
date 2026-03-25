// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QAtomicInteger>
#include <QAtomicInt>
#include <QDateTime>
#include "ParameterOperate.h"

class PLUGIN_EXPORT CStats : public CParameterOperate
{
    Q_OBJECT

public:
    explicit CStats(CParameterOperate* parent = nullptr,
                    const QString& szPrefix = QString());

    [[nodiscard]] static QString Convertbytes(quint64 bytes);

    QString TotalSends();
    QString TotalReceives();
    quint64 GetTotalSends();
    quint64 GetTotalReceives();

    QString SendRate();
    QString ReceiveRate();
    /*!
     * \brief Send rate. The average rate over the interval period. Unit: Byte/Seconds
     * \return
     */
    [[nodiscard]] virtual double GetSendRate();
    [[nodiscard]] virtual double GetReceiveRate();

    /*!
     * \brief Get interval. Unit: Seconds
     */
    [[nodiscard]] int GetInterval();
    /*!
     * \brief Set interval. Unit: Seconds
     */
    int SetInterval(int interval = 1);

Q_SIGNALS:
    /*!
     * \~chinese 当数据发生变化时触发，用于通知用户实时调用 slotCalculating 进行计算。
     * \~english Triggered when data changes,
     *           used to notify users to call slotCalculating(true) in real time for calculation.
     */
    void sigDataChanged();

public Q_SLOTS:
    /*!
     * \brief Calculating
     */
    virtual void slotCalculating();
    virtual void AddSends(quint64 size);
    virtual void AddReceives(quint64 size);

private:
    QAtomicInteger<quint64> m_TotalSends;
    QAtomicInteger<quint64> m_TotalReceives;
    int m_tmInterval;
    QDateTime m_lastTime;
    QAtomicInteger<quint64> m_lastSends;
    QAtomicInteger<quint64> m_lastReceives;
    double m_dbSendRate;
    double m_dbReceiveRate;

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};

/*!
 * \~chinese 安全级别
 * \~english Security level
 */
class PLUGIN_EXPORT CSecurityLevel : QObject {
    Q_OBJECT

public:
    enum Level {
        No = 0x8000,              // No the function
        Risk = 0x00,
        Authentication = 0x01,
        SecureChannel = 0x02,
        Proxy = 0x04,
        Gateway = 0x08,
        Redirect = 0x010,

        SecureMask = Authentication | SecureChannel,   // Green
        NormalMask = Proxy | Gateway | Redirect,       // Yellow
        RiskyMask = ~SecureMask                        // Red
    };
    Q_ENUM(Level)
    Q_DECLARE_FLAGS(Levels, Level)
    Q_FLAG(Levels)

    CSecurityLevel(Levels level = Level::No, QObject* parent = nullptr);
    ~CSecurityLevel();

    [[nodiscard]] virtual Levels GetLevel() const;
    [[nodiscard]] virtual QString GetString() const;
    [[nodiscard]] virtual QColor GetColor() const;
    [[nodiscard]] virtual QString GetUnicodeIcon() const;
    [[nodiscard]] virtual QIcon GetIcon() const;
    [[nodiscard]] static QString GetString(const Levels &level);
    [[nodiscard]] static QString GetUnicodeIcon(const Levels &level);
    [[nodiscard]] static QIcon GetIcon(const Levels &level);
    [[nodiscard]] static QColor GetColor(const Levels &level);

Q_SIGNALS:
    /*!
     * \~chinese 当安全级别改变时触发
     * \~english Triggered when the security level changes
     */
    void sigSecurityLevel(Levels level);

private:
    Levels m_Level;
};

// 在类外部声明操作符（通常放在头文件末尾）
Q_DECLARE_OPERATORS_FOR_FLAGS(CSecurityLevel::Levels)
