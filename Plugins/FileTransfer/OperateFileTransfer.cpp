// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QRegularExpression>

#include "BackendFieTransfer.h"
#include "OperateFileTransfer.h"
#include "Plugin.h"
#include "DlgFileTransfer.h"

static Q_LOGGING_CATEGORY(log, "Plugin.FileTransfer.Operate")

COperateFileTransfer::COperateFileTransfer(CPlugin *plugin) : COperate(plugin)
    , m_frmFileTransfer(nullptr)
    , m_pThread(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
    bool check = connect(&m_Parameter, &CParameter::sigChanged,
                         this, [&](){
                             emit this->sigUpdateParameters(this);
                         });
    Q_ASSERT(check);
}

COperateFileTransfer::~COperateFileTransfer()
{
    qDebug(log) << Q_FUNC_INFO;
}

CBackend* COperateFileTransfer::InstanceBackend()
{
    return new CBackendFieTransfer(this);
}

CParameterFileTransfer* COperateFileTransfer::GetParameter()
{
    return &m_Parameter;
}

const qint16 COperateFileTransfer::Version() const
{
    return GetPlugin()->Version().toInt();
}

QWidget *COperateFileTransfer::GetViewer()
{
    return m_frmFileTransfer;
}

int COperateFileTransfer::Initial()
{
    int nRet = 0;
    nRet = COperate::Initial();
    if(nRet)
        return nRet;
    
    if(m_pActionSettings) {
        m_Menu.addSeparator();
        m_Menu.addAction(m_pActionSettings);
    }

    m_frmFileTransfer = new CFrmFileTransfer();

    return nRet;
}

int COperateFileTransfer::Clean()
{
    int nRet = 0;
    if(m_frmFileTransfer) {
        m_frmFileTransfer->deleteLater();
        m_frmFileTransfer = nullptr;
    }
    nRet = COperate::Clean();
    return nRet;
}

int COperateFileTransfer::Start()
{
    int nRet = 0;
    m_pThread = new CBackendThread(this);
    if(!m_pThread) {
        qCritical(log) << "new CBackendThread fail";
        return -2;
    }
    m_pThread->start();
    return nRet;
}

int COperateFileTransfer::Stop()
{
    int nRet = 0;
    if(m_pThread)
    {
        m_pThread->quit();
        //Don't delete m_pThread, See CBackendThread
        m_pThread = nullptr;
    }
    return nRet;
}

int COperateFileTransfer::SetGlobalParameters(CParameterPlugin *pPara)
{
    m_Parameter.SetGlobalParameters(pPara);
    return COperate::SetGlobalParameters(pPara);
}

QDialog *COperateFileTransfer::OnOpenDialogSettings(QWidget *parent)
{
    return new CDlgFileTransfer(&m_Parameter, parent);
}

int COperateFileTransfer::Load(QSettings &set)
{
    return m_Parameter.Load(set);
}

int COperateFileTransfer::Save(QSettings &set)
{
    return m_Parameter.Save(set);
}

const QString COperateFileTransfer::Name()
{
    QString szName;
    CParameterNet* pNet = nullptr;
    switch((CParameterFileTransfer::Protocol)m_Parameter.GetProtocol()) {
    case CParameterFileTransfer::Protocol::SFTP:
        pNet = &m_Parameter.m_SSH.m_Net;
        break;
    case CParameterFileTransfer::Protocol::FTP:
        pNet = &m_Parameter.m_Net;
        break;
    }
    if(!pNet->GetHost().isEmpty()) {
        if(m_Parameter.GetGlobalParameters()
            && m_Parameter.GetGlobalParameters()->GetShowProtocolPrefix())
            szName = Protocol() + ":";
        szName += pNet->GetHost()
                  + ":" + QString::number(pNet->GetPort());
    }
    if(szName.isEmpty())
        szName = COperate::Name();
    return szName;
}

const QString COperateFileTransfer::Description()
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

    auto &net = m_Parameter.m_Net;
    if(!net.GetHost().isEmpty())
        szDescription += tr("Server address: ") + net.GetHost()
                         + ":" + QString::number(net.GetPort()) + "\n";

    if(GetSecurityLevel() != SecurityLevel::No)
        szDescription += tr("Security level: ") + GetSecurityLevelString() + "\n";

    if(!GetPlugin()->Description().isEmpty())
        szDescription += tr("Description: ") + GetPlugin()->Description();

    return szDescription;
}

const QString COperateFileTransfer::Protocol() const
{
    switch(m_Parameter.GetProtocol()) {
    case CParameterFileTransfer::Protocol::FTP:
        return "FTP";
    case CParameterFileTransfer::Protocol::SFTP:
        return "SFTP";
    }
    return QString();
}

const QString COperateFileTransfer::Id()
{
    QString szId = COperate::Id();
    auto &net = m_Parameter.m_Net;
    if(!net.GetHost().isEmpty())
        szId += "_" + net.GetHost()
                + "_" + QString::number(net.GetPort());
    
    static QRegularExpression exp("[-@:/#%!^&* \\.]");
    szId = szId.replace(exp, "_");
    return szId;
}
