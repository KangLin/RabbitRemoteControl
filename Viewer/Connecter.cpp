//! @author: Kang Lin(kl222@126.com)

#include "Connecter.h"
#include <QClipboard>
#include <QApplication>
#include "PluginFactory.h"

CConnecter::CConnecter(CPluginFactory *parent) : QObject(parent),
    m_pView(new CFrmViewer()),
    m_pPluginFactory(parent)
{
    m_pView->setAttribute(Qt::WA_DeleteOnClose);
}

CFrmViewer* CConnecter::GetViewer()
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
