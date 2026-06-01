// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "Plugin.h"
#include "BackendSftpServer.h"
#include "ParameterSftpServer.h"
#include "DlgSettingsSftpServer.h"
#include "OperateSftpServer.h"

static Q_LOGGING_CATEGORY(log, "Operate.SftpServer")
COperateSftpServer::COperateSftpServer(CPlugin *plugin)
    : COperateServer(plugin)
    , m_pPara(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
}

COperateSftpServer::~COperateSftpServer()
{
    qDebug(log) << Q_FUNC_INFO;
    QString szClass = this->metaObject()->className();
    QString szWhat;
    szWhat += "Please call ";
    szWhat += szClass;
    szWhat += "::Stop() first";
    Q_ASSERT_X(!m_pPara, szClass.toStdString().c_str(), szWhat.toStdString().c_str());

}

CParameterSftpServer* COperateSftpServer::GetParameter() const
{
    return m_pPara;
}

const QString COperateSftpServer::Name()
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
    if(GetParameter() && GetParameter()->GetPluginParameters()
        && (GetParameter()->GetPluginParameters()->GetNameStyles()
            & CParameterPlugin::NameStyle::SecurityLevel)
        && !(GetSecurityLevel() & CSecurityLevel::Level::No)
        && !sl.GetUnicodeIcon().isEmpty())
        szSecurityLevel = sl.GetUnicodeIcon().left(2);

    return szSecurityLevel + szName;
}

const QString COperateSftpServer::Description()
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

const qint16 COperateSftpServer::Version() const
{
    return 0;
}

CBackend* COperateSftpServer::InstanceBackend()
{
    return new CBackendSftpServer(this);
}

int COperateSftpServer::SetPluginParameters(CParameterPlugin *pPara)
{
    int nRet = 0;
    if(m_pPara) {
        m_pPara->SetPluginParameters(pPara);
        m_pPara->m_Net.m_User.SetSavePassword(true);
        m_pPara->m_Net.m_User.SetSavePassphrase(true);
    }
    return nRet;
}

QDialog *COperateSftpServer::OnOpenDialogSettings(QWidget *parent)
{
    return new CDlgSettingsSftpServer(m_pPara, parent);
}

int COperateSftpServer::Load(QSettings &set)
{
    int nRet = 0;
    if(m_pPara)
        nRet = m_pPara->Load(set);
    return nRet;
}

int COperateSftpServer::Save(QSettings &set)
{
    int nRet = 0;
    if(m_pPara)
        nRet = m_pPara->Save(set);
    return nRet;
}

int COperateSftpServer::Initial()
{
    int nRet = 0;
    nRet = COperateServer::Initial();
    if(nRet) return nRet;

    m_pPara = new CParameterSftpServer();

    return nRet;
}

int COperateSftpServer::Clean()
{
    int nRet = 0;
    if(m_pPara) {
        delete m_pPara;
        m_pPara = nullptr;
    }
    nRet = COperateServer::Clean();
    return nRet;
}
