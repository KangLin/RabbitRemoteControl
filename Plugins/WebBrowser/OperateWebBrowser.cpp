// Author: Kang Lin <kl222@126.com>

#include "OperateWebBrowser.h"
#include <QLoggingCategory>
#include <DlgSettings.h>

static Q_LOGGING_CATEGORY(log, "WebBrowser.Operate")
COperateWebBrowser::COperateWebBrowser(CPlugin *plugin): COperate(plugin)
    , m_pWeb(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
}

COperateWebBrowser::~COperateWebBrowser()
{
    qDebug(log) << Q_FUNC_INFO;
}

const QString COperateWebBrowser::Name()
{
    QString szName;
    szName = m_Parameter.GetName();
    if(szName.isEmpty()) {
        if(m_pWeb)
            szName = m_pWeb->windowTitle();
    }
    if(szName.isEmpty())
        szName = COperate::Name();
    return szName;
}

const QIcon COperateWebBrowser::Icon() const
{
    QIcon icon;
    if(m_pWeb)
        icon = m_pWeb->windowIcon();
    if(icon.isNull())
        icon = COperate::Icon();
    return icon;
}

const qint16 COperateWebBrowser::Version() const
{
    return 0;
}

QWidget *COperateWebBrowser::GetViewer()
{
    return m_pWeb;
}

int COperateWebBrowser::SetGlobalParameters(CParameterPlugin *pPara)
{
    int nRet = 0;
    m_Parameter.SetGlobalParameters(pPara);
    nRet = COperate::SetGlobalParameters(pPara);
    return nRet;
}

QDialog *COperateWebBrowser::OnOpenDialogSettings(QWidget *parent)
{
    qDebug(log) << Q_FUNC_INFO;
    return new CDlgSettings(&m_Parameter, parent);
}

int COperateWebBrowser::Initial()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = COperate::Initial();
    if(nRet) return nRet;
    m_pWeb = new CFrmWebBrowser(&m_Parameter);
    if(m_pWeb) {
        m_pWeb->InitMenu(&m_Menu);
        bool check = connect(m_pWeb, &CFrmWebBrowser::sigInformation,
                             this, &COperateWebBrowser::sigInformation);
        Q_ASSERT(check);
        check = connect(m_pWeb, &CFrmWebBrowser::sigUpdateTitle,
                        this, &COperateWebBrowser::slotUpdateName);
        Q_ASSERT(check);
    }
    m_Menu.addSeparator();
    if(m_pActionSettings)
        m_Menu.addAction(m_pActionSettings);
    return nRet;
}

int COperateWebBrowser::Clean()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    if(m_pWeb) {
        delete m_pWeb;
        m_pWeb = nullptr;
    }
    return nRet;
}

int COperateWebBrowser::Start()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    if(m_pWeb)
        nRet = m_pWeb->Start();
    emit sigRunning();
    return nRet;
}

int COperateWebBrowser::Stop()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    if(m_pWeb)
        nRet = m_pWeb->Stop();
    emit sigFinished();
    return nRet;
}

int COperateWebBrowser::Load(QSettings &set)
{
    int nRet = 0;
    nRet = m_Parameter.Load(set);
    if(m_pWeb)
        nRet = m_pWeb->Load(set);
    return nRet;
}

int COperateWebBrowser::Save(QSettings &set)
{
    int nRet = 0;
    nRet = m_Parameter.Save(set);
    if(m_pWeb)
        nRet = m_pWeb->Save(set);
    return nRet;
}
