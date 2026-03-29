// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>

#include "BackendFtpServer.h"
#include "DlgSettings.h"
#include "Plugin.h"
#include "OperateFtpServer.h"

static Q_LOGGING_CATEGORY(log, "FtpServer.Operate")
COperateFtpServer::COperateFtpServer(CPlugin *plugin)
    : COperate(plugin)
    , m_pThread(nullptr)
    , m_pView(nullptr)
    , m_pStart(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
}

COperateFtpServer::~COperateFtpServer()
{
    qDebug(log) << Q_FUNC_INFO;
}

const qint16 COperateFtpServer::Version() const
{
    return 0;
}

QWidget *COperateFtpServer::GetViewer()
{
    return m_pView;
}

int COperateFtpServer::Start()
{
    qDebug(log) << Q_FUNC_INFO;
    emit sigRunning();
    return 0;
}

int COperateFtpServer::Stop()
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_pStart && m_pStart->isChecked())
        m_pStart->setChecked(false);
    emit sigFinished();
    return 0;
}

CBackend* COperateFtpServer::InstanceBackend()
{
    auto p = new CBackendFtpServer(this);
    bool check = connect(p, SIGNAL(sigConnectCount(int,int,int)),
                         m_pView, SLOT(slotConnectCount(int,int,int)));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigConnected(QString,quint16)),
                    m_pView, SLOT(slotConnected(QString,quint16)));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigDisconnected(QString,quint16)),
                    m_pView, SLOT(slotDisconnected(QString,quint16)));
    Q_ASSERT(check);
    check = connect(m_pView, SIGNAL(sigDisconnect(QString,quint16)),
                    p, SLOT(slotDisconnect(QString,quint16)));
    Q_ASSERT(check);
    return p;
}

int COperateFtpServer::SetPluginParameters(CParameterPlugin *pPara)
{
    if(GetParameter()) {
        GetParameter()->SetPluginParameters(pPara);
        GetParameter()->m_Net.m_User.SetSavePassword(true);
        if(pPara) {
            bool check = connect(pPara, SIGNAL(sigNameStylesChanged()),
                                 this, SLOT(slotUpdateName()));
            Q_ASSERT(check);
        }
        return 0;
    } else {
        QString szMsg = "There is not parameters! "
                        "please first create parameters, "
                        "then call SetParameter() in the ";
        szMsg += metaObject()->className() + QString("::")
                 + metaObject()->className();
        szMsg += QString("() or ") + metaObject()->className()
                 + QString("::") + "Initial()";
        szMsg += " to set the parameters pointer. "
                 "Default set CParameterClient for the parameters of operate "
                 "(CParameterOperate or its derived classes) "
                 "See CManager::CreateOperate. "
                 "If you are sure the parameter of operate "
                 "does not need CParameterClient. "
                 "Please overload the SetPluginParameters() in the ";
        szMsg += QString(metaObject()->className()) + " . don't set it";
        qCritical(log) << szMsg.toStdString().c_str();
        Q_ASSERT(false);
    }
    return -1;
}

QDialog *COperateFtpServer::OnOpenDialogSettings(QWidget *parent)
{
    auto pDlg = new CDlgSettings(m_Para, parent);
    return pDlg;
}

int COperateFtpServer::Load(QSettings &set)
{
    if(m_Para)
        return m_Para->Load(set);
    return 0;
}

int COperateFtpServer::Save(QSettings &set)
{
    if(m_Para)
        return m_Para->Save(set);
    return 0;
}

