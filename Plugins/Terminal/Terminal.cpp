// Author: Kang Lin <kl222@126.com>

#include <QDesktopServices>
#include <QLoggingCategory>
#include <QApplication>

#include "Terminal.h"
#include "DlgSettingsTerminal.h"

static Q_LOGGING_CATEGORY(log, "Plugin.Terminal.Operate")

CTerminal::CTerminal(CPlugin *parent)
    : COperateTerminal(parent)
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
        });
        Q_ASSERT(check);
        if(!m_Parameters.GetShell().isEmpty())
            m_pTerminal->setShellProgram(m_Parameters.GetShell());
        if(!m_Parameters.GetShellParameters().isEmpty())
            m_pTerminal->setArgs(QStringList() << m_Parameters.GetShellParameters());
        m_pTerminal->startShellProgram();
    }
    emit sigRunning();
    return 0;
}

int CTerminal::Stop()
{
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
    
    QAction* pAction = m_Menu.addAction(
        QIcon::fromTheme("folder-open"),
        tr("Open working directory with file explorer"),
        QKeySequence(QKeySequence::Open), //Qt::CTRL | Qt::Key_O),
        [&](){
            QDesktopServices::openUrl(
                QUrl::fromLocalFile(m_pTerminal->workingDirectory()));
        });
    m_Menu.insertAction(m_pActionFind, pAction);
    pAction = m_Menu.addAction(
        QIcon::fromTheme("edit-copy"),
        tr("Copy working directory to clipboard"),
        this, [&](){
            if(!m_pTerminal) return;
            QClipboard* pClipboard = QApplication::clipboard();
            if(!pClipboard) return;
            pClipboard->setText(m_pTerminal->workingDirectory());
        });
    m_Menu.insertAction(m_pActionFind, pAction);
    return nRet;
}
