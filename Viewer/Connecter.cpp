// Author: Kang Lin <kl222@126.com>

#include "Connecter.h"
#include <QClipboard>
#include <QApplication>
#include "PluginViewer.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonLog.h"
#ifdef BUILD_QUIWidget
    #include "QUIWidget/QUIWidget.h"
#endif

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

int CConnecter::OpenDialogSettings(QWidget *parent)
{
    int nRet = -1;
    QDialog* p = GetDialogSettings(parent);
    if(p)
    {
        p->setWindowIcon(this->Icon());
#ifdef BUILD_QUIWidget
        QUIWidget* quiwidget = new QUIWidget();
        quiwidget->setMainWidget(p);
        bool check = connect(p, SIGNAL(accepted()), quiwidget, SLOT(accept()));
        Q_ASSERT(check);
        check = connect(p, SIGNAL(rejected()), quiwidget, SLOT(reject()));
        Q_ASSERT(check);
        p = quiwidget;
#endif
        
        p->setAttribute(Qt::WA_DeleteOnClose);
        nRet = p->exec();
    } else {
        LOG_MODEL_ERROR("CConnecter",  "The protol[%s] don't settings dialog", Protol().toStdString().c_str());
    }
    return nRet;
}
