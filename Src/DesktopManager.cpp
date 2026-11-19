// Author: Kang Lin <kl222@126.com>

#include <QProcess>
#include <QLoggingCategory>
#include <QDesktopServices>
#include <QUrl>
#include "DesktopManager.h"
#include "ParameterPlugin.h"

static Q_LOGGING_CATEGORY(log, "Plugin.Hook.DesktopManager")
CDesktopManager::CDesktopManager(
    CParameterPlugin *para, QObject *parent)
    : QObject{parent}
    , m_pParameterPlugin(para)
    , m_bScript(false)
{
    qDebug(log) << Q_FUNC_INFO;
}

CDesktopManager::~CDesktopManager()
{
    qDebug(log) << Q_FUNC_INFO;
}

void CDesktopManager::slotDisableShortcuts()
{
    //qDebug(log) << Q_FUNC_INFO;
    if(m_pParameterPlugin->GetDesktopShortcutsScript()
        && !m_pParameterPlugin->GetRestoreDesktopShortcutsScript().isEmpty()
        && !m_pParameterPlugin->GetDisableDesktopShortcutsScript().isEmpty()) {
        qInfo(log) << "Exec disable shortcuts shortcuts script:" << m_pParameterPlugin->GetDisableDesktopShortcutsScript();
        bool bRet = false;
#if defined(Q_OS_WIN32)
        bRet = QDesktopServices::openUrl(m_pParameterPlugin->GetDisableDesktopShortcutsScript());
#else
        QStringList lstPara;
        if(!m_pParameterPlugin->GetDisableDesktopShortcutsScriptParameters().isEmpty())
            lstPara = m_pParameterPlugin->GetDisableDesktopShortcutsScriptParameters().split(" ");
        //bRet = QProcess::startDetached(m_pParameterPlugin->GetDisableDesktopShortcutsScript(), lstPara);
        bRet = RunCommand(m_pParameterPlugin->GetDisableDesktopShortcutsScript(), lstPara);
#endif
        if(!bRet)
            qCritical(log) << "Failed run disable shortcuts script file" << m_pParameterPlugin->GetDisableDesktopShortcutsScript();
        m_bScript = true;
    } else {
        OnDisableShortcuts();
        m_bScript = false;
    }
}

void CDesktopManager::slotRestoreShortcuts()
{
    //qDebug(log) << Q_FUNC_INFO;
    if(m_bScript) {
        if(m_pParameterPlugin->GetRestoreDesktopShortcutsScript().isEmpty()) {
            qCritical(log) << "Restore shortcuts is empty";
            return;
        }
        qInfo(log) << "Exec restore shortcuts shortcuts script:" << m_pParameterPlugin->GetRestoreDesktopShortcutsScript();
        bool bRet = false;
#if defined(Q_OS_WIN32)
        bRet = QDesktopServices::openUrl(m_pParameterPlugin->GetRestoreDesktopShortcutsScript());
#else
        QStringList lstPara;
        if(!m_pParameterPlugin->GetRestoreDesktopShortcutsScriptParameters().isEmpty())
            lstPara = m_pParameterPlugin->GetRestoreDesktopShortcutsScriptParameters().split(" ");
        bRet = RunCommand(m_pParameterPlugin->GetRestoreDesktopShortcutsScript(), lstPara);
        //bRet = QProcess::startDetached(m_pParameterPlugin->GetRestoreDesktopShortcutsScript(), lstPara);
#endif
        if(!bRet)
            qCritical(log) << "Failed run restore shortcuts script file" << m_pParameterPlugin->GetRestoreDesktopShortcutsScript();
    } else {
        OnRestoreShortcuts();
    }
}

bool CDesktopManager::RunCommand(const QString &program, const QStringList &args, int timeout)
{
    QProcess process;
    process.setProgram(program);
    if(!args.isEmpty())
        process.setArguments(args);

    //qDebug(log) << "Command:" << program << args;

    process.start();

    if (!process.waitForFinished(timeout)) {
        qWarning(log) << "Command timeout:" << program << args;
        return false;
    }

    if (process.exitCode() != 0) {
        QString errorOutput = process.readAllStandardError();
        qWarning(log) << "Command failed:" << program << args
                      << "exit code:" << process.exitCode()
                      << "failed:" << errorOutput;
        return false;
    }

    QString szOutput = process.readAllStandardOutput();
    if (!szOutput.isEmpty()) {
        qDebug(log) << "Command output:" << szOutput.trimmed();
    }

    return true;
}

QString CDesktopManager::GetCommandOutput(const QString &program, const QStringList &args, int timeout)
{
    QProcess process;
    process.setProgram(program);
    if(!args.isEmpty())
        process.setArguments(args);

    //qDebug(log) << "Command:" << program << args;

    process.start();

    if (!process.waitForFinished(timeout)) {
        qWarning(log) << "Command timeout:" << program << args;
        return QString();
    }

    if (process.exitCode() != 0) {
        QString errorOutput = process.readAllStandardError();
        qWarning(log) << "Command failed:" << program << args
                      << "exit code:" << process.exitCode()
                      << "failed:" << errorOutput;
        return QString();
    }

    return process.readAllStandardOutput();
}
