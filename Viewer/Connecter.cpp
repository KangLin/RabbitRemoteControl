// Author: Kang Lin <kl222@126.com>

#include "Connecter.h"
#include <QClipboard>
#include <QApplication>
#include "PluginViewer.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonLog.h"

CConnecter::CConnecter(CPluginViewer *parent) : QObject(parent),
    m_pPluginFactory(parent)
{
}

CConnecter::~CConnecter()
{
}

const QString CConnecter::Id()
{
    return Protol() + "_" + m_pPluginFactory->Name()
            + "_" + ServerName().replace(":", "_");
}

const QString CConnecter::Name()
{
    return m_pPluginFactory->Name();
}

const QString CConnecter::Description()
{
    return tr("Name:") + Name() + " "
            + tr("Protol:") + Protol() + " "
            + tr("Server name:") + ServerName() + " "
            + m_pPluginFactory->Description();
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
    emit sigUpdateName(m_szServerName);
}

QString CConnecter::ServerName()
{
    return m_szServerName;
}

const CPluginViewer* CConnecter::GetPluginFactory() const
{
    return m_pPluginFactory;
}
