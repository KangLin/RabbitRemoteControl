// Author: Kang Lin <kl222@126.com>

#include <QProcess>
#include <QLoggingCategory>
#include <QDesktopServices>
#include <QUrl>
#include "DesktopManager.h"
#include "ParameterPlugin.h"

static Q_LOGGING_CATEGORY(log, "Plugin.Hook.Desktop.Shortcut.Manager")
CDesktopManager::CDesktopManager(
    CParameterPlugin *para, QObject *parent)
    : QThread{parent}
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
    if(m_pParameterPlugin->GetDesktopShortcutsScript()
        && !m_pParameterPlugin->GetRestoreDesktopShortcutsScript().isEmpty()
        && !m_pParameterPlugin->GetDisableDesktopShortcutsScript().isEmpty()) {
        qDebug(log) << "Exec disable shortcuts script:" << m_pParameterPlugin->GetDisableDesktopShortcutsScript();
#if defined(Q_OS_WIN32)
        QDesktopServices::openUrl(m_pParameterPlugin->GetDisableDesktopShortcutsScript());
#else
        RunCommand(m_pParameterPlugin->GetDisableDesktopShortcutsScript());
#endif
        m_bScript = true;
    } else {
        OnDisableShortcuts();
        m_bScript = false;
    }
}

void CDesktopManager::slotRestoreShortcuts()
{
    if(m_bScript) {
        if(m_pParameterPlugin->GetRestoreDesktopShortcutsScript().isEmpty()) {
            qCritical(log) << "Restore shortcuts is empty";
            return;
        }
        qDebug(log) << "Exec restore shortcuts script:" << m_pParameterPlugin->GetRestoreDesktopShortcutsScript();
#if defined(Q_OS_WIN32)
        QDesktopServices::openUrl(m_pParameterPlugin->GetRestoreDesktopShortcutsScript());
#else
        RunCommand(m_pParameterPlugin->GetRestoreDesktopShortcutsScript());
#endif
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