int COperateFtpServer::Initial()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    bool check = false;

    m_Para = QSharedPointer<CParameterFtpServer>(new CParameterFtpServer());
    if(!m_Para)
        return -1;

    check = connect(m_Para.get(), &CParameter::sigChanged,
                    this, [&](){
                        emit this->sigUpdateParameters(this);
                    });
    Q_ASSERT(check);

    nRet = COperate::Initial();
    if(nRet)
        return nRet;

    m_pStart = m_Menu.addAction(QIcon::fromTheme("media-playback-start"), tr("Start server"));
    check = connect(m_pStart, &QAction::toggled, this, &COperateFtpServer::slotStart);
    Q_ASSERT(check);
    m_pStart->setCheckable(true);
    m_pStart->setToolTip(m_pStart->text());
    if(m_pActionSettings) {
        m_Menu.addSeparator();
        m_Menu.addAction(m_pActionSettings);
    }

    m_pView = new CFrmMain(this);
    return nRet;
}

int COperateFtpServer::Clean()
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_pView)
        delete m_pView;
    return 0;
}

QSharedPointer<CParameterFtpServer> COperateFtpServer::GetParameter() const
{
    return m_Para;
}

void COperateFtpServer::slotStart(bool checked)
{
    qDebug(log) << Q_FUNC_INFO << m_pStart->isChecked();
    bool check = false;
    if(!checked) {
        m_pStart->setText(tr("Start server"));
        m_pStart->setToolTip(m_pStart->text());
        m_pStart->setIcon(QIcon::fromTheme("media-playback-start"));
        if(m_pThread)
        {
            slotSetSecurityLevel(CSecurityLevel::Level::No);
            m_pThread->quit();
            //Don't delete m_pThread, See CBackendThread
            m_pThread = nullptr;
        }
        return;
    }

    m_pStart->setText(tr("Stop server"));
    m_pStart->setToolTip(m_pStart->text());
    m_pStart->setIcon(QIcon::fromTheme("media-playback-stop"));
    Q_ASSERT(!m_pThread);
    m_pThread = new CBackendThread(this, false);
    if(!m_pThread) {
        qCritical(log) << "new CBackendThread fail";
        return;
    }
    check = connect(this, &COperateFtpServer::sigError,
                    this, [this](const int nError, const QString &szError){
                        m_pStart->setChecked(false);
                    });
    Q_ASSERT(check);
    m_pThread->start();
}

const QString COperateFtpServer::Name()
{
    QString szName;
    // Show the name of parameter
    if(GetParameter() && !(GetParameter()->GetName().isEmpty()))
        szName += GetParameter()->GetName();
    else {
        // Show the name
        szName += COperate::Name();
    }

    // Show the prefix of security level
    QString szSecurityLevel;
    CSecurityLevel sl(GetSecurityLevel());
    if((GetParameter()->GetPluginParameters()->GetNameStyles()
         & CParameterPlugin::NameStyle::SecurityLevel)
        && !(GetSecurityLevel() & CSecurityLevel::Level::No)
        && !sl.GetUnicodeIcon().isEmpty())
        szSecurityLevel = sl.GetUnicodeIcon().left(2);

    return szSecurityLevel + szName;
}

const QString COperateFtpServer::Description()
{
    QString szDescription;
    if(!Name().isEmpty())
        szDescription = tr("Name: ") + Name() + "\n";

    if(!GetTypeName().isEmpty())
        szDescription += tr("Type: ") + GetTypeName() + "\n";

    if(!Protocol().isEmpty()) {
        szDescription += tr("Protocol: ") + Protocol();
#ifdef DEBUG
        if(!GetPlugin()->DisplayName().isEmpty())
            szDescription += " - " + GetPlugin()->DisplayName();
#endif
        szDescription += "\n";
    }

    CSecurityLevel sl(GetSecurityLevel());
    if(!(GetSecurityLevel() & CSecurityLevel::Level::No)) {
        szDescription += tr("Security level: ");
        if(!sl.GetUnicodeIcon().isEmpty())
            szDescription += sl.GetUnicodeIcon() + " ";
        szDescription += sl.GetString() + "\n";
    }

    if(!GetPlugin()->Description().isEmpty())
        szDescription += tr("Description: ") + GetPlugin()->Description();

    return szDescription;
}
