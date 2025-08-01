// Author: Kang Lin <kl222@126.com>

#include <QDesktopServices>
#include <QLoggingCategory>
#include <QApplication>
#include <QThread>

#include "Terminal.h"
#include "DlgSettingsTerminal.h"

static Q_LOGGING_CATEGORY(log, "Plugin.Terminal.Operate")

CTerminal::CTerminal(CPlugin *parent)
    : COperateTerminal(parent)
    , m_pOpenFolderWithExplorer(nullptr)
    , m_pCopyToClipboard(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
    SetParameter(&m_Parameters);
}

CTerminal::~CTerminal()
{
    qDebug(log) << Q_FUNC_INFO;
}

QDialog *CTerminal::OnOpenDialogSettings(QWidget *parent)
{
    return new CDlgSettingsTerminal(&m_Parameters, parent);
}

int CTerminal::Start()
{
    slotUpdateParameter(this);
    if(m_pTerminal) {
        bool check = connect(m_pTerminal, &QTermWidget::titleChanged,
                             this, [&](){
            qDebug(log) << "Title changed:" << m_pTerminal->title()
                        << "; Working directory:"
                        << m_pTerminal->workingDirectory();
            if(m_Parameters.GetEnableTitleChanged()) {
                m_pTerminal->setWindowTitle(m_pTerminal->title());
                emit sigUpdateName(m_pTerminal->title());
            }
            if(m_Parameters.GetRestoreDirectory()) {
                if(!m_pTerminal->workingDirectory().isEmpty())
                    m_Parameters.SetLasterDirectory(m_pTerminal->workingDirectory());
            }
        });
        Q_ASSERT(check);
        if(!m_Parameters.GetShell().isEmpty())
            m_pTerminal->setShellProgram(m_Parameters.GetShell());
        if(!m_Parameters.GetShellParameters().isEmpty())
            m_pTerminal->setArgs(QStringList() << m_Parameters.GetShellParameters());
        qDebug(log) << "Start:" << m_Parameters.GetShell() << m_Parameters.GetShellParameters();
        m_pTerminal->startShellProgram();
        if(m_Parameters.GetRestoreDirectory() && !m_Parameters.GetLasterDirectory().isEmpty())
            m_pTerminal->sendText("cd " + m_Parameters.GetLasterDirectory() + "\r");
        if(!m_Parameters.GetCommands().isEmpty()) {
            foreach (auto c, m_Parameters.GetCommands())
                m_pTerminal->sendText(c + "\r");
        }
    }
    emit sigRunning();
    return 0;
}

int CTerminal::Stop()
{
    // if(m_Parameters.GetRestoreDirectory()) {
    //     if(!m_pTerminal->workingDirectory().isEmpty())
    //         m_Parameters.SetLasterDirectory(m_pTerminal->workingDirectory());
    //     emit sigUpdateParameters(this);
    // }

    if(m_pTerminal)
        m_pTerminal->close();
    emit sigFinished();
    return 0;
}

CBackend *CTerminal::InstanceBackend()
{
    return nullptr;
}

int CTerminal::Initial()
{
    int nRet = 0;
    nRet = COperateTerminal::Initial();
    if(nRet)
        return nRet;
    
    m_pOpenFolderWithExplorer = m_Menu.addAction(
        QIcon::fromTheme("folder-open"),
        tr("Open working directory with file explorer") + "\tCtrl+O",
        QKeySequence(QKeySequence::Open), //Qt::CTRL | Qt::Key_O),
        [&](){
            QDesktopServices::openUrl(
                QUrl::fromLocalFile(m_pTerminal->workingDirectory()));
        });
    m_Menu.insertAction(m_pActionFind, m_pOpenFolderWithExplorer);
    m_pCopyToClipboard = m_Menu.addAction(
        QIcon::fromTheme("edit-copy"),
        tr("Copy working directory to clipboard"),
        this, [&](){
            if(!m_pTerminal) return;
            QClipboard* pClipboard = QApplication::clipboard();
            if(!pClipboard) return;
            pClipboard->setText(m_pTerminal->workingDirectory());
        });
    m_Menu.insertAction(m_pActionFind, m_pCopyToClipboard);
    return nRet;
}

const QString CTerminal::Id()
{
    QString szId;
    szId = COperateTerminal::Id();
    if(GetParameter() && !GetParameter()->GetShellName().isEmpty())
        szId += "_" + GetParameter()->GetShellName();
    static QRegularExpression exp("[-@:/#%!^&* \\.]");
    szId = szId.replace(exp, "_");
    return szId;
}

const QString CTerminal::Name()
{
    if(GetParameter() && !GetParameter()->GetShellName().isEmpty())
        return COperateTerminal::Name() + " - " + GetParameter()->GetShellName();
    return COperateTerminal::Name();
}

void CTerminal::SetShotcuts(bool bEnable)
{
    COperateTerminal::SetShotcuts(bEnable);
    if(bEnable) {
        m_pOpenFolderWithExplorer->setShortcut(QKeySequence(QKeySequence::Open));
        return;
    }
    m_pOpenFolderWithExplorer->setShortcut(QKeySequence());
    m_pCopyToClipboard->setShortcut(QKeySequence());
}
