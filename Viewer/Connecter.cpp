//! @author: Kang Lin(kl222@126.com)

#include "Connecter.h"
#include <QClipboard>
#include <QApplication>
#include "PluginFactory.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonLog.h"

CConnecter::CConnecter(CPluginFactory *parent) : QObject(parent),
    m_pView(new CFrmViewer()),
    m_pPluginFactory(parent)
{
}

CConnecter::~CConnecter()
{
    if(m_pView)
        delete m_pView;
}

QWidget *CConnecter::GetViewer()
{
    return m_pView;
}

const QString CConnecter::Name() const
{
    return m_pPluginFactory->Name();
}

const QString CConnecter::Description() const
{
    return m_pPluginFactory->Description();
}

const QString CConnecter::Protol() const
{
    return m_pPluginFactory->Protol();
}

const QIcon CConnecter::Icon() const
{
    return m_pPluginFactory->Icon();
}

void CConnecter::slotSetClipboard(QMimeData* data)
{
    QClipboard* pClipboard = QApplication::clipboard();
    pClipboard->setMimeData(data);
}

void CConnecter::slotSetServerName(const QString& szName)
{
    m_szServerName = szName;
    emit sigServerName(m_szServerName);
}

QString CConnecter::GetServerName()
{
    return m_szServerName;
}

const CPluginFactory* CConnecter::GetPluginFactory() const
{
    return m_pPluginFactory;
}

int CConnecter::OpenDialogSettings(QWidget *parent)
{
    int nRet = -1;
    QDialog* pDlg = GetDialogSettings(parent);
    if(pDlg)
    {
        pDlg->setAttribute(Qt::WA_DeleteOnClose);
        nRet = pDlg->exec();
    } else {
        LOG_MODEL_ERROR("CConnecter",  "The protol[%s] don't settings dialog", Protol().toStdString().c_str());
    }
    return nRet;
}
