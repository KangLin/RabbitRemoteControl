// Author: Kang Lin <kl222@126.com>

#include "Connecter.h"
#include <QClipboard>
#include <QApplication>
#include <QDebug>
#include <QGenericArgument>
#include <QRegularExpression>

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
    LOG_MODEL_DEBUG("CConnecter", "CConnecter::~CConnecter");
}

const QString CConnecter::Id()
{
    QString szId = Protol() + "_" + m_pPluginViewer->Name();
    if(GetParameter())
        szId += "_" + GetParameter()->GetHost()
                + "_" + QString::number(GetParameter()->GetPort());
    szId = szId.replace(QRegularExpression("[@:/#%!^&*\\.]"), "_");
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
    emit sigUpdateName(Name());
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
        // The dialog is closed when the connect is close.
        bool check = connect(this, SIGNAL(sigDisconnected()),
                             p, SLOT(reject()));
        Q_ASSERT(check);
        p->setWindowIcon(this->Icon());
        p->setWindowTitle(tr("Set ") + m_pPluginViewer->DisplayName());
#ifdef BUILD_QUIWidget
        QUIWidget* quiwidget = new QUIWidget();
        quiwidget->setMainWidget(p);
        check = connect(p, SIGNAL(accepted()), quiwidget, SLOT(accept()));
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
    if(GetParameter())
        GetParameter()->disconnect(this);

    m_pParameter = p;

    if(GetParameter())
    {
        bool check = false;
        check = connect(GetParameter(), SIGNAL(sigNameChanged()),
                        this, SLOT(slotUpdateName()));
        Q_ASSERT(check);
        check = connect(GetParameter(), SIGNAL(sigShowServerNameChanged()),
                        this, SLOT(slotShowServerName()));
        Q_ASSERT(check);   
    }
    return 0;
}

void CConnecter::slotShowServerName()
{
    emit sigUpdateName(Name());
}

void CConnecter::slotUpdateName()
{
    emit sigUpdateName(Name());
}

QObject* CConnecter::createObject(const QString& className, QObject* parent)
{
    Q_UNUSED(parent);
    int type = QMetaType::type(className.toStdString().c_str());
    if(QMetaType::UnknownType == type)
    {
        qCritical() << className << " is QMetaType::UnknownType";
        return nullptr;
    }
    QObject *obj = (QObject*)QMetaType::create(type);
    if(nullptr == obj)
    {
        qCritical() << "QMetaType::create fail: " << type;
        return nullptr;
    }
    //const QMetaObject* metaObj = QMetaType::metaObjectForType(type);
    //QObject *obj = metaObj->newInstance(Q_ARG(QObject*, parent));
    return obj;
}

void CConnecter::slotBlockShowWidget(const QString& className, int &nRet, void* pContext)
{
    bool check = false;
    QObject *obj = createObject(className);
    Q_ASSERT(obj);
    if(!obj) return;
    
    obj->metaObject()->invokeMethod(obj, "SetContext", Q_ARG(void*, pContext));
    obj->metaObject()->invokeMethod(obj, "SetConnecter", Q_ARG(CConnecter*, this));
    if(obj->inherits("QDialog"))
    {
        QDialog* pDlg = qobject_cast<QDialog*>(obj);
        pDlg->setAttribute(Qt::WA_DeleteOnClose);
        check = connect(this, SIGNAL(sigDisconnected()),
                        pDlg, SLOT(reject()));
        Q_ASSERT(check);
        nRet = pDlg->exec();
    } else if(obj->inherits("QWidget"))
    {
        QWidget* pWdg = qobject_cast<QWidget*>(obj);
        pWdg->setAttribute(Qt::WA_DeleteOnClose);
        check = connect(this, SIGNAL(sigDisconnected()),
                        pWdg, SLOT(close()));
        Q_ASSERT(check);
        pWdg->show();
    }
}

void CConnecter::slotBlockShowMessage(QString title, QString message,
                                      QMessageBox::StandardButtons buttons,
                                      QMessageBox::StandardButton &nRet)
{
    nRet = QMessageBox::information(GetViewer(), title, message, buttons);
}
