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
    virtual void AddSends(quint64 size);
    virtual void AddReceives(quint64 size);

    QString SendRate();
    QString ReceiveRate();
    /*!
     * \brief Send rate. The average rate over the interval period. Unit: Byte/Seconds
     * \return
     */
    [[nodiscard]] virtual double GetSendRate();
    [[nodiscard]] virtual double GetReceiveRate();

    /*!
     * \brief Get interval. Unit: seconds
     */
    [[nodiscard]] int GetInterval();
    /*!
     * \brief Set interval. Unit: seconds
     */
    int SetInterval(int interval = 5);

public Q_SLOTS:
    virtual void slotCalculating();

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
