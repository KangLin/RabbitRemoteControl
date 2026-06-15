// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>

#include "BackendFreeRDPServer.h"
#include "ParameterFreeRDPServer.h"
#include "DlgSettingsFreeRDPServer.h"
#include "OperateFreeRDPServer.h"

static Q_LOGGING_CATEGORY(log, "FreeRDPServer.Operate")
COperateFreeRDPServer::COperateFreeRDPServer(CPlugin *plugin)
    : COperateServer(plugin)
    , m_pPara(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
}

COperateFreeRDPServer::~COperateFreeRDPServer()
{
    qDebug(log) << Q_FUNC_INFO;
    QString szClass = this->metaObject()->className();
    QString szWhat;
    szWhat += "Please call ";
    szWhat += szClass;
    szWhat += "::Stop() first";
    Q_ASSERT_X(!m_pPara, szClass.toStdString().c_str(), szWhat.toStdString().c_str());
}

CParameterFreeRDPServer* COperateFreeRDPServer::GetParameter() const
{
    return m_pPara;
}

const qint16 COperateFreeRDPServer::Version() const
{
    return 0;
}

CBackend* COperateFreeRDPServer::InstanceBackend()
{
    return new CBackendFreeRDPServer(this);
}

int COperateFreeRDPServer::SetPluginParameters(CParameterPlugin *pPara)
{
    int nRet = 0;
    if(m_pPara) {
        m_pPara->SetPluginParameters(pPara);
        m_pPara->m_Net.m_User.SetSavePassword(true);
        m_pPara->m_Net.m_User.SetSavePassphrase(true);
    }
    return nRet;
}

QDialog *COperateFreeRDPServer::OnOpenDialogSettings(QWidget *parent)
{
    return new CDlgSettingsFreeRDPServer(m_pPara, parent);
}

int COperateFreeRDPServer::Load(QSettings &set)
{
    int nRet = 0;

    if(m_pPara)
        nRet = m_pPara->Load(set);

    return nRet;
}

int COperateFreeRDPServer::Save(QSettings &set)
{
    int nRet = 0;
    if(m_pPara)
        nRet = m_pPara->Save(set);
    return nRet;
}

int COperateFreeRDPServer::Initial()
{
    int nRet = 0;
    nRet = COperateServer::Initial();
    if(nRet) return nRet;
    m_pPara = new CParameterFreeRDPServer();
    return nRet;
}

int COperateFreeRDPServer::Clean()
{
    int nRet = 0;
    if(m_pPara) {
        delete m_pPara;
        m_pPara = nullptr;
    }
    nRet = COperateServer::Clean();
    return nRet;
}
