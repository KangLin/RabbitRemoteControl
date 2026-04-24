// Author: Kang Lin <kl222@126.com>

#pragma once
#include <QObject>
#include <QDateTime>
#include <QTimer>
#include "plugin_export.h"
#include "ThreadPool.h"

/*!
 * \brief 应用使用统计信息
 * 
 * 当前使用下载相应资源文件来统计应用的使用信息。
 * 相关的资源位于：　App/Client/Resource/Stats/
 * 在 CI 中安装资源 .github/workflows/build.yml 中安装。
 * 
 */
class PLUGIN_EXPORT CStatsAppUsage : public QObject
{
    Q_OBJECT
public:
    explicit CStatsAppUsage(const QString& szVersion = QString(),
                            const QString& szUrl = QString(),
                            QObject *parent = nullptr);
    virtual ~CStatsAppUsage();

    void SetUrl(const QString& szUrl);
    //! Semantic Versioning: https://semver.org/
    void SetVersion(const QString& szVersion);

public Q_SLOTS:
    void Start();
    void Stop();

public Q_SLOTS:
    void slotTimeout();
Q_SIGNALS:
    void sigFinished();

private:
    void StartOnce();
    void StopOnce();
    void StartDay();
    void StopDay();
    void RunOneDay();

    void Download(const QString& szFile);

private:
    QString m_szUrl;
    QString m_szVersion;
    QString m_szExt;
    QDateTime m_tmStart;
    bool m_bRunOneDay;
    QTimer m_Timer;
    CThreadPool m_ThreadPool;
};
