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
#include "Parameter.h"

CConnecter::CConnecter(CPluginViewer *parent) : QObject(parent),
    m_pPluginViewer(parent),
    m_pParameter(nullptr)
{
}

CConnecter::~CConnecter()
{
}

const QString CConnecter::Id()
{
    QString szId = Protol() + "_" + m_pPluginViewer->Name();
    if(GetParameter())
        szId += "_" + GetParameter()->GetHost()
                + "_" + QString::number(GetParameter()->GetPort());
    szId = szId.replace(QRegExp("[@:/#%!^&*]"), "_");
    return szId;
}

const QString CConnecter::Name()
{
    if(GetParameter() && !GetParameter()->GetName().isEmpty())
        return GetParameter()->GetName();
    return ServerName();
}

const QString CConnecter::Description()
{
    return tr("Name:") + Name() + " "
            + tr("Protol:") + Protol() + " "
            + tr("Server name:") + ServerName() + " "
            + m_pPluginViewer->Description();
}

const QString CConnecter::Protol() const
{
    return m_pPluginViewer->Protol();
}

const QIcon CConnecter::Icon() const
{
    return m_pPluginViewer->Icon();
}

void CConnecter::slotSetClipboard(QMimeData* data)
{
    QClipboard* pClipboard = QApplication::clipboard();
    pClipboard->setMimeData(data);
}

void CConnecter::slotSetServerName(const QString& szName)
{
    m_szServerName = szName;
    emit sigUpdateServerName(ServerName());
}

QString CConnecter::ServerName()
{
    return m_szServerName;
}

int CConnecter::OpenDialogSettings(QWidget *parent)
{
    int nRet = -1;
    QDialog* p = GetDialogSettings(parent);
    if(p)
    {
        p->setWindowIcon(this->Icon());
        p->setWindowTitle(tr("Set ") + m_pPluginViewer->DisplayName());
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

int CConnecter::Load(QString szFile)
{
    Q_ASSERT(!szFile.isEmpty());
    if(szFile.isEmpty())
    {
        LOG_MODEL_ERROR("CConnecter", "The load file is empty");
        return -1;
    }
    QSettings set(szFile, QSettings::IniFormat);
    return Load(set);
}

int CConnecter::Save(QString szFile)
{
    Q_ASSERT(!szFile.isEmpty());
    if(szFile.isEmpty())
    {
        LOG_MODEL_ERROR("CConnecter", "The load file is empty");
        return -1;
    }    
    QSettings set(szFile, QSettings::IniFormat);
    return Save(set);
}

CParameter* CConnecter::GetParameter()
{
    return m_pParameter;
}

int CConnecter::SetParameter(CParameter *p)
{
    if(m_pParameter)
        m_pParameter->disconnect(this);
    
    m_pParameter = p;
    
    if(m_pParameter)
    {
        bool check = connect(m_pParameter, SIGNAL(sigShowServerNameChanged()),
                             this, SLOT(slotShowServerName()));
        Q_ASSERT(check);
    }
    return 0;
}

void CConnecter::slotShowServerName()
{
    emit sigUpdateServerName(ServerName());
}
