// Author: Kang Lin <kl222@126.com>

#include "Connecter.h"
#include "PluginClient.h"
#include "RabbitCommonDir.h"
#ifdef BUILD_QUIWidget
    #include "QUIWidget/QUIWidget.h"
#endif
#include "ParameterNet.h"

#include <QClipboard>
#include <QApplication>
#include <QDebug>
#include <QGenericArgument>
#include <QRegularExpression>
#include <QCheckBox>
#include <QLoggingCategory>
#include <QInputDialog>
#include <QMetaMethod>

static Q_LOGGING_CATEGORY(log, "Client.Connecter")
    
CConnecter::CConnecter(CPluginClient *parent) : QObject(parent),
    m_pPluginClient(parent),
    m_pParameter(nullptr)
{
    bool check = false;
    
    if(QApplication::clipboard())
    {
        check = connect(QApplication::clipboard(), SIGNAL(dataChanged()),
                        this, SIGNAL(sigClipBoardChanged()));
        Q_ASSERT(check);
    }
}

CConnecter::~CConnecter()
{
    qDebug(log) << "CConnecter::~CConnecter";
}

const QString CConnecter::Id()
{
    QString szId = Protocol() + "_" + GetPlugClient()->Name();
    if(GetParameter())
    {
        if(!GetParameter()->GetName().isEmpty())
            szId += "_" + GetParameter()->GetName();
        if(!GetParameter()->m_Net.GetHost().isEmpty())
            szId += "_" + GetParameter()->m_Net.GetHost()
                    + "_" + QString::number(GetParameter()->m_Net.GetPort());
    }
    szId = szId.replace(QRegularExpression("[-@:/#%!^&*\\.]"), "_");
    return szId;
}

/*!
 * \~chinese
 * \brief 显示顺序：
 *        - 用户参数设置的名称
 *        - 如果允许，远程服务名。
 *        - 远程地址
 *
 * \~english
 *  Display order:
 *  - User parameter Name()
 *  - if enable, Server name
 *  - Host and port
 *  
 * \~
 * \see ServerName()
 */
const QString CConnecter::Name()
{
    QString szName;
    if(GetParameter() && GetParameter()->GetParameterClient()
        && GetParameter()->GetParameterClient()->GetShowProtocolPrefix())
        szName = Protocol() + ":";
    
    if(GetParameter() && !(GetParameter()->GetName().isEmpty()))
        szName += GetParameter()->GetName();
    else
        szName += ServerName();
    return szName;
}

const QString CConnecter::Description()
{
    return tr("Name: ") + Name() + "\n"
           + tr("Protocol: ") + Protocol()
#ifdef DEBUG
           + " - " + GetPlugClient()->DisplayName()
#endif
           + "\n"
           + tr("Server name: ") + ServerName() + "\n"
           + tr("Description: ") + GetPlugClient()->Description();
}

const QString CConnecter::Protocol() const
{
    return GetPlugClient()->Protocol();
}

const QIcon CConnecter::Icon() const
{
    return GetPlugClient()->Icon();
}

void CConnecter::slotSetClipboard(QMimeData* data)
{    
    QClipboard* pClipboard = QApplication::clipboard();
    if(pClipboard) {
        //        pClipboard->disconnect(this);
        
        pClipboard->setMimeData(data);
        
        //        bool check = connect(pClipboard, SIGNAL(dataChanged()),
        //                             this, SIGNAL(sigClipBoardChanged()));
        //        Q_ASSERT(check);
    }
}

void CConnecter::slotSetServerName(const QString& szName)
{
    if(m_szServerName == szName)
        return;
    
    m_szServerName = szName;
    if(GetParameter())
    {
        if(GetParameter()->GetServerName() == szName)
            return;
        GetParameter()->SetServerName(szName);
    }
    
    emit sigUpdateName(Name());
}

QString CConnecter::ServerName()
{
    if(GetParameter() && GetParameter()->GetParameterClient()
        && GetParameter()->GetParameterClient()->GetShowIpPortInName())
    {
        return GetParameter()->m_Net.GetHost()
               + ":" + QString::number(GetParameter()->m_Net.GetPort());
    }
    
    if(m_szServerName.isEmpty() && GetParameter())
        return GetParameter()->GetServerName();
    return m_szServerName;
}

int CConnecter::OpenDialogSettings(QWidget *parent)
{
    int nRet = -1;
    QDialog* p = OnOpenDialogSettings(parent);
    if(p)
    {
        // The dialog is closed when the connect is close.
        bool check = connect(this, SIGNAL(sigDisconnected()),
                             p, SLOT(reject()));
        Q_ASSERT(check);
        p->setWindowIcon(this->Icon());
        p->setWindowTitle(tr("Set ") + GetPlugClient()->DisplayName());
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
        qCritical(log) << "The Protocol [" << Protocol() << "] don't settings dialog";
    }
    return nRet;
}

int CConnecter::Load(QString szFile)
{
    Q_ASSERT(!szFile.isEmpty());
    if(szFile.isEmpty())
    {
        qCritical(log) << "The load file is empty";
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
        qCritical(log) << "The load file is empty";
        return -1;
    }    
    QSettings set(szFile, QSettings::IniFormat);
    return Save(set);
}

