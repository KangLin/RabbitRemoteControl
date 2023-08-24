// Author: Kang Lin <kl222@126.com>

#include "Connecter.h"
#include "PluginClient.h"
#include "RabbitCommonDir.h"
#ifdef BUILD_QUIWidget
    #include "QUIWidget/QUIWidget.h"
#endif
#include "ParameterConnecter.h"

#include <QClipboard>
#include <QApplication>
#include <QDebug>
#include <QGenericArgument>
#include <QRegularExpression>
#include <QCheckBox>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(Client)

CConnecter::CConnecter(CPluginClient *parent) : QObject(parent),
    m_pPluginClient(parent),
    m_pParameter(nullptr)
{
    /* 因为断连会从用户调用或者远程端断开连接或出错
     * 从用户端调用会直接调用 DisConnect
     * 从远程端断开连接或出错，则会触发 sigDisconnected 而不会调用 DisConnect，所以需要在此关联
     * 这里隐式是第一个调用，否则对象先释放，则从远程端断连则不会执行。
     */
    bool check = connect(this, SIGNAL(sigDisconnected()),
                         this, SLOT(DisConnect()));
    Q_ASSERT(check);
    if(QApplication::clipboard())
    {
        check = connect(QApplication::clipboard(), SIGNAL(dataChanged()),
                        this, SIGNAL(sigClipBoardChanged()));
        Q_ASSERT(check);
    }
}

CConnecter::~CConnecter()
{
    qDebug(Client) << "CConnecter::~CConnecter";
}

const QString CConnecter::Id()
{
    QString szId = Protocol() + "_" + m_pPluginClient->Name();
    if(GetParameter())
    {
        if(!GetParameter()->GetName().isEmpty())
            szId += "_" + GetParameter()->GetName();
        if(!GetParameter()->GetHost().isEmpty())
            szId += "_" + GetParameter()->GetHost()
                    + "_" + QString::number(GetParameter()->GetPort());
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
    if(GetParameter() && GetParameter()->m_pParameterClient
        && GetParameter()->m_pParameterClient->GetShowProtocolPrefix())
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
            + tr("Protocol: ") + Protocol() + "\n"
            + tr("Server name: ") + ServerName() + "\n"
            + tr("Description: ") + m_pPluginClient->Description();
}

const QString CConnecter::Protocol() const
{
    return m_pPluginClient->Protocol();
}

const QIcon CConnecter::Icon() const
{
    return m_pPluginClient->Icon();
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
    if(GetParameter() && GetParameter()->m_pParameterClient
        && GetParameter()->m_pParameterClient->GetShowIpPortInName())
    {
        return GetParameter()->GetHost()
               + ":" + QString::number(GetParameter()->GetPort());
    }

    if(m_szServerName.isEmpty() && GetParameter())
        return GetParameter()->GetServerName();
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
        p->setWindowTitle(tr("Set ") + m_pPluginClient->DisplayName());
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
        qCritical(Client) << "The Protocol [" << Protocol() << "] don't settings dialog";
    }
    return nRet;
}

int CConnecter::Load(QString szFile)
{
    Q_ASSERT(!szFile.isEmpty());
    if(szFile.isEmpty())
    {
        qCritical(Client) << "The load file is empty";
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
        qCritical(Client) << "The load file is empty";
        return -1;
    }    
    QSettings set(szFile, QSettings::IniFormat);
    return Save(set);
}

CParameterConnecter* CConnecter::GetParameter()
{
    return m_pParameter;
}

int CConnecter::SetParameterClient(CParameterClient* pPara)
{
    if(GetParameter())
    {
        GetParameter()->m_pParameterClient = pPara;
        if(pPara)
        {
            GetParameter()->SetSavePassword(pPara->GetSavePassword());
            bool check = connect(pPara, SIGNAL(sigShowProtocolPrefixChanged()),
                                 this, SLOT(slotUpdateName()));
            Q_ASSERT(check);
            check = connect(pPara, SIGNAL(sigSHowIpPortInNameChanged()),
                            this, SLOT(slotUpdateName()));
            Q_ASSERT(check);
        }
        return 0;
    } else {
        qCritical(Client) << "The CConnecter is not parameters! please create parameters."
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

int CConnecter::SetParameter(CParameterConnecter *p)
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
        qCritical(Client) << className << " is QMetaType::UnknownType";
        return nullptr;
    }
    QObject *obj = (QObject*)QMetaType::create(type);
    if(nullptr == obj)
    {
        qCritical(Client) << "QMetaType::create fail: " << type;
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
    /*
    if(-1 == obj->metaObject()->indexOfMethod("SetContext"))
    {
        qCritical(Client) << "The class" << className << "is not method" << "SetContext"
                          << "It must be SetContext and SetConnecter method.";
        Q_ASSERT(false);
    }
    if(-1 == obj->metaObject()->indexOfMethod("SetConnecter"))
    {
        qCritical(Client) << "The class" << className << "is not method" << "SetConnecter"
            << "It must be SetContext and SetConnecter method.";
        Q_ASSERT(false);
    } //*/
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
    } else if(obj->inherits("QWidget")) {
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
                                      QMessageBox::StandardButton &nRet,
                                      bool &checkBox,
                                      QString szCheckBoxContext)
{
    QCheckBox* pBox = nullptr;
    QMessageBox msg(QMessageBox::Information, title, message, buttons, GetViewer());
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
