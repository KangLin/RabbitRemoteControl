#include "OperateWebBrowser.h"
#include <QLoggingCategory>

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

const qint16 COperateWebBrowser::Version() const
{
    return 0;
}

QWidget *COperateWebBrowser::GetViewer()
{
    return m_pWeb;
}

int COperateWebBrowser::Start()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;

    emit sigRunning();
    return nRet;
}

int COperateWebBrowser::Stop()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;

    emit sigFinished();
    return nRet;
}

int COperateWebBrowser::SetGlobalParameters(CParameterPlugin *pPara)
{
    int nRet = 0;
    return nRet;
}

QDialog *COperateWebBrowser::OnOpenDialogSettings(QWidget *parent)
{
    qDebug(log) << Q_FUNC_INFO;
    return nullptr;
}

int COperateWebBrowser::Initial()
{
    qDebug(log) << Q_FUNC_INFO;
    m_pWeb = new CFrmWebBrowser();
    int nRet = COperate::Initial();
    if(nRet) return nRet;
    if(m_pWeb)
        m_pWeb->InitMenu(&m_Menu);
    return nRet;
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