int CConnecter::SetParameterClient(CParameterClient* pPara)
{
    if(GetParameter())
    {
        GetParameter()->SetParameterClient(pPara);
        if(pPara)
        {
            bool check = connect(pPara, SIGNAL(sigShowProtocolPrefixChanged()),
                                 this, SLOT(slotUpdateName()));
            Q_ASSERT(check);
            check = connect(pPara, SIGNAL(sigSHowIpPortInNameChanged()),
                            this, SLOT(slotUpdateName()));
            Q_ASSERT(check);
        }
        return 0;
    } else {
        qCritical(log) << "The CConnecter is not parameters! please create parameters."
                          "and call SetParameter in the "
                       << metaObject()->className() << "::" << metaObject()->className()
                       << "to set the parameters pointer. "
                          "Default set CParameterClient for the parameters of connecter (CParameterConnecter or its derived classes) "
                          "See: CClient::CreateConnecter."
                          "If you are sure the parameter of connecter does not need CParameterClient. "
                          "please overload the SetParameterClient in the"
                       << metaObject()->className() << ". don't set it";
        Q_ASSERT(false);
    }
    return -1;
}

CParameterBase* CConnecter::GetParameter()
{
    if(!m_pParameter)
    {
        QString szMsg;
        szMsg = "Need use SetParameter() to set parameters in ";
        szMsg += metaObject()->className();
        szMsg += "::";
        szMsg += metaObject()->className();
        Q_ASSERT_X(m_pParameter, "CConnecter",  + szMsg.toStdString().c_str());
    }
    return m_pParameter;
}

int CConnecter::SetParameter(CParameterBase *p)
{
    if(m_pParameter)
        m_pParameter->disconnect(this);
    
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
        check = connect(GetParameter(), &CParameter::sigChanged,
                        this, [&](){
            emit this->sigUpdateParameters(this);
        });
        Q_ASSERT(check);
    }
    return 0;
}

CPluginClient* CConnecter::GetPlugClient() const
{
    return m_pPluginClient;
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
    int type =
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QMetaType::fromName(className.toStdString().c_str()).id();
#else
        QMetaType::type(className.toStdString().c_str());
#endif
    if(QMetaType::UnknownType == type)
    {
        qCritical(log) << className << " is QMetaType::UnknownType";
        return nullptr;
    }
    QObject *obj =
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        (QObject*)QMetaType(type).create();
#else
        (QObject*)QMetaType::create(type);
#endif
    if(nullptr == obj)
    {
        qCritical(log) << "QMetaType::create fail: " << type;
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
    Q_ASSERT_X(obj, "Connecter", QString("Create object failed: " + className).toStdString().c_str());
    /*
    const QMetaObject* metaObject = obj->metaObject();
    QStringList methods;
    for(int i = metaObject->methodOffset(); i < metaObject->methodCount(); ++i)
        methods << QString::fromLatin1(metaObject->method(i).methodSignature());
    qCritical(log) << methods;
    //*/
    if(-1 == obj->metaObject()->indexOfMethod("SetContext(void*)"))
    {
        QString szErr;
        szErr = "The class " + className + " is not method SetContext. It must be SetContext(void*) method.";
        qCritical(log) << szErr;
        Q_ASSERT_X(false, "Connecter", szErr.toStdString().c_str());
    }
    obj->metaObject()->invokeMethod(obj, "SetContext", Q_ARG(void*, pContext));
    if(-1 < obj->metaObject()->indexOfMethod("SetConnecter(CConnecter*)"))
    {
        obj->metaObject()->invokeMethod(obj, "SetConnecter", Q_ARG(CConnecter*, this));
    }

    if(obj->inherits("QDialog"))
    {
        QDialog* pDlg = qobject_cast<QDialog*>(obj);
        pDlg->setAttribute(Qt::WA_DeleteOnClose);
        check = connect(this, SIGNAL(sigDisconnected()),
                        pDlg, SLOT(reject()));
        Q_ASSERT(check);
        nRet = pDlg->exec();
    } else if(obj->inherits("QWidget")) {
        QWidget* pWdg = qobject_cast<QWidget*>(obj);
        pWdg->setAttribute(Qt::WA_DeleteOnClose);
        check = connect(this, SIGNAL(sigDisconnected()),
                        pWdg, SLOT(close()));
        Q_ASSERT(check);
        pWdg->show();
    }
}

void CConnecter::slotBlockShowMessageBox(const QString &szTitle, const QString &szMessage,
                                         QMessageBox::StandardButtons buttons,
                                         QMessageBox::StandardButton &nRet,
                                         bool &checkBox,
                                         QString szCheckBoxContext)
{
    QCheckBox* pBox = nullptr;
    QMessageBox msg(QMessageBox::Information,
                    szTitle, szMessage, buttons, GetViewer());
    if(!szCheckBoxContext.isEmpty())
    {
        pBox = new QCheckBox(szCheckBoxContext);
        if(pBox)
            pBox->setCheckable(true);
        msg.setCheckBox(pBox);
    }
    nRet = (QMessageBox::StandardButton)msg.exec();
    if(pBox)
        checkBox = pBox->isChecked();
}

void CConnecter::slotBlockInputDialog(const QString &szTitle, const QString &szLable,
                                      const QString &szMessage,
                                      QString& szText)
{
    bool ok = false;
    QString t = QInputDialog::getText(nullptr,
                                      szTitle,
                                      szLable,
                                      QLineEdit::Normal,
                                      szMessage,
                                      &ok);
    if(ok && !t.isEmpty())
        szText = t;
}
